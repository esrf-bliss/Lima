import os, os.path
import string
import copy
import types

from MpxCommon import *

_MpxFsrDefInst_= None

def getMpxFsrDef(version):
    global _MpxFsrDefInst_
    if _MpxFsrDefInst_ is None:
	_MpxFsrDefInst_= _MpxFsrDef(version)
    if _MpxFsrDefInst_.version != version:
	_MpxFsrDefInst_= _MpxFsrDef(version)
    return _MpxFsrDefInst_

class _MpxFsrDef:
    def __init__(self, version):
	self.version= version
	self.__fsrKeys= {}
	self.__dacCode= {}
	self.__default= {}

	if self.version == 0:
	    self.__initDUMMY()
	elif self.version==1:
	    self.__initMPX2()
	elif self.version==2:
	    self.__initMXR2()
	elif self.version==3:
	    self.__initTPX1()

    def listKeys(self, saved=1):
	if saved:
	    return [ key for key in self.__fsrKeys.keys() 
			if key not in ["daccode", "sensedac", "dacsel"] ]
	return self.__fsrKeys.keys()

    def fsrKeys(self):
	return self.__fsrKeys

    def dacCode(self):
	return self.__dacCode

    def default(self):
	return self.__default

    def __initDUMMY(self):
	self.__fsrKeys= { "testa": [(3,4)],
			"testb": [(8, 15)],
			"testc": [(32, 35), (44, 47)]
	}

    def __initMPX2(self):
        self.__fsrKeys= { "biasdelayn": [(3,10)],
			"biasdisc": [(11,18)],
		 	"biaspreamp": [(19,26)],
		 	"biassetdisc": [(45,48), (55,58)],
		 	"biasths": [(59,66)],
		 	"biasikrum": [(99,106)],
		 	"biasabuffer": [(107,114)],
		 	"thh": [(115,122)],
		 	"thl": [(123,130)],
		 	"fbk": [(131,138)],
		 	"gnd": [(180,187)],
		 	"biaslvdstx": [(226,233)],
		 	"reflvdstx": [(234,241)],
		 	"daccode": [(37,38), (40,40), (41,41)],
		 	"sensedac": [(42,42)],
		 	"dacsel": [(43,43)],
	}
    	self.__default= { "biasdelayn": 0,
                 	"biasdisc": 128,
                 	"biaspreamp": 60,
                 	"biassetdisc": 240,
                 	"biasths": 150,
                 	"biasikrum": 200,
                 	"biasabuffer": 255,
                 	"fbk": 180,
                 	"gnd": 128,
                 	"biaslvdstx": 128,
                 	"reflvdstx": 128,
		 	"thh": 0,
		 	"thl": 6600,
	}
	self.__dacCode= { "biasdelayn": 0x1,
			"biasdisc": 0x2,
			"biaspreamp": 0x3,
			"biassetdisc": 0x4,
			"biasths": 0x5,
			"biasikrumhalf": 0x8,
			"biasikrum": 0x7,
			"biasabuffer": 0xe,
			"thh": 0xc,
			"thl": 0xb,
			"fbk": 0xa,
			"gnd": 0xd,
			"biaslvdstx": 0x6,
			"reflvdstx": 0x9
	}

    def __initMXR2(self):
    	self.__fsrKeys= { "ikrum": [(3,10)],
		 	"disc": [(11,18)],
		 	"preamp": [(19,26)],
		 	"daccode": [(37,38), (40,41)],
		 	"sensedac": [(42,42)],
		 	"dacsel": [(43,43)],
		 	"buffanaloga": [(45,48),(55,58)],
		 	"buffanalogb": [(59,66)],
		 	"delayn": [(85, 92)],
		 	"thl": [(99, 112)],
		 	"thh": [(113, 126)],
		 	"fbk": [(127, 134)],
		 	"gnd": [(135, 142)],
		 	"ctpr": [(143, 174)],
		 	"ths": [(180, 187)],
		 	"biaslvds": [(226, 233)],
		 	"reflvds": [(234, 241)],
	}
    	self.__default= { "ikrum": 200,
                 	"disc": 250,
                 	"preamp": 128,
                 	"buffanaloga": 128,
                 	"buffanalogb": 128,
                 	"delayn": 128,
                 	"fbk": 115,
                 	"gnd": 128,
                 	"ths": 180,
                 	"biaslvds": 128,
                 	"reflvds": 129,
		 	"thh": 0,
		 	"thl": 6600,
	}
	self.__dacCode= { "ikrum": 0xf,
			"disc": 0xb,
			"preamp": 0x7,
			"buffanaloga": 0x3,
			"buffanalogb": 0x4,
			"delayn": 0x9,
			"thl": 0x6,
			"thh": 0xc,
			"fbk": 0xa,
			"gnd": 0xd,
			"ths": 0x1,
			"biaslvds": 0x2,
			"reflvds": 0xe
	}


    def __initTPX1(self):
    	self.__fsrKeys= { "ikrum": [(4,11)],
		 	"disc": [(12,19)],
		 	"preamp": [(20,27)],
		 	"daccode": [(38,39),(41,42)],
		 	"sensedac": [(43,43)],
		 	"dacsel": [(44,44)],
		 	"buffanaloga": [(46,49),(56,59)],
		 	"buffanalogb": [(60,67)],
		 	"hist": [(86,93)],
		 	"thl": [(100,113)],
		 	"vcas": [(120,127)],
		 	"fbk": [(128,135)],
		 	"gnd": [(136,143)],
		 	"ctpr": [(144,175)],
		 	"ths": [(181,188)],
		 	"biaslvds": [(227,234)],
		 	"reflvds": [(235,242)],
	}
    	self.__default= { "ikrum": 200,
                 	"disc": 250,
                 	"preamp": 128,
                 	"buffanaloga": 128,
                 	"buffanalogb": 128,
                 	"hist": 128,
                 	"vcas": 128,
                 	"fbk": 115,
                 	"gnd": 128,
                 	"ths": 180,
                 	"biaslvds": 128,
                 	"reflvds": 129,
		 	"thl": 6600,
	}
	self.__dacCode= { "ikrum": 0xf,
			"disc": 0xb,
			"preamp": 0x7,
			"buffanaloga": 0x3,
			"buffanalogb": 0x4,
			"hist": 0x9,
			"thl": 0x6,
			"vcas": 0xc,
			"fbk": 0xa,
			"gnd": 0xd,
			"ths": 0x1,
			"biaslvds": 0x2,
			"reflvds": 0xe
	}

class MpxChipDacs:
    def __init__(self, version):
	self.version= version
	self.reset()

    def reset(self):
	self.__data= {}
	for key in getMpxFsrDef(self.version).listKeys():
	    self.__data[key]= 0
	self.__data.update(getMpxFsrDef(self.version).default())

    def getOneDac(self, name):
	return self.__data.get(string.lower(name), None)

    def setOneDac(self, name, value):
	self.__setValue(name, value)

    def getDacs(self):
	return copy.deepcopy(self.__data)

    def setDacs(self, dacs):
	for (key, val) in dacs.items():
	    self.__setValue(key, val)

    def __setValue(self, name, value):
	lower= string.lower(name)
	if lower not in getMpxFsrDef(self.version).listKeys(0):
	    MpxError("<%s> is not a valid dac parameter"%name)

	if lower=="daccode":
	    self.__data[lower]= self.dacCode(value)
	else:
	    try:
		ivalue= int(value)
	    except:
		MpxError("<%s> should be an integer value"%str(value))
	    self.__data[lower]= ivalue

    def dacCode(self, code):
	dacs= getMpxFsrDef(self.version).dacCode()
	if type(code)==types.StringType:
	    name= string.lower(code)
	    if dacs.has_key(name):
		return dacs[name]
	    else:
		raise MpxError("<%s> is not a valid dac name"%code)
	else:
	    if code in dacs.values():
		return code
	    else:
		raise MpxError("<%s> is not a valid dac code"%str(code))

    def getFsrString(self):
	fsrDef= getMpxFsrDef(self.version).fsrKeys()
        fsrInt= [ 0 for idx in range(32) ]
	for (key,val) in self.__data.items():
            base= 0
            for (lsb,msb) in fsrDef[key]:
                for idx in range(msb-lsb+1):
                    if val&(1<<(base+idx)):
                        fsrInt[31-(lsb+idx)/8]|=1<<((lsb+idx)%8)
                base+= (msb-lsb+1)
        fsrStr= string.join([ "%c"%i for i in fsrInt ], "")
        return fsrStr


class MpxDacs:
    def __init__(self, version, nchip):
	self.version= version
	self.nchip= nchip
	self.reset()

    def reset(self):
	self.__dacs= []
	self.__thlnoise= []
	for ichip in range(self.nchip):
	    self.__dacs.append(MpxChipDacs(self.version))
	    self.__thlnoise.append(0)

	self.__e0thl= 0
	self.__estep= 0.

    def __getChipIdx(self, chipid):
	if chipid == 0:
	    return range(self.nchip)
	else:
	    if chipid not in range(1, self.nchip+1):
	        raise MpxError("Invalid chipid <%s>. Range is [1,%d]"%(chipid, self.nchip+1))
	    return [ chipid-1 ]

    def getFsrString(self, chipid):
	idx= self.__getChipIdx(chipid)
	return self.__dacs[idx].getFsrString()

    def setThlNoise(self, chipid, value):
	for idx in self.__getChipIdx(chipid):
	    self.__thlnoise[idx]= value

    def getThlNoise(self, chipid):
	if chipid == 0:
	    return self.__thlnoise
	else:
	    idx= self.__getChipIdx(chipid)
	    return self.__thlnoise[idx[0]]

    def setECalibrarion(self, e0thl, estep):
	self.__e0thl= e0thl
	self.__estep= estep

    def getECalibrarion(self, e0thl, step):
	return (self.__e0thl, self.__estep)

    def setThl(self, value):
	for idx in range(self.nchip):
	    val= value
	    if idx>0 and self.__thlnoise[0]>0 and self.__thlnoise[idx]>0:
		val= val + thlnoise[idx] - thlnoise[0]
	    self.__dacs[idx].setDac("thl", val)

    def getThl(self):
	pass

    def setEThl(self, value):
	if self.__e0thl==0. or self.__estep==0:
	    raise MpxError("No energy calibration for THL")
	val= value / self.__estep
	ival= int(round(val)) + self.__e0thl
	self.setThl(ival)

    def getEThl(self):
	pass

    def setOneDac(self, chipid, name, value):
	for idx in self.__getChipIdx(chipid):
	    self.__dacs[idx].setOneDac(name, value)

    def setDacs(self, chipid, dacs):
	for idx in self.__getChipIdx(chipid):
	    self.__dacs[idx].setDacs(name, value)
	
    def getOneDac(self, chipid, name):
	dacs= []
	for idx in self.__getChipIdx(chipid):
	    dacs.append(self.getOneDac(idx, name))
	res= dacs[0]
	if chipid == 0 and self.nchip > 1:
	    for idx in range(1, self.nchip):
		if chip[idx]!=res:
		    return None
	return res

    def getDacs(self, chipid):
	dacs= []
	for idx in self.__getChipIdx(chipid):
	    dacs.append(self.getDacs())
	res= dacs[0]
	if chipid == 0 and self.nchip > 1:
	    for (key, val) in dacs[0].items():
		for idx in range(1, self.nchip):
		    if dacs[idx][key] != val:
		        del res[key]
	    if not len(res.keys()):
	   	return None
	return res
	   

