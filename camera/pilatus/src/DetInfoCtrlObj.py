import weakref
import os

from lima import HwDetInfoCtrlObj
from lima import Debug

CAMERA_NAME_TOKEN = 'camera_name'
CAMERA_WIDE_TOKEN = 'camera_wide'
CAMERA_HIGH_TOKEN = 'camera_high'
CAMERA_BPP_TOKEN = 'camera_bpp'

CAMERA_INFO_FILE = os.path.expanduser(os.path.join('~det','p2_det/config/cam_data/camera.def')

##@brief to find information for the detector, we looking for
#file named CAMERA_INFO_FILE in HOME directory of the users
#@todo check if we can do best
class DetInfoCtrlObj(HwDetInfoCtrlObj) :

    DEB_CLASS(DebModCamera, "DetInfoCtrlObj","Pilatus")
    def __init__(self,interface) :
        self.__cnt = weakref.ref(interface)
        self.__readConfig()

        #Variables
        self.__name = None
        self.__width = None
        self.__height = None
        self.__bpp = None
        
    @DEB_MEMBER_FUNCT
    def getMaxImageSize(self) :
        return lima.Size(self.__width,self.__height)

    @DEB_MEMBER_FUNCT
    def getDetectorImageSize(self) :
        return self.getMaxImageSize()
    
    @DEB_MEMBER_FUNCT
    def getDefImageType(self) :
        if self.__bpp == 32:
            return lima.Bpp32
        else:                           # TODO
            raise lima.Exceptions(lima.Hardware,lima.NotSupported)

    @DEB_MEMBER_FUNCT
    def getCurrImageType(self) :
        return self.getDefImageType()

    @DEB_MEMBER_FUNCT
    def setCurrImageType(self) :
        raise lima.Exceptions(lima.Hardware,lima.NotSupported)

    
    @DEB_MEMBER_FUNCT
    def getPixelSize(self) :
        return 172e-6

    @DEB_MEMBER_FUNCT
    def getDetectorType(self) :
        return 'Pilatus'

    @DEB_MEMBER_FUNCT
    def getDetectorModel(self):
        return self.__name.split(',')[0].split()[-1]


    ##@brief image size won't change so no callback
    @DEB_MEMBER_FUNCT
    def registerMaxImageSizeCallback(cb) :
        pass

    ##@brief image size won't change so no callback
    @DEB_MEMBER_FUNCT
    def unregisterMaxImageSizeCallback(cb) :
        pass

    @DEB_MEMBER_FUNCT
    def get_min_exposition_time(self):
        return 1e-6
    ##@todo don't know realy what is the maximum exposure time
    #for now set to a high value 1 hour
    @DEB_MEMBER_FUNCT
    def get_max_exposition_time(self) :
        return 3600

    @DEB_MEMBER_FUNCT
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
    @DEB_MEMBER_FUNCT
    def get_max_latency(self):
        return self.get_min_latency()
    
    @DEB_MEMBER_FUNCT
    def __readConfig(self) :
        f = file(CAMERA_INFO_FILE)
        for line in f:
            if line.startswith(CAMERA_NAME_TOKEN) :
                self.__name = line.split('=').stip(' \t\"')
            elif line.startswith(CAMERA_WIDE_TOKEN) :
                self.__width = int(line.split('=').stip(' \t'))
            elif line.startswith(CAMERA_HIGH_TOKEN) :
                self.__height = int(line.split('=').stip(' \t'))
            elif line.startswith(CAMERA_NAME_TOKEN) :
                self.__bpp = int(line.split('=').stip(' \t'))
        f.close()
