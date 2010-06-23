import os.path
import gc

from Lima.Core import *
from Lima import Espia

from MpxCommon import *

import MpxDacs
import MpxDetConfig
import MpxChipConfig


class MpxAcq:
    def __init__(self, espia_dev_nr):
	self.__edev= Espia.Dev(0)
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

	self.cfgPath= None
	self.mpxCfg= None
	self.mpxDacs= None
	self.priamPorts= None

    def __del__(self):
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
	return self.__ct

    def getInterface(self):
	return self.__hwInt

    def getPriamAcq(self):
	return self.__pacq

    def getPriamSerial(self):
	return self.__pser

    def getEspiaAcq(self):
	return self.__eacq

    def getEspiaSerial(self):
	return self.__eser

    def getEspiaDev(self):
	return self.__edev

    def setPath(self, path):
	spath= os.path.normpath(path)
        if not os.path.isdir(spath):
            raise MpxError("<%s> is not a directory"%path)
        if not os.access(spath, os.R_OK):
            raise MpxError("no read permission on <%s>"%path)
        self.cfgPath= spath

    def loadConfig(self, name):
	self.loadDetConfig(name)
	self.loadChipConfig(name)

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

	if self.__mdet.needReconstruction():
	    print "Setting image reconstruction ..."
	    model= self.__mdet.getReconstruction()
	    self.__reconstruct= Maxipix.MaxipixReconstruction()
	    self.__reconstruct.setModel(model)
	    self.__reconstruct.setType(Maxipix.MaxipixReconstruction.ZERO)
	    self.__reconstruct.setXnYGapSpace(self.mpxCfg["xgap"], 
						self.mpxCfg["ygap"])
	    self.__ct.setReconstructionTask(self.__reconstruct)
	else:
	    self.__ct.setReconstructionTask(None)

    def loadChipConfig(self, name):
	self.chipCfg= MpxChipConfig.MpxPixelConfig(self.mpxCfg["version"], 
						self.mpxCfg["nchip"])
	self.chipCfg.setPath(self.cfgPath)
	self.chipCfg.loadConfig(name)

	for idx in range(self.mpxCfg["nchip"]):
	    scfg= self.chipCfg.getMpxString(idx+1)
	    print "Loading Chip Config <%s> #%d ..."%(name, idx)
	    self.__pacq.setChipCfg(self.priamPorts[idx], scfg)

