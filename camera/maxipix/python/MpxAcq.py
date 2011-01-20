############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2011
# European Synchrotron Radiation Facility
# BP 220, Grenoble 38043
# FRANCE
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################
import os.path
import gc
import types

from Lima.Core import *
from Lima import Espia

from MpxCommon import *

import MpxDacs
import MpxDetConfig
import MpxChipConfig


class MpxAcq:
    DEB_CLASS(DebModApplication, "MpxAcq")
    
    @DEB_MEMBER_FUNCT
    def __init__(self, espia_dev_nb=None):
    
        self.__cam_inited = False
	
	if espia_dev_nb is not None:	
	    self.init(espia_dev_nb)

    @DEB_MEMBER_FUNCT
    def init(self, espia_dev_nb):	
	self.__edev= Espia.Dev(espia_dev_nb)
	self.__edev.resetLink()
	self.__eser= Espia.SerialLine(self.__edev)
	self.__eacq= Espia.Acq(self.__edev)

	self.__ebuf= Espia.BufferMgr(self.__eacq)
	self.__mbuf= BufferCtrlMgr(self.__ebuf)

	self.__pser= Maxipix.PriamSerial(self.__eser)
	self.__pacq= Maxipix.PriamAcq(self.__pser)
	self.__mdet= Maxipix.MaxipixDet()

	self.__hwInt= Maxipix.Interface(self.__eacq, self.__mbuf, 
					self.__pacq, self.__mdet)
	self.__ct= CtControl(self.__hwInt)
	self.__reconstruct= None
	self.__reconstructType = Maxipix.MaxipixReconstruction.RAW	
	
	self.cfgPath= None
	self.mpxCfg= None
	self.mpxDacs= None
	self.priamPorts= None

        self.mpxFillModes = {'RAW':Maxipix.MaxipixReconstruction.RAW,
                             'ZERO':Maxipix.MaxipixReconstruction.ZERO,
                             'DISPATCH':Maxipix.MaxipixReconstruction.DISPATCH,
                             'MEAN':Maxipix.MaxipixReconstruction.MEAN,
                             }
        
        self.__cam_inited = True
	
    @DEB_MEMBER_FUNCT
    def __del__(self):
        if self.__cam_inited:
            del self.__ct; gc.collect()
            if self.__reconstruct is not None:
                del self.__reconstruct
            del self.__hwInt; gc.collect()
            del self.__mdet; gc.collect()
            del self.__pacq; gc.collect()
            del self.__pser; gc.collect()
            del self.__mbuf; gc.collect()
            del self.__ebuf; gc.collect()
            del self.__eacq; gc.collect()
            del self.__eser; gc.collect()
            del self.__edev; gc.collect()


    def getControl(self):
        if self.__cam_inited:
	    return self.__ct
	else:
	    raise MpxError("init() method must be called first")

    def getInterface(self):
        if self.__cam_inited:
            return self.__hwInt
	else:
	    raise MpxError("init() method must be called first")

    def getPriamAcq(self):
        if self.__cam_inited:
	    return self.__pacq
	else:
	    raise MpxError("init() method must be called first")

    def getPriamSerial(self):
        if self.__cam_inited:
           return self.__pser
        else:
	    raise MpxError("init() method must be called first")


    def getEspiaAcq(self):
        if self.__cam_inited:
            return self.__eacq
	else:
	    raise MpxError("init() method must be called first")

    def getEspiaSerial(self):
        if self.__cam_inited:
            return self.__eser
	else:
	    raise MpxError("init() method must be called first")

    def getEspiaDev(self):
        if self.__cam_inited:
            return self.__edev
	else:
	    raise MpxError("init() method must be called first")

    @DEB_MEMBER_FUNCT
    def setFillMode(self, fillMode):
        
        if self.__cam_inited:
            if self.__reconstruct:
                if fillMode not in self.mpxFillModes.values():
                    raise MpxError("invalid reconstruction fill mode %d"%fillMode)
                else:
                    self.__reconstructType = fillMode
                    self.__reconstruct.setType(fillMode)		 
	else:
	    raise MpxError("init() method must be called first")

    @DEB_MEMBER_FUNCT
    def getFillMode(self):
        if self.__cam_inited:
            return self.__reconstructType
	else:
	    raise MpxError("init() method must be called first")


    @DEB_MEMBER_FUNCT
    def setPath(self, path):
	spath= os.path.normpath(path)
        if not os.path.isdir(spath):
            raise MpxError("<%s> is not a directory"%path)
        if not os.access(spath, os.R_OK):
            raise MpxError("no read permission on <%s>"%path)
        self.cfgPath= spath

    @DEB_MEMBER_FUNCT
    def loadConfig(self, name):
	self.loadDetConfig(name)
	self.loadChipConfig(name)
        # Need to inform afterward the hwInterface about new ranges
        # which are calculated once the configs have been loaded.
        # By callback the CtAcquisition will be refreshed too.
        self.__hwInt.updateValidRanges()
        

    @DEB_MEMBER_FUNCT
    def loadDetConfig(self, name):
	detConfig= MpxDetConfig.MpxDetConfig()
	detConfig.setPath(self.cfgPath)
	print "Loading Detector Config <%s> ..."%name
	detConfig.loadConfig(name)

	self.cfgName= detConfig.getName()
	self.cfgFilename= detConfig.getFilename()

	self.mpxCfg= detConfig.getMpxCfg()
	self.__mdet.setVersion(self.mpxCfg["version"])
	self.__mdet.setNbChip(self.mpxCfg["xchip"], self.mpxCfg["ychip"])
	self.__mdet.setPixelGap(self.mpxCfg["xgap"], self.mpxCfg["ygap"])

	self.priamPorts= detConfig.getPriamPorts()
	self.mpxDacs= detConfig.getDacs()

	print "Setting PRIAM configuration ..."
	self.__pacq.setup(self.mpxCfg["version"], self.mpxCfg["polarity"],
			  self.mpxCfg["frequency"], self.mpxDacs.getFsrString(1))
        
	self.__pacq.setParalellReadout(self.priamPorts)
	self.__pacq.setImageMode(Maxipix.PriamAcq.NORMAL)
	self.__pacq.setGateMode(Maxipix.PriamAcq.INACTIVE)
	self.__pacq.setShutterMode(Maxipix.PriamAcq.SEQUENCE)
	self.__pacq.setIntervalTime(0.)
	self.__pacq.setShutterTime(0.)

	self.applyChipFsr(0)

	if self.__mdet.needReconstruction():
	    print "Setting image reconstruction ..."
	    model= self.__mdet.getReconstruction()
	    if self.__reconstruct is not None:
	    	self.__ct.setReconstructionTask(None)
		del self.__reconstruct
	    self.__reconstruct= Maxipix.MaxipixReconstruction()
	    self.__reconstruct.setModel(model)
	    self.setFillMode(Maxipix.MaxipixReconstruction.RAW)	    
	    self.__reconstruct.setXnYGapSpace(self.mpxCfg["xgap"], 
						self.mpxCfg["ygap"])
	    self.__ct.setReconstructionTask(self.__reconstruct)
	else:
	    self.__ct.setReconstructionTask(None)

    @DEB_MEMBER_FUNCT
    def loadChipConfig(self, name):
	self.chipCfg= MpxChipConfig.MpxPixelConfig(self.mpxCfg["version"], 
						self.mpxCfg["nchip"])
	self.chipCfg.setPath(self.cfgPath)
	self.chipCfg.loadConfig(name)

	self.applyPixelConfig(0)

    @DEB_MEMBER_FUNCT
    def applyPixelConfig(self, chipid):
	if chipid==0:
	    for idx in range(self.mpxCfg["nchip"]):
	        scfg= self.chipCfg.getMpxString(idx+1)
	        print "Loading Chip Config #%d ..."%(idx+1)
	        self.__pacq.setChipCfg(self.priamPorts[idx], scfg)
	else:
	    port= self.__getPriamPort(chipid)
	    scfg= self.chipCfg.getMpxString(chipid)
	    print "Loading Chip Config #%d ..."%(chipid)
	    self.__pacq.setChipCfg(port, scfg)
	     
    @DEB_MEMBER_FUNCT
    def applyChipFsr(self, chipid):
	if chipid==0:
	    for idx in range(self.mpxCfg["nchip"]):
		sfsr= self.mpxDacs.getFsrString(idx+1)
	        print "Loading Chip FSR #%d ..."%(idx+1)
		self.__pacq.setChipFsr(self.priamPorts[idx], sfsr)
	else:
	    port= self.__getPriamPort(chipid)
	    sfsr= self.mpxDacs.getFsrString(chipid)
	    print "Loading Chip FSR #%d ..."%(chipid)
	    self.__pacq.setChipFsr(port, sfsr)

    def __getPriamPort(self, chipid):
	if chipid <= 0 or chipid > self.mpxCfg["nchip"]:
            raise MpxError("<%d> is not a valid chipID"%chipid)
	return self.priamPorts[chipid-1]
	
