import weakref

from Lima import Core

class SyncCtrlObj(Core.HwSyncCtrlObj) :
    #Core.Debug.DEB_CLASS(Core.DebModCamera, "SyncCtrlObj")
    def __init__(self,comm_object,det_info) :
        Core.HwSyncCtrlObj.__init__(self)
        self.__comm = weakref.ref(comm_object)
        self.__det_info = weakref.ref(det_info)
        
        #Variables
        self.__exposure = comm_object.exposure()
        self.__latency = det_info.get_min_latency()
        self.__nb_frames = 1
        self.__limaTrig2CamTrig = {Core.IntTrig : comm_object.INTERNAL,
                                   Core.IntTrigMult : comm_object.INTERNAL_TRIG_MULTI,
                                   Core.ExtTrigSingle : comm_object.EXTERNAL_START,
                                   Core.ExtTrigMult : comm_object.EXTERNAL_MULTI_START,
                                   Core.ExtGate : comm_object.EXTERNAL_GATE}
        self.__CamTrig2limaTrig = dict([(y,x) for x,y in self.__limaTrig2CamTrig.iteritems()])

    #@Core.Debug.DEB_MEMBER_FUNCT
    def checkTrigMode(self,trig_mode) :
        tMode = self.__limaTrig2CamTrig.get(trig_mode,None)
        return tMode is not None
        
    #@Core.Debug.DEB_MEMBER_FUNCT
    def setTrigMode(self,trig_mode):
        com = self.__comm()
        if self.checkTrigMode(trig_mode) :
            tMode = self.__limaTrig2CamTrig.get(trig_mode)
            com.set_trigger_mode(tMode)
        else:
            raise Core.Exceptions(Core.Hardware,Core.NotSupported)


    #@Core.Debug.DEB_MEMBER_FUNCT
    def getTrigMode(self) :
        cvt_trigger_mode = None
        com = self.__comm()
        trig_mode = com.trigger_mode()
        return self.__CamTrig2limaTrig.get(trig_mode,None)
    
    #@Core.Debug.DEB_MEMBER_FUNCT
    def setExpTime(self,exp_time):
        self.__exposure = exp_time
        com = self.__comm()
        com.set_exposure(exp_time)
        
    #@Core.Debug.DEB_MEMBER_FUNCT
    def getExpTime(self) :
        if self.__exposure is None:
            com = self.__comm()
            self.__exposure = com.exposure()
        return self.__exposure

    #@Core.Debug.DEB_MEMBER_FUNCT
    def setLatTime(self,lat_time):
        self.__latency = lat_time

    #@Core.Debug.DEB_MEMBER_FUNCT
    def getLatTime(self) :
        return self.__latency

    #@Core.Debug.DEB_MEMBER_FUNCT
    def setNbFrames(self,nb_frames) :
        self.__nb_frames = nb_frames

    #@Core.Debug.DEB_MEMBER_FUNCT
    def getNbFrames(self) :
        return self.__nb_frames

    #@Core.Debug.DEB_MEMBER_FUNCT
    def setNbHwFrames(self,nb_frames) :
        self.setNbFrames(nb_frames)

    #@Core.Debug.DEB_MEMBER_FUNCT
    def getNbHwFrames(self) :
        return self.getNbHwFrames()

    #@Core.Debug.DEB_MEMBER_FUNCT
    def getValidRanges(self) :
        det_info = self.__det_info()
        return Core.HwSyncCtrlObj.ValidRangesType(det_info.get_min_exposition_time(),
                                                  det_info.get_max_exposition_time(),
                                                  det_info.get_min_latency(),
                                                  det_info.get_max_latency())

    def prepareAcq(self) :
        com = self.__comm()
        exposure = self.__exposure
        exposure_period = exposure + self.__latency
        com.set_exposure_period(exposure_period)

        trig_mode = self.getTrigMode()
        nb_frames = trig_mode == Core.IntTrigMult and 1 or self.__nb_frames
        com.set_nb_images_in_sequence(nb_frames)
