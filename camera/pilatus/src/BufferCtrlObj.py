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
        threading.Thread.__init__(self)

        self.__cond = threading.Condition()
        self.__continue = True
        self.__buffer_ctrl = weakref.ref(buffer_ctrl)
        com = buffer_ctrl._com()
	try:
            self.__dirFd = os.open(com.DEFAULT_PATH,os.O_DIRECTORY)
	except OSError:
	    self.__dirFd = None

        self.__basePath = com.DEFAULT_PATH
        self.__fileBase = com.DEFAULT_FILE_BASE
        self.__fileExt = com.DEFAULT_FILE_EXTENTION

        self.__waitFlag = True
        self.__numberOfNewFile = 0
        self.__lastImageRead = -1
        self.__readError = False
        
    def is_read_error(self) :
        with self.__cond:
            return self.__readError
        
    def reset(self) :
        with self.__cond:
            self.__waitFlag = True
            self.__numberOfNewFile = 0
            self.__lastImageRead = -1
            self.__readError = False
            
            buffer_ctrl = self.__buffer_ctrl()
            com = buffer_ctrl._com()
            #Remove all images in the tmp buffer
            for filename in os.listdir(com.DEFAULT_PATH) :
                base,ext = os.path.splitext(filename)
                if ext.lower() == com.DEFAULT_FILE_EXTENTION :
                    os.unlink(os.path.join(com.DEFAULT_PATH,filename))

    def start_read(self) :
        with self.__cond:
            self.__waitFlag = False
            self.__cond.notify()

    def stop_read(self) :
        with self.__cond:
            self.__waitFlag = True
            
    def quit(self) :
        with self.__cond:
            self.__waitFlag = False
            self.__continue = False
            self.__cond.notify()
        self.join()

    def getLastAcquiredFrame(self) :
        with self.__cond:
            return self.__lastImageRead
        
    def run(self) :
        lastDirectoryTime = None

	if not self.__dirFd: return #@todo should throw an execption

        with self.__cond:
            while(self.__continue) :
                newDirectoryTime = os.fstat(self.__dirFd).st_mtime
                while(self.__waitFlag or
                      (self.__continue and lastDirectoryTime == newDirectoryTime)):
                    self.__cond.wait(0.5)
                    newDirectoryTime = os.fstat(self.__dirFd).st_mtime
                while(self.__continue and not self.__waitFlag) :
                    nextFrameId = self.__lastImageRead + 1
                    self.__cond.release()
                    nextFullPath = os.path.join(self.__basePath,
                                                '%s%.5d%s' % (self.__fileBase,nextFrameId,
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
                              
                            continueFlag = True
                            if buffer_ctrl._cbk:
                                hw_frame_info = lima.HwFrameInfoType(nextFrameId,data,lima.Timestamp(),
                                                                     0,lima.HwFrameInfoType.Transfer)
                                continueFlag = buffer_ctrl._cbk.newFrameReady(hw_frame_info)
                                
                            del data

                            #remove old image from buffer (tmp_fs)
                            idImage2remove = nextFrameId - buffer_ctrl.getNbBuffers()
                            if idImage2remove >= 0 :
                                fullImagePath2remove = os.path.join(self.__basePath,
                                                '%s%.5d%s' % (self.__fileBase,idImage2remove,
                                                              self.__fileExt))
                                os.unlink(fullImagePath2remove)
                                
                            self.__cond.acquire()
                            self.__lastImageRead = nextFrameId
                            self.__waitFlag = not continueFlag
                    else:               # We didn't managed to access the file
                        ErrorFlag = False
                        lastDirectoryTime = newDirectoryTime
                        #Get all images from directory
                        files = [x for x in os.listdir(self.__basePath) if os.path.splitext(x)[-1] == '.edf']
                        if files:
                            files.sort(_invert_sort_file)
                            lastImageName,_ = os.path.splitext(files[0])
                            lastImageId = int(lastImageName.split('_')[-1])
                            #We're probably losing some frames
                            if lastImageId - nextFrameId > 10 :
                                ErrorFlag = True
                        
                        self.__cond.acquire()
                        if ErrorFlag:
                            self.__waitFlag = True
                            self.__readError = True
                        break
                        

                
                
            
class BufferCtrlObj(lima.HwBufferCtrlObj):
	#lima.Debug.DEB_CLASS(lima.DebModCamera,"BufferCtrlObj")

        def __init__(self,comm_object,det_info) :
            lima.HwBufferCtrlObj.__init__(self)
            self._com = weakref.ref(comm_object)
            self.__det_info = weakref.ref(det_info)
            self._cbk = None
            self.__nb_buffer = 1
            self.__imageReader = _ImageReader(self)
            self.__imageReader.start()

        def __del__(self) :
            self.__imageReader.quit()

        def quit(self) :
            self.__imageReader.quit()

        def start(self) :
            self.__imageReader.start_read()

        def stop(self) :
            self.__imageReader.stop_read()

        def is_error(self) :
            return self.__imageReader.is_read_error()
        
        #@lima.Debug.DEB_MEMBER_FUNCT
	def setFrameDim(self,frame_dim) :
            pass
            
        #@lima.Debug.DEB_MEMBER_FUNCT
        def getFrameDim(self) :
            det_info = self.__det_info()
            return lima.FrameDim(det_info.getDetectorImageSize(),
                                 det_info.getDefImageType())
        
        #@lima.Debug.DEB_MEMBER_FUNCT
        def setNbBuffers(self,nb_buffers) :
           self.__nb_buffer = nb_buffers
            
        #@lima.Debug.DEB_MEMBER_FUNCT
	def getNbBuffers(self) :
            return self.__nb_buffer

        #@lima.Debug.DEB_MEMBER_FUNCT
        def setNbConcatFrames(self,nb_concat_frames) :
            if nb_concat_frames != 1:
                raise lima.Exceptions(lima.Hardware,lima.NotSupported)

        #@lima.Debug.DEB_MEMBER_FUNCT
        def getNbConcatFrames(self) :
            return 1

        #@lima.Debug.DEB_MEMBER_FUNCT
        def setNbAccFrames(self,nb_acc_frames) :
            com = self._com()
            com.set_nb_exposure_per_frame(nb_acc_frames)
            
        #@lima.Debug.DEB_MEMBER_FUNCT
	def getNbAccFrames(self) :
            com = self._com()
            return com.nb_exposure_per_frame()
        
        #@lima.Debug.DEB_MEMBER_FUNCT
        def getMaxNbBuffers(self) :
            com = self._com()
            det_info = self.__det_info()
            imageFormat = det_info.getMaxImageSize()
            imageSize = imageFormat.getWidth() * imageFormat.getHeight() * 4 # 4 == image 32bits
            return com.DEFAULT_TMPFS_SIZE / imageSize / 2.

        #@lima.Debug.DEB_MEMBER_FUNCT
        def getBufferPtr(self,buffer_nb,concat_frame_nb = 0) :
            pass
        
        #@lima.Debug.DEB_MEMBER_FUNCT
        def getFramePtr(self,acq_frame_nb) :
            pass

        #@lima.Debug.DEB_MEMBER_FUNCT
        def getStartTimestamp(self,start_ts) :
            pass
        
        #@lima.Debug.DEB_MEMBER_FUNCT
        def getFrameInfo(self,acq_frame_nb) :
            hw_frame_info = lima.HwFrameInfoType()
            com = self._com()            
            fileBase = com.DEFAULT_FILE_BASE
            fileExt = com.DEFAULT_FILE_EXTENTION
            fullPath = os.path.join(com.DEFAULT_PATH,
                                    '%s%.5d%s' % (fileBase,acq_frame_nb,
                                                  fileExt))
            if os.access(fullPath,os.R_OK) :
                try:
                    f = EdfFile.EdfFile(fullPath)
                    data = f.GetData(0)
                except:
                    pass
                else:
                    return lima.HwFrameInfoType(acq_frame_nb,data,lima.Timestamp(),
                                                0,lima.HwFrameInfoType.Transfer)

            return hw_frame_info

        #@lima.Debug.DEB_MEMBER_FUNCT
        def registerFrameCallback(self,frame_cb) :
            self._cbk = frame_cb
            
        #@lima.Debug.DEB_MEMBER_FUNCT
	def unregisterFrameCallback(self,frame_cb) :
            self._cbk = None


        #@lima.Debug.DEB_MEMBER_FUNCT
        def getLastAcquiredFrame(self) :
            return self.__imageReader.getLastAcquiredFrame()

        def reset(self) :
            self.__imageReader.reset()
        
