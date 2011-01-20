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
import os
import ConfigDict
import MpxDacs
from MpxCommon import *

class MpxDetConfig:

    def __init__(self, path=None, name=None):
	self.path= None
	self.reset()

	if path is not None:
	    self.setPath(path)
	if name is not None:
	    self.loadConfig(name)

    def reset(self):
	self.name= None
	self.cfgFile= None

	self.mpxCfg= None
	self.priamPorts= None
	self.dacs= None

    def setPath(self, path):
	spath= os.path.normpath(path)
	if not os.path.isdir(spath):
	    raise MpxError("<%s> is not a directory"%path)
	if not os.access(spath, os.R_OK):
	    raise MpxError("no read permission on <%s>"%path)
	self.path= spath

    def loadConfig(self, name):
	cfgFile= self.__getConfigFile(name)
	self.loadDetectorConfig(cfgFile)
	self.cfgFile= cfgFile
	self.name= name

    def __getConfigFile(self, name):
	fname= "%s.cfg"%name
	if self.path is not None:
	    fname= "%s/%s"%(self.path, fname)
	return fname

    def __checkConfigFile(self, fname):
	if not os.path.isfile(fname):
            raise MpxError("<%s> is not a valid file"%fname)
        if not os.access(fname, os.R_OK):
            raise MpxError("No read permission on <%s>"%fname)
        return fname

    def __setParamError(self, msg):
	txt = "ConfigFile <%s>"%self.cfgFile
	if self.__section is not None:
	    txt= txt + " : Section <%s>"%self.__section
	txt= txt + " : " + msg
	raise MpxError(txt)

    def __getParamNeeded(self, pars, name, chklist=None):
	if not pars.has_key(name):
	    self.__setParamError("Missing mandatory parameter <%s>"%name)
	param= pars[name]
	if chklist is not None:
	    if param not in chklist:
		self.__setParamError("<%s> has an invalid value. Should be in %s"%(name, str(chklist)))
	return param

    def __getParamOptional(self, pars, name, chklist=None, default=None):
	if not pars.has_key(name):
	    if default is not None:
		return default
	    else:
	        return None
	param= pars[name]
	if chklist is not None:
	    if param not in chklist:
		self.__setParamError("<%s> has an invalid value. Should be in %s"%(name, str(chklist)))
	return param

    def loadDetectorConfig(self, fname):
	self.cfgFile= self.__checkConfigFile(fname)
	cfg= ConfigDict.ConfigDict()
	cfg.read(fname)

	self.__section= None
	self.__parseMaxipixSection(cfg)
	self.__parsePriamSection(cfg)
	self.__parseDacsSection(cfg)
	self.__parseThresholdSection(cfg)

    def __parseMaxipixSection(self, cfg):
	if not cfg.has_key("maxipix"):
	    self.__setParamError("No <maxipix> section found")
	pars= cfg["maxipix"]
	self.__section= "maxipix"
	self.mpxCfg= {}
	self.mpxCfg["type"]= self.__getParamNeeded(pars, "type", MpxTypes)
	self.mpxCfg["version"]= mpxVersion(self.mpxCfg["type"])
	self.mpxCfg["polarity"]= mpxPolarity(self.__getParamNeeded(pars, "polarity", MpxPolarityTypes))
	self.mpxCfg["frequency"]= self.__getParamNeeded(pars, "frequency")
	
	self.mpxCfg["xchip"]= self.__getParamNeeded(pars, "xchip", range(1,6))
	self.mpxCfg["ychip"]= self.__getParamOptional(pars, "ychip", [1,2], 1)
	self.mpxCfg["nchip"]= self.mpxCfg["xchip"]*self.mpxCfg["ychip"]

	self.mpxCfg["xgap"]= self.__getParamOptional(pars, "xgap", None, 0)
	self.mpxCfg["ygap"]= self.__getParamOptional(pars, "ygap", None, 0)
        
    def __parsePriamSection(self, cfg):
	self.priamPorts= range(self.mpxCfg["nchip"])
	if self.mpxCfg["xchip"]==2 and self.mpxCfg["ychip"]==2:
	    self.priamPorts[0]= 1
	    self.priamPorts[1]= 2
	    self.priamPorts[2]= 0
	    self.priamPorts[3]= 3

	if cfg.has_key("priam"):
	    pars= cfg["priam"]
	    self.__section= "priam"
	    for idx in range(self.mpxCfg["nchip"]):
		name= "chip_%d"%(idx+1)
		self.priamPorts[idx]= self.__getParamOptional(pars, name, 
						range(5), self.priamPorts[idx])

    def __parseThresholdSection(self, cfg):
	self.__section= "threshold"
	pars= cfg.get("threshold", {})
	thlnoise= []
	for idx in range(self.mpxCfg["nchip"]):
	    name= "thlnoise_%d"%(idx+1)
	    thlnoise.append(self.__getParamOptional(pars, name, None, 0))
	estep= self.__getParamOptional(pars, "estep", None, 0)
	e0thl= self.__getParamOptional(pars, "e0thl", None, 0)

	thl= self.dacs.getOneDac(1, "thl")
	self.dacs.setThlNoise(0, thlnoise)
	self.dacs.setECalibration(e0thl, estep)
	self.dacs.setThl(thl)

    def __parseDacsSection(self, cfg):
	self.dacs= MpxDacs.MpxDacs(self.mpxCfg["version"], self.mpxCfg["nchip"])
	self.__section= "dacs"
	if cfg.has_key("dacs"):
	    pars= cfg["dacs"]
	    setDacs= {}
	    fsrKeys= MpxDacs.getMpxFsrDef(self.mpxCfg["version"]).listKeys()
	    for key in pars.keys():
		if key not in fsrKeys:
		    self.__setParamError("Invalid key <%s>"%key)
		else:
		    setDacs[key]= pars[key]
	    self.dacs.setDacs(0, setDacs)

    def getPath(self):
	return self.path

    def getFilename(self):
	return self.cfgFile

    def getName(self):
	return self.name

    def getMpxCfg(self):
	return self.mpxCfg

    def getPriamPorts(self):
	return self.priamPorts

    def getDacs(self):
	return self.dacs

if __name__=="__main__":
    import sys
    if len(sys.argv)!=3:
	print "Usage: %s <path> <config_name>"%sys.argv[0]
    else:
	def printDict(pars):
	    for (key,val) in pars.items():
		print "\t", key, "=", val
	    print

	cfg= MpxDetConfig(path= sys.argv[1], name= sys.argv[2])
	print
	print "> Path       =", cfg.getPath()
	print "> ConfigName =", cfg.getName()
	print "> FileName   =", cfg.getFilename()
	print 
	print "[maxipix]"
	printDict(cfg.getMpxCfg())
	print "[priam]"
	print "ports =", str(cfg.getPriamPorts())
	print "[threshold]"
	print "thlnoise = ", str(cfg.getDacs().getThlNoise(0))
	(e0thl, estep)= cfg.getDacs().getECalibration()
	print "e0thl = ", e0thl
	print "estep = ", estep
	print "[dacs]"
	printDict(cfg.getDacs().getDacs(0))

