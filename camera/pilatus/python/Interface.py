from Lima import Core

from DetInfoCtrlObj import DetInfoCtrlObj
from SyncCtrlObj import SyncCtrlObj
from BufferCtrlObj import BufferCtrlObj
from Communication import Communication

DEFAULT_SERVER_PORT = 41234

class Interface(Core.HwInterface) :
    #Core.Debug.DEB_CLASS(Core.DebModCamera, "Interface")

    def __init__(self,port = DEFAULT_SERVER_PORT) :
	Core.HwInterface.__init__(self)
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
        
    #@Core.Debug.DEB_MEMBER_FUNCT
    def getCapList(self) :
        return [Core.HwCap(x) for x in [self.__detInfo,self.__syncObj,self.__buffer]]

    #@Core.Debug.DEB_MEMBER_FUNCT
    def reset(self,reset_level):
        if reset_level == self.HardReset:
            self.__comm.hard_reset()

        self.__comm.soft_reset()
    
    #@Core.Debug.DEB_MEMBER_FUNCT
    def prepareAcq(self):
        camserverStatus = self.__comm.status()
        if camserverStatus == self.__comm.DISCONNECTED:
            self.__comm.connect('localhost',self.__port)

        self.__buffer.reset()
        self.__syncObj.prepareAcq()
        
    #@Core.Debug.DEB_MEMBER_FUNCT
    def startAcq(self) :
        self.__comm.start_acquisition()
        self.__buffer.start()
        
    #@Core.Debug.DEB_MEMBER_FUNCT
    def stopAcq(self) :
        self.__comm.stop_acquisition()
        self.__buffer.stop()
        
    #@Core.Debug.DEB_MEMBER_FUNCT
    def getStatus(self) :
        camserverStatus = self.__comm.status()
        status = Core.HwInterface.StatusType()

        if self.__buffer.is_error() :
            status.det = Core.DetFault
        elif camserverStatus == self.__comm.ERROR:
            status.det = Core.DetFault
            status.acq = Core.AcqFault
        else:
            if camserverStatus != self.__comm.OK:
                status.det = Core.DetExposure
                status.acq = Core.AcqRunning
            else:
                status.det = Core.DetIdle
                lastAcquiredFrame = self.__buffer.getLastAcquiredFrame()
                requestNbFrame = self.__syncObj.getNbFrames()
                if lastAcquiredFrame >= 0 and lastAcquiredFrame == (requestNbFrame - 1):
                    status.acq = Core.AcqReady
                else:
                    status.acq = Core.AcqRunning
            
        status.det_mask = (Core.DetExposure|Core.DetFault)
        return status
    
    #@Core.Debug.DEB_MEMBER_FUNCT
    def getNbAcquiredFrames(self) :
        return self.__buffer.getLastAcquiredFrame() + 1
    
    #@Core.Debug.DEB_MEMBER_FUNCT
    def getNbHwAcquiredFrames(self):
        return self.getNbAcquiredFrames()

    #get lower communication
    def communication(self) :
        return self.__comm
