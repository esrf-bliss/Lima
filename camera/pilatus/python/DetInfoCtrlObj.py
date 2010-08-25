import os

from Lima import Core


CAMERA_NAME_TOKEN = 'camera_name'
CAMERA_WIDE_TOKEN = 'camera_wide'
CAMERA_HIGH_TOKEN = 'camera_high'
CAMERA_BPP_TOKEN = 'camera_bpp'

CAMERA_INFO_FILE = os.path.expanduser(os.path.join('~det','p2_det/config/cam_data/camera.def'))

##@brief to find information for the detector, we looking for
#file named CAMERA_INFO_FILE in HOME directory of the users
#@todo check if we can do best

class DetInfoCtrlObj(Core.HwDetInfoCtrlObj) :

    #Core.Debug.DEB_CLASS(Core.DebModCamera, "DetInfoCtrlObj")
    def __init__(self) :
        Core.HwDetInfoCtrlObj.__init__(self)

        #Variables
        self.__name = None
        self.__width = None
        self.__height = None
        self.__bpp = None

    def init(self) :
        self._readConfig()
        
    #@Core.Debug.DEB_MEMBER_FUNCT
    def getMaxImageSize(self) :
        return Core.Size(self.__width,self.__height)

    #@Core.Debug.DEB_MEMBER_FUNCT
    def getDetectorImageSize(self) :
        return self.getMaxImageSize()
    
    #@Core.Debug.DEB_MEMBER_FUNCT
    def getDefImageType(self) :
        if self.__bpp == 32:
            return Core.Bpp32S
        else:                           # TODO
            raise Core.Exception(Core.Hardware,Core.NotSupported)

    #@Core.Debug.DEB_MEMBER_FUNCT
    def getCurrImageType(self) :
        return self.getDefImageType()

    #@Core.Debug.DEB_MEMBER_FUNCT
    def setCurrImageType(self) :
        raise Core.Exceptions(Core.Hardware,Core.NotSupported)

    
    #@Core.Debug.DEB_MEMBER_FUNCT
    def getPixelSize(self) :
        return 172e-6

    #@Core.Debug.DEB_MEMBER_FUNCT
    def getDetectorType(self) :
        return 'Pilatus'

    #@Core.Debug.DEB_MEMBER_FUNCT
    def getDetectorModel(self):
	if self.__name :
           return self.__name.split(',')[0].split()[-1]
	else:
	   return "Pilatus unknown"


    ##@brief image size won't change so no callback
    #@Core.Debug.DEB_MEMBER_FUNCT
    def registerMaxImageSizeCallback(self,cb) :
        pass

    ##@brief image size won't change so no callback
    #@Core.Debug.DEB_MEMBER_FUNCT
    def unregisterMaxImageSizeCallback(self,cb) :
        pass

    #@Core.Debug.DEB_MEMBER_FUNCT
    def get_min_exposition_time(self):
        return 1e-6
    ##@todo don't know realy what is the maximum exposure time
    #for now set to a high value 1 hour
    #@Core.Debug.DEB_MEMBER_FUNCT
    def get_max_exposition_time(self) :
        return 3600

    #@Core.Debug.DEB_MEMBER_FUNCT
    def get_min_latency(self) :
        period = None
        model = self.getDetectorModel()
        if model == '300K' or model == '300KWF' :
            period = 1/200.             # 200Hz
        else:                           # PILATUS UNKNOWN
            period = 1/12.              # frequence of the Pilatus 6M (slowest)

        latency = period - self.get_min_exposition_time()
        return latency

    ##@todo don't know
    #@see get_max_exposition_time
    #@Core.Debug.DEB_MEMBER_FUNCT
    def get_max_latency(self):
        return self.get_min_latency()
    
    #@Core.Debug.DEB_MEMBER_FUNCT
    def _readConfig(self) :
        try:
            f = file(CAMERA_INFO_FILE)
	except IOError:
	    return
        for line in f:
            if line.startswith(CAMERA_NAME_TOKEN) :
                self.__name = line.split('=')[-1].strip(' \t\"')
            elif line.startswith(CAMERA_WIDE_TOKEN) :
                self.__width = int(line.split('=')[-1].strip(' \t'))
            elif line.startswith(CAMERA_HIGH_TOKEN) :
                self.__height = int(line.split('=')[-1].strip(' \t'))
            elif line.startswith(CAMERA_BPP_TOKEN) :
                self.__bpp = int(line.split('=')[-1].strip(' \t'))
        f.close()
