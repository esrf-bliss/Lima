import os, os.path
import string
import numpy
import EdfFile

from MpxCommon import *


TimePixModes= {
	"medipix": 0,
	"tot": 1,
	"timepix-1hit": 2,
	"timepix": 3
}

class MpxPixelConfig:
    def __init__(self, version, nchip):
	self.version= mpxVersion(version)
	self.nchip= nchip

	self.path= None
	self.name= None

	self.reset()

    def reset(self):
	self.__arr= []
	for idx in range(self.nchip):
	    self.__arr.append(MpxPixelArray(self.version))
	self.name= None

    def setPath(self, path):
	spath= os.path.normpath(path)
       	if not os.path.isdir(spath):
            raise MpxError("<%s> is not a directory"%path)
        if not os.access(spath, os.R_OK):
            raise MpxError("no read permission on <%s>"%path)
        self.path= spath

    def loadConfig(self, name):
	files= []
	for idx in range(self.nchip):
	    files.append(self.__getConfigFile(name, idx+1))
	for idx in range(self.nchip):
	    self.__arr[idx].load(files[idx])

    def __getConfigFile(self, name, chip):
	root= "%s_chip_%d"%(name, chip)
	if self.path is not None:
	    root= "%s/%s"%(self.path, root)
	for ext in ["edf", "bpc"]:
	    fname= "%s.%s"%(root, ext)
	    if os.path.isfile(fname):
		return fname
	raise MpxError("No config file found for <%s>"%root)

    def __getChipIdx(self, chipid, notzero=0):
        if chipid == 0:
	    if notzero:
		raise MpxError("Must specify chipID")
            return range(self.nchip)
        else:
            if chipid not in range(1, self.nchip+1):
                raise MpxError("Invalid chipid <%s>. Range is [1,%d]"%(chipid, self.nchip+1))
            return [ chipid-1 ]

    def getMpxString(self, chipid):
	idx= self.__getChipIdx(chipid, 1)
	return self.__arr[idx[0]].getMpxString()

    def getChipArray(self, chipid):
	idx= self.__getChipIdx(chipid, 1)
	return self.__arr[idx]

    def setTimePixMode(self, mode):
	for idx in range(self.nchip):
	    self.__arr[idx].setTimePixMode(mode)

    def getTimePixMode(self):
	modes= []
	for idx in range(self.nchip):
	    modes.append(self.__arr[idx].getTimePixMode())
	mode= modes[0]
	if len(modes)>1:
	    for idx in range(1, len(modes)):
		if modes[idx] != mode:
		    raise MpxError("ChipID <%d> does not have same mode (%d<>%d)"%(idx+1, modes[idx], mode))
	return mode

    def setLow2Max(self, chipid):
	for idx in self.__getChipIdx(0, 0):
	    self.__arr[idx].setLow2Max()
    def setLow2Min(self, chipid):
	for idx in self.__getChipIdx(0, 0):
	    self.__arr[idx].setLow2Min()
    def setHigh2Max(self, chipid):
	for idx in self.__getChipIdx(0, 0):
	    self.__arr[idx].setHigh2Max()
    def setHigh2Min(self, chipid):
	for idx in self.__getChipIdx(0, 0):
	    self.__arr[idx].setHigh2Min()

class MpxPixelArray:
    """ Medipix Pixel Configuration Matrix Class

    This class hold pixel configuration matrix for one chip.
    It gives following functionnalities:
    - handles chip types differences : medipix, timepix
    - read from / save to files in EDF and BPC format (pixelman)
    - read an EDF mask only file
    - matrix to string conversion for priam transfer (using PriamArray module)
    - add utility functions to set timepix mode, set threshold to min/max

    Each configuration array are defined as property and can be addressed directly.
    maskArray : mask bit array
    testArray : test bit array
    lowArray : lower threshold array
    highArray : higher threshold array or mode for timepix versions
    """

    SIZE= (256,256)
    __MASK= 0
    __TEST= 1
    __LOW= 2
    __HIGH= 3

    arrayDefs= [ {} for _i_ in range(len(MpxVersion)) ]
    arrayDefs[0]= { "Labels": ["Mask Array", "Test Array", \
				"Low Threshold Array", "High Threshold Array"],
		    "Mask": [0x1, 0x1, 0x7, 0x7],
		    "Depth": [1, 1, 3, 3],
		    "BpcShift": [7, 6, 0, 3]
		  }
    arrayDefs[1]= arrayDefs[0]
    arrayDefs[2]= arrayDefs[0]
    arrayDefs[3]= { "Labels": ["Mask Array", "Test Array", \
				"Threshlod Array", "Pixel Mode Array"],
		    "Mask": [0x1, 0x1, 0xf, 0x3],
		    "Depth": [1, 1, 4, 2],
		    "BpcShift": [7, 6, 0, 4]
		  }

    def __init__(self, version, filename=None):
        """ Initialisation parameters:
        version : chip version as defined in MedipixVersion list
        filename : if a filename is given, tries to load it in either EDF or BPC
                   format depending on file extension
        """
	self.version= mpxVersion(version)

	self.arrayLabels= self.arrayDefs[self.version]["Labels"]
	self.arrayMask= self.arrayDefs[self.version]["Mask"]
	self.arrayDepth= self.arrayDefs[self.version]["Depth"]

	self.__arrays= []
	for idx in range(4):
	    self.__arrays.append(numpy.zeros(self.SIZE, numpy.uint8))
	
	if filename is not None:
	    self.load(filename)

    def reset(self):
        """ Reset all arrays: mask, test, low and high arrays
        """
	for idx in range(4):
	    self.__resetArray(idx)
	
    def getMpxString(self):
        """ Conversion into string needed for Priam transfer
        """
	arr= Maxipix.PixelConfigArray(self.version)
	arr.maskArray= self.__arrays[self.__MASK]
	arr.testArray= self.__arrays[self.__TEST]
	arr.lowArray= self.__arrays[self.__LOW]
	arr.highArray= self.__arrays[self.__HIGH]
	return arr.convert()
		    
    def save(self, filename):
        """ Saves current arrays to file.
        Format is EDF or BPC depending on filename extension (.edf, .bpc)
        """
	filesep= os.path.splitext(filename)
	if filesep[1]==".edf":
	    self.saveEdf(filename)
	elif filesep[1]==".bpc":
	    self.saveBpc(filename)
	else:
	    raise MpxError("Unknown file extension (not .edf or .bpc)")
	self.filename= filename

    def load(self, filename):
        """ Load arrays from file in either EDF or BPC (depending on extension .edf or .bpc)
	"""
	if not os.path.isfile(filename):
	    raise MpxError("Cannot find file <%s>"%filename)
	if not os.path.getsize(filename):
	    raise MpxError("<%s> is empty"%filename)
	filesep= os.path.splitext(filename)
	if filesep[1]==".edf":
	    self.loadEdf(filename)
	elif filesep[1]==".bpc":
	    self.loadBpc(filename)
	else:
	    raise MpxError("Unknown file extension (not .edf or .bpc)")

    def loadBpc(self, filename):
        """ Load a config file in BPC format (pixelman)
        """
	try:
	    file= open(filename, "rb")
	except:
	    raise MpxError("Cannot open <%s> for reading"%filename)

	instr= file.read()
	file.close()

	if len(instr) != self.SIZE[0]*self.SIZE[1]:
	    raise MpxError("<%s> has not the correct size"%filename)

	data= numpy.fromstring(instr, numpy.uint8)
	data= numpy.reshape(data, self.SIZE)

	shift= self.arrayDefs[self.version]["BpcShift"]
	self.reset()
	self.__setArray(self.__MASK, numpy.array((data>>shift[self.__MASK])&self.arrayMask[self.__MASK]))
	self.__setArray(self.__TEST, numpy.array((data>>shift[self.__TEST])&self.arrayMask[self.__TEST]))
	self.__setArray(self.__HIGH, numpy.array((data>>shift[self.__HIGH])&self.arrayMask[self.__HIGH]))
	self.__setArray(self.__LOW, numpy.array(data&self.arrayMask[self.__LOW]))

    def saveBpc(self, filename):
        """ Saves current config in file in BPC format
        """
	shift= self.arrayDefs[self.version]["BpcShift"]
	data= numpy.zeros(self.SIZE, numpy.uint8)
	data= data | ((self.__arrays[self.__MASK]&self.arrayMask[self.__MASK]) << shift[self.__MASK])
	data= data | ((self.__arrays[self.__TEST]&self.arrayMask[self.__TEST]) << shift[self.__TEST])
	data= data | ((self.__arrays[self.__HIGH]&self.arrayMask[self.__HIGH]) << shift[self.__HIGH])
	data= data | (self.__arrays[self.__LOW]&self.arrayMask[self.__LOW])

	file= open(filename, "wb")
	file.write(data.tostring())
	file.close()

    def loadEdf(self, filename):
        """ Load a config file in EDF format
        """
	try:
	    edf= EdfFile.EdfFile(filename)
	except:
	    raise MpxError("<%s> is not in EDF format"%filename)

	if edf.GetNumImages()!=4:
	    raise MpxError("<%s> does not contain 4 images"%filename)

	self.reset()
	for idx in range(4):
	    self.__setArray(idx, edf.GetData(idx))

    def saveEdf(self, filename):
        """ Saves current config file in EDF format
        """
	if os.path.isfile(filename):
	    os.remove(filename)
	edf= EdfFile.EdfFile(filename)
	header= {}
	for idx in range(4):
	    header["MedipixConfig"]= self.arrayLabels[idx]
	    edf.WriteImage(header, self.__arrays[idx], Append=1)

    def loadMask(self, filename):
	""" Load an EDF mask file and sets maskArray (edf with array of values 0/1)
	"""
	try:
	    edf= EdfFile.EdfFile(filename)
	except:
	    raise MpxError("<%s> is not in EDF format"%filename)

        self.__resetMaskArray()
	self.__setMaskArray(edf.GetData(0))

    def saveMask(self, filename):
	""" Saves current mask array in a separate EDF file
	"""
	if os.path.isfile(filename):
	    os.remove(filename)
        edf= EdfFile.EdfFile(filename)
        header= {}
        header["MedipixConfig"]= self.arrayLabels[self.__MASK]
	edf.WriteImage(header, self.__getMaskArray())

    def setTimePixMode(self, mode):
	""" Set timepix mode for all pixels.
	Timepix modes are defined in TimePixModes.
	Argument can be either string or list index
	"""
	if type(mode) is types.StringType:
		smode= string.lower(mode)
		if TimePixModes.has_key(smode):
			imode= TimePixModes[mode]
		else:
			raise MpxError("Unknown TimePix mode <%s>"%mode)
	else:
		imode= int(mode)
		nmode= len(TimePixModes.keys())
		if imode<0 or imode>=nmode:
			MpxError("Invalid TimePixMode. Should be in [0..%d]"%(nmode-1))

	self.__setArrayValue(self.__HIGH, imode)

    def getTimePixMode(self):
	""" Get current timepix mode and check mode consistency
	"""
	harr= self.__getHighArray()
	imode= harr[0,0]
	if numpy.sum(harr!=imode)>0:
		MpxError("TimePixMode array has different values")
	return imode

    def setLow2Max(self):
	""" Set low threshold array (lowArray) to its max value """
	self.__setArrayValue(self.__LOW, self.arrayDefs[self.version]["Mask"][self.__LOW])
    def setLow2Min(self):
	""" Set low threshold array (lowArray) to its min value """
	self.__setArrayValue(self.__LOW, 0)
    def setHigh2Max(self):
	""" Set high threshold array (lowArray) to its max value """
	self.__setArrayValue(self.__HIGH, self.arrayDefs[self.version]["Mask"][self.__HIGH])
    def setHigh2Min(self):
	""" Set high threshold array (lowArray) to its min value """
	self.__setArrayValue(self.__HIGH, 0)
    def __setArrayValue(self, index, value):
	self.__arrays[index][:,:]= value

    def __getMaskArray(self):
	""" Get mask bit array """
	return self.__getArray(self.__MASK)
    def __getTestArray(self):
	""" Get test bit array """
	return self.__getArray(self.__TEST)
    def __getLowArray(self):
	""" Get Low Threshold Array """
	return self.__getArray(self.__LOW)
    def __getHighArray(self):
	""" Get High Threshold Array """
	return self.__getArray(self.__HIGH)
    def __getArray(self, index):
	return self.__arrays[index]

    def __setMaskArray(self, data):
	""" Set mask bit array """
	self.__setArray(self.__MASK, data)
    def __setTestArray(self, data):
	""" Set test bit array """
	self.__setArray(self.__TEST, data)
    def __setLowArray(self, data):
	""" Set Low Threshold Array """
	self.__setArray(self.__LOW, data)
    def __setHighArray(self, data):
	""" Set High Threshold Array """
	self.__setArray(self.__HIGH, data)
    def __setArray(self, index, data):	
	if type(data)!=numpy.ndarray:
	    raise MpxError("%s should be a numpy Array"%self.arrayLabels[index])
	if data.shape != self.SIZE:
	    raise MpxError("%s is not (256,256)"%self.arrayLabels[index])
	if data.dtype != numpy.uint8:
	    raise MpxError("%s is not an uint8 array"%self.arrayLabels[index])
	self.__arrays[index]= self.__arrays[index] | \
	    numpy.array(data & self.arrayMask[index])

    def __resetMaskArray(self):
	""" Reset mask bit array """
	self.__resetArray(self.__MASK)
    def __resetTestArray(self):
	""" Reset test bit array """
	self.__resetArray(self.__TEST)
    def __resetLowArray(self):
	""" Reset Low Threshold Array """
	self.__resetArray(self.__LOW)
    def __resetHighArray(self):
	""" Reset High Threshold Array """
	self.__resetArray(self.__HIGH)
    def __resetArray(self, index):
	self.__arrays[index]= numpy.zeros(self.SIZE, numpy.uint8)

    testArray= property(__getTestArray, __setTestArray, __resetTestArray, "Test Bit Array")
    maskArray= property(__getMaskArray, __setMaskArray, __resetMaskArray, "Mask Bit Array")
    lowArray= property(__getLowArray, __setLowArray, __resetLowArray, "Low Threshold Array")
    highArray= property(__getHighArray, __setHighArray, __resetHighArray, "High Threshold Array")

