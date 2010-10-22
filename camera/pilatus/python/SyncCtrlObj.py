import weakref

from Lima import Core

class SyncCtrlObj(Core.HwSyncCtrlObj) :
    #Core.Debug.DEB_CLASS(Core.DebModCamera, "SyncCtrlObj")
    def __init__(self,buffer_obj,comm_object,det_info) :
        Core.HwSyncCtrlObj.__init__(self,buffer_obj)
        self.__comm = weakref.ref(comm_object)
        self.__det_info = weakref.ref(det_info)
        
        #Variables
        self.__exposure = comm_object.exposure()
        self.__latency = det_info.get_min_latency()
        self.__nb_frames = 1
        
    #@Core.Debug.DEB_MEMBER_FUNCT
    def setTrigMode(self,trig_mode):
        com = self.__comm()
        cvt_trigger_mode = None
        if trig_mode == Core.IntTrig:
            com.set_trigger_mode(com.INTERNAL)
        elif trig_mode == Core.ExtTrigSingle:
            com.set_trigger_mode(com.EXTERNAL_START)
        elif trig_mode == Core.ExtTrigMult:
            com.set_trigger_mode(com.EXTERNAL_MULTI_START)
        elif trig_mode == Core.ExtGate :
            com.set_trigger_mode(com.EXTERNAL_GATE)
        else:
            raise Core.Exceptions(Core.Hardware,Core.NotSupported)

    #@Core.Debug.DEB_MEMBER_FUNCT
    def getTrigMode(self) :
        cvt_trigger_mode = None
        com = self.__comm()
        trig_mode = com.trigger_mode()
        if trig_mode == com.INTERNAL :
            cvt_trigger_mode = Core.IntTrig
        elif trig_mode == com.EXTERNAL_START:
            cvt_trigger_mode = Core.ExtTrigSingle
        elif trig_mode == com.EXTERNAL_MULTI_START:
            cvt_trigger_mode = Core.ExtTrigMult
        elif trig_mode == com.EXTERNAL_GATE:
            cvt_trigger_mode = Core.ExtGate
        return cvt_trigger_mode
    
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
        com.set_nb_images_in_sequence(self.__nb_frames)
