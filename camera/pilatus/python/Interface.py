import lima

from DetInfoCtrlObj import DetInfoCtrlObj
from SyncCtrlObj import SyncCtrlObj
from BufferCtrlObj import BufferCtrlObj
from Communication import Communication

DEFAULT_SERVER_PORT = 41234

class Interface(lima.HwInterface) :
    #lima.Debug.DEB_CLASS(lima.DebModCamera, "Interface")

    def __init__(self,port = DEFAULT_SERVER_PORT) :
	lima.HwInterface.__init__(self)
        self.__port = port

        self.__comm = Communication('localhost',self.__port)
        self.__detInfo = DetInfoCtrlObj()
        self.__detInfo.init()
        self.__buffer = BufferCtrlObj(self.__comm,self.__detInfo)
        self.__syncObj = SyncCtrlObj(self.__buffer,self.__comm,self.__detInfo)

    def __del__(self) :
        self.__comm.quit()
        self.__buffer.quit()

    def quit(self) :
        self.__comm.quit()
        self.__buffer.quit()
        
    #@lima.Debug.DEB_MEMBER_FUNCT
    def getCapList(self) :
        return [lima.HwCap(x) for x in [self.__detInfo,self.__syncObj,self.__buffer]]

    #@lima.Debug.DEB_MEMBER_FUNCT
    def reset(self,reset_level):
        if reset_level == self.HardReset:
            self.__comm.hard_reset()

        self.__comm.soft_reset()
    
    #@lima.Debug.DEB_MEMBER_FUNCT
    def prepareAcq(self):
        camserverStatus = self.__comm.status()
        if camserverStatus == self.__comm.DISCONNECTED:
            self.__comm.connect('localhost',self.__port)

        self.__buffer.reset()
        self.__syncObj.prepareAcq()
        
    #@lima.Debug.DEB_MEMBER_FUNCT
    def startAcq(self) :
        self.__comm.start_acquisition()
        self.__buffer.start()
        
    #@lima.Debug.DEB_MEMBER_FUNCT
    def stopAcq(self) :
        self.__comm.stop_acquisition()
        self.__buffer.stop()
        
    #@lima.Debug.DEB_MEMBER_FUNCT
    def getStatus(self) :
        camserverStatus = self.__comm.status()
        status = lima.HwInterface.StatusType()

        if self.__buffer.is_error() :
            status.det = lima.DetFault
        elif camserverStatus == self.__comm.ERROR:
            status.det = lima.DetFault
            status.acq = lima.AcqFault
        else:
            if camserverStatus != self.__comm.OK:
                status.det = lima.DetExposure
                status.acq = lima.AcqRunning
            else:
                status.det = lima.DetIdle
                lastAcquiredFrame = self.__buffer.getLastAcquiredFrame()
                requestNbFrame = self.__syncObj.getNbFrames()
                if lastAcquiredFrame >= 0 and lastAcquiredFrame == (requestNbFrame - 1):
                    status.acq = lima.AcqReady
                else:
                    status.acq = lima.AcqRunning
            
        status.det_mask = (lima.DetExposure|lima.DetFault)
        return status
    
    #@lima.Debug.DEB_MEMBER_FUNCT
    def getNbAcquiredFrames(self) :
        return self.__buffer.getLastAcquiredFrame() + 1
    
    #@lima.Debug.DEB_MEMBER_FUNCT
    def getNbHwAcquiredFrames(self):
        return self.getNbAcquiredFrames()

    #get lower communication
    def communication(self) :
        return self.__comm
