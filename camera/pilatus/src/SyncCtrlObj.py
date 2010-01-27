import weakref

import lima

class SyncCtrlObj(lima.HwSyncCtrlObj) :
    DEB_CLASS(DebModCamera, "SyncCtrlObj","Pilatus")

    def __init__(self,comm_object,det_info) :
        lima.HwSyncCtrlObj.__init__(self)
        self.__comm = weakref.ref(comm_object)
        self.__det_info = weakref.ref(det_info)
        
        #Variables
        self.__exposure = self.__comm.exposure()
        self.__latency = self.__comm.exposure_period() - self.__comm.exposure()
        
    @DEB_MEMBER_FUNCT
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

    @DEB_MEMBER_FUNCT
    def getTrigMode(self) :
        cvt_trigger_mode = None
        com = self.__comm()
        trig_mode = com.trig_mode()
        if trig_mode == com.INTERNAL :
            cvt_trigger_mode = lima.IntTrig
        elif trig_mode == com.EXTERNAL_START:
            cvt_trigger_mode = lima.ExtTrigSingle
        elif trig_mode == com.EXTERNAL_MULTI_START:
            cvt_trigger_mode = lima.ExtTrigMult
        elif trig_mode == com.EXTERNAL_GATE:
            cvt_trigger_mode = lima.ExtGate
        return cvt_trigger_mode
    
    @DEB_MEMBER_FUNCT
    def setExpTime(self,exp_time):
        self.__exposure = exp_time
        
    @DEB_MEMBER_FUNCT
    def getExpTime(self) :
        return self.__exposure

    @DEB_MEMBER_FUNCT
    def setLatTime(self,lat_time):
        self.__latency = lat_time

    @DEB_MEMBER_FUNCT
    def getLatTime(self) :
        return self.__latency

    @DEB_MEMBER_FUNCT
    def setNbFrames(self,nb_frames) :
        self.__nb_frames = nb_frames

    @DEB_MEMBER_FUNCT
    def getNbFrames(self) :
        return self.__nb_frames

    @DEB_MEMBER_FUNCT
    def setNbHwFrames(self,nb_frames) :
        self.setNbFrames(nb_frames)

    @DEB_MEMBER_FUNCT
    def getNbHwFrames(self) :
        return self.getNbHwFrames()

    @DEB_MEMBER_FUNCT
    def getValidRanges(self) :
        det_info = self.__det_info()
        valid_ranges = self.ValidRangesType()
        valid_ranges.min_exp_time = det_info.get_min_exposition_time()
        valid_ranges.max_exp_time = det_info.get_max_exposition_time()
        valid_ranges.min_lat_time = det_info.get_min_latency()
        valid_ranges.max_lat_time = det_info.get_max_latency()
        return valid_ranges
