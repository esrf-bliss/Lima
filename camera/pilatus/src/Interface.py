import lima

from DetInfoCtrlObj import DetInfoCtrlObj
from SyncCtrlObj import SyncCtrlObj
from BufferCtrlObj import BufferCtrlObj

DEFAULT_SERVER_PORT = 41234

class Interface(lima.HwInterface) :
    DEB_CLASS(DebModCamera, "Interface","Pilatus")

    def __init__(self,port = DEFAULT_SERVER_PORT) :
	lima.HwInterface.__init__(self)
        self.__port = port

        self.__comm = Communication('localhost',self.__port)
        self.__detInfo = DetInfoCtrlObj(self)
        self.__syncObj = SyncCtrlObj(self.__comm,self.__detInfo)
        self.__buffer = BufferCtrlObj(self.__comm,self.__detInfo)
        
    @DEB_MEMBER_FUNCT
    def getCapList(self) :
        return [self.__detInfo,self.__syncObj,self.__buffer]

    @DEB_MEMBER_FUNCT
    def getHwCtrlObj(cap_type) :
        if cap_type == lima.HwCap.DetInfo:
            return self.__detInfo
        elif cap_type == lima.HwCap.Sync:
            return self.__syncObj
        elif cap_type == lima.HwCap.Buffer:
            return self.__buffer
 
    @DEB_MEMBER_FUNCT
    def reset(self,reset_level):
        if reset_level == self.HardReset:
            self.__comm.hard_reset()

        self.__comm.soft_reset()
    
    @DEB_MEMBER_FUNCT
    def prepareAcq(self):
        camserverStatus = self.__comm.status()
        if camserverStatus == self.__comm.DISCONNECTED:
            self.__comm.connect('localhost',self.__port)

        self.__buffer.reset()
            
    @DEB_MEMBER_FUNCT
    def startAcq(self) :
        self.__comm.start_acquisition()
    
    @DEB_MEMBER_FUNCT
    def stopAcq(self) :
        self.__comm.stop_acquisition()
    
    @DEB_MEMBER_FUNCT
    def getStatus(self) :
        camserverStatus = self.__comm.status()
        lima.AcqReady, lima.AcqRunning, lima.AcqFault
        status = lima.HwInterface.Status()

        if camserverStatus == self.__comm.ERROR:
            status.det = lima.DetFault
            status.acq = lima.AcqFault
        else:
            if camserverStatus == self.__comm.RUNNING:
                status.det = lima.DetExposure
                status.acq = lima.AcqRunning
            else:
                status.det = lima.DetIdle
                status.acq = lima.AcqReady # TODO test if the file save is finnished
            
        status.det_mask = (lima.DetExposure|lima.DetFault)
        
        return status
    
    @DEB_MEMBER_FUNCT
    def getNbAcquiredFrames(self) :
        return self.__buffer.getLastAcquiredFrame()
    
    @DEB_MEMBER_FUNCT
    def getNbHwAcquiredFrames(self):
        pass
