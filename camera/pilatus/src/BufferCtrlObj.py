import weakref

import lima

class BufferCtrlObj(lima.HwBufferCtrlObj):
	DEB_CLASS(DebModCamera,"BufferCtrlObj","Pilatus")

        @DEB_MEMBER_FUNCT
        def __init__(self,comm_object,det_info) :
            lima.HwBufferCtrlObj.__init__(self)
            self.__com = weakref.ref(comm_object)
            self.__det_info = weakref.ref(det_info)
            self.__cbk = None
            
        @DEB_MEMBER_FUNCT
	def setFrameDim(self,frame_dim) :
            pass
            
        @DEB_MEMBER_FUNCT
        def getFrameDim(self) :
            det_info = self.__det_info()
            return lima.FrameDim(det_info.getDetectorImageSize(),
                                 det_info.getDefImageType())
        
        @DEB_MEMBER_FUNCT
        def setNbBuffers(self,nb_buffers) :
            if nb_buffers != 1:
                raise lima.Exceptions(lima.Hardware,lima.NotSupported)
            
        @DEB_MEMBER_FUNCT
	def getNbBuffers(self) :
            return 1

        @DEB_MEMBER_FUNCT
        def setNbConcatFrames(self,nb_concat_frames) :
            if nb_concat_frames != 1:
                raise lima.Exceptions(lima.Hardware,lima.NotSupported)

        @DEB_MEMBER_FUNCT
        def getNbConcatFrames(self) :
            return 1

        @DEB_MEMBER_FUNCT
        def setNbAccFrames(self,nb_acc_frames) :
            com = self.__com()
            com.set_nb_exposure_per_frame(nb_acc_frames)
            
        @DEB_MEMBER_FUNCT
	def getNbAccFrames(self) :
            com = self.__com()
            return com.nb_exposure_per_frame()
        
        @DEB_MEMBER_FUNCT
        def getMaxNbBuffers(self) :
            return 1

        @DEB_MEMBER_FUNCT
        def getBufferPtr(self,buffer_nb,concat_frame_nb = 0) :
            pass
        
        @DEB_MEMBER_FUNCT
        def getFramePtr(self,acq_frame_nb) :
            pass

        @DEB_MEMBER_FUNCT
        def getStartTimestamp(self,start_ts) :
            pass
        
        @DEB_MEMBER_FUNCT
        def getFrameInfo(self,acq_frame_nb) :
            pass

        @DEB_MEMBER_FUNCT
        def registerFrameCallback(self,frame_cb) :
            self.__cbk = frame_cb
            
        @DEB_MEMBER_FUNCT
	def unregisterFrameCallback(self,frame_cb) :
            self.__cbk = None


        @DEB_MEMBER_FUNCT
        def getLastAcquiredFrame(self) :
            pass
        
