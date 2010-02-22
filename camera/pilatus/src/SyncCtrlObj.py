import weakref

import lima

class SyncCtrlObj(lima.HwSyncCtrlObj) :
    #lima.Debug.DEB_CLASS(lima.DebModCamera, "SyncCtrlObj")

    def __init__(self,buffer_obj,comm_object,det_info) :
        lima.HwSyncCtrlObj.__init__(self,buffer_obj)
        self.__comm = weakref.ref(comm_object)
        self.__det_info = weakref.ref(det_info)
        
        #Variables
        self.__exposure = comm_object.exposure()
        self.__latency = det_info.get_min_latency()
        self.__nb_frames = 1
        
    #@lima.Debug.DEB_MEMBER_FUNCT
    def setTrigMode(self,trig_mode):
        com = self.__comm()
        cvt_trigger_mode = None
        if trig_mode == lima.IntTrig:
            com.set_trigger_mode(com.INTERNAL)
        elif trig_mode == lima.ExtTrigSingle:
            com.set_trigger_mode(com.EXTERNAL_START)
        elif trig_mode == lima.ExtTrigMult:
            com.set_trigger_mode(com.EXTERNAL_MULTI_START)
        elif trig_mode == lima.ExtGate :
            com.set_trigger_mode(com.EXTERNAL_GATE)
        else:
            raise lima.Exceptions(lima.Hardware,lima.NotSupported)

    #@lima.Debug.DEB_MEMBER_FUNCT
    def getTrigMode(self) :
        cvt_trigger_mode = None
        com = self.__comm()
        trig_mode = com.trigger_mode()
        if trig_mode == com.INTERNAL :
            cvt_trigger_mode = lima.IntTrig
        elif trig_mode == com.EXTERNAL_START:
            cvt_trigger_mode = lima.ExtTrigSingle
        elif trig_mode == com.EXTERNAL_MULTI_START:
            cvt_trigger_mode = lima.ExtTrigMult
        elif trig_mode == com.EXTERNAL_GATE:
            cvt_trigger_mode = lima.ExtGate
        return cvt_trigger_mode
    
    #@lima.Debug.DEB_MEMBER_FUNCT
    def setExpTime(self,exp_time):
        self.__exposure = exp_time
        com = self.__comm()
        com.set_exposure(exp_time)
        
    #@lima.Debug.DEB_MEMBER_FUNCT
    def getExpTime(self) :
        if self.__exposure is None:
            com = self.__comm()
            self.__exposure = com.exposure()
        return self.__exposure

    #@lima.Debug.DEB_MEMBER_FUNCT
    def setLatTime(self,lat_time):
        self.__latency = lat_time

    #@lima.Debug.DEB_MEMBER_FUNCT
    def getLatTime(self) :
        return self.__latency

    #@lima.Debug.DEB_MEMBER_FUNCT
    def setNbFrames(self,nb_frames) :
        self.__nb_frames = nb_frames

    #@lima.Debug.DEB_MEMBER_FUNCT
    def getNbFrames(self) :
        return self.__nb_frames

    #@lima.Debug.DEB_MEMBER_FUNCT
    def setNbHwFrames(self,nb_frames) :
        self.setNbFrames(nb_frames)

    #@lima.Debug.DEB_MEMBER_FUNCT
    def getNbHwFrames(self) :
        return self.getNbHwFrames()

    #@lima.Debug.DEB_MEMBER_FUNCT
    def getValidRanges(self) :
        det_info = self.__det_info()
        return lima.HwSyncCtrlObj.ValidRangesType(det_info.get_min_exposition_time(),
                                                  det_info.get_max_exposition_time(),
                                                  det_info.get_min_latency(),
                                                  det_info.get_max_latency())

    def prepareAcq(self) :
        com = self.__comm()
        exposure = com.exposure()
        exposure_period = exposure + self.__latency
        com.set_exposure_period(exposure_period)
        com.set_nb_images_in_sequence(self.__nb_frames)
