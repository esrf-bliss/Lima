from __future__ import with_statement

import weakref
import threading
import os

import lima
import EdfFile

def _invert_sort_file(a,b) :
    b1,ext = os.path.splitext(a)
    b2,ext = os.path.splitext(b)
    n1 = int(b1.split('_')[-1])
    n2 = int(b2.split('_')[-1])
    return n2 - n1

class _ImageReader(threading.Thread) :
    def __init__(self,buffer_ctrl) :
        _ImageReader.__init__(self)

        self.__cond = threading.Condition()
        self.__continue = True
        self.__buffer_ctrl = weakref.ref(buffer_ctrl)
        com = buffer_ctrl._com()
        self.__dirFd = os.open(com.DEFAULT_PATH,os.O_DIRECTORY)

        self.__basePath = com.DEFAULT_PATH
        self.__fileBase = com.DEFAULT_FILE_BASE
        self.__fileExt = com.DEFAULT_FILE_EXTENTION

        self.__waitFlag = True
        self.__numberOfNewFile = 0
        self.__lastImageRead = -1
        
    def reset(self) :
        with self.__cond:
            self.__waitFlag = True
            self.__numberOfNewFile = 0
            self.__lastImageRead = -1
            
            com = buffer_ctrl._com()
            #Remove all images in the tmp buffer
            for filename in os.listdir(com.DEFAULT_PATH) :
                base,ext = os.path.splitext(filename)
                if ext.tolower() == com.DEFAULT_FILE_EXTENTION :
                    os.unlink(os.path.join(com.DEFAULT_PATH,filename))
                    
    def start_read(self) :
        with self.__cond:
            self.__waitFlag = False
            self.__cond.notify()
            
    def run(self) :
        lastDirectoryTime = None
        with self.__cond:
            while(self.__continue) :
                newDirectoryTime = os.fstat(self.__dirFd).st_mtime
                while(not self.__continue and not self.__waitFlag and
                      lastDirectoryTime == newDirectoryTime):
                    self.__cond.wait(0.5)
                    newDirectoryTime = os.fstat(self.__dirFd).st_mtime

                while(self.__continue and not self.__waitFlag) :
                    nextFrameId = self.__lastImageRead + 1
                    self.__cond.release()
                    nextFullPath = os.path.join(self.__basePath,
                                                '%s%.5d%s' % (self.__basePath,self.__fileBase,
                                                              self.__fileExt))
                    if os.access(nextFullPath,os.R_OK) :
                        try:
                            f = EdfFile.EdfFile(nextFullPath)
                            data = f.GetData(0)
                        except:
                            self.__cond.acquire()
                            break
                        else:
                            buffer_ctrl = self.__buffer_ctrl()

                            hw_frame_info = lima.HwFrameInfoType()
                            hw_frame_info.frame_data = data
                            hw_frame_info.buffer_owner_ship = Transfer
                            hw_frame_info.acq_frame_nb = nextFrameId
                            if buffer_ctrl._cbk:
                                buffer_ctrl._cbk.newFrameReady(hw_frame_info)
                            self.__cond.acquire()
                            self.__lastImageRead = nextFrameId
                    else:               # We didn't managed to access the file
                        lastDirectoryTime = newDirectoryTime
                        #Get all images from directory
                        files = [x for x in os.listdir(self.__basePath) if os.path.splitext(x)[-1] == '.edf']
                        if files:
                            files.sort(_invert_sort_file)
                            lastImageName,_ = os.path.splitext(files[0])
                            lastImageId = int(lastImageName.split('_')[-1])
                            #We're probably losing some frames
                            if lastImageId - nextFrameId > 10 :
                                pass    # trigg an error
                        self.__cond.acquire()
                        break
                        

                
                
            
class BufferCtrlObj(lima.HwBufferCtrlObj):
	DEB_CLASS(DebModCamera,"BufferCtrlObj","Pilatus")

        @DEB_MEMBER_FUNCT
        def __init__(self,comm_object,det_info) :
            lima.HwBufferCtrlObj.__init__(self)
            self._com = weakref.ref(comm_object)
            self.__det_info = weakref.ref(det_info)
            self._cbk = None
            self.__nb_buffer = 1
            self.__imageReader = _ImageReader(self)
            self.__imageReader.start()
            
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
           self.__nb_buffer = nb_buffers
            
        @DEB_MEMBER_FUNCT
	def getNbBuffers(self) :
            return self.__nb_buffer

        @DEB_MEMBER_FUNCT
        def setNbConcatFrames(self,nb_concat_frames) :
            if nb_concat_frames != 1:
                raise lima.Exceptions(lima.Hardware,lima.NotSupported)

        @DEB_MEMBER_FUNCT
        def getNbConcatFrames(self) :
            return 1

        @DEB_MEMBER_FUNCT
        def setNbAccFrames(self,nb_acc_frames) :
            com = self._com()
            com.set_nb_exposure_per_frame(nb_acc_frames)
            
        @DEB_MEMBER_FUNCT
	def getNbAccFrames(self) :
            com = self._com()
            return com.nb_exposure_per_frame()
        
        @DEB_MEMBER_FUNCT
        def getMaxNbBuffers(self) :
            com = self._com()
            det_info = self.__det_info()
            imageFormat = det_info.getMaxImageSize()
            imageSize = imageFormat.getWidth() * imageFormat.getHeight() * 4 # 4 == image 32bits
            return com.DEFAULT_TMPFS_SIZE / imageSize

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
            self._cbk = frame_cb
            
        @DEB_MEMBER_FUNCT
	def unregisterFrameCallback(self,frame_cb) :
            self._cbk = None


        @DEB_MEMBER_FUNCT
        def getLastAcquiredFrame(self) :
            pass

        def reset(self) :
            self.__imageReader.reset()
        
