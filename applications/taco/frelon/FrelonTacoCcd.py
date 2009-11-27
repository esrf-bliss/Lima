from TacoCcd import *
import gc

class FrelonAcq:

    DEB_CLASS(DebModApplication, "FrelonAcq")

    @DEB_MEMBER_FUNCT
    def __init__(self, espia_dev_nb):
        self.m_edev          = Espia.Dev(espia_dev_nb)
        self.m_acq           = Espia.Acq(self.m_edev)
        self.m_buffer_cb_mgr = Espia.BufferMgr(self.m_acq)
        self.m_eserline      = Espia.SerialLine(self.m_edev)
        self.m_cam           = Frelon.Camera(self.m_eserline)
        self.m_buffer_mgr    = BufferCtrlMgr(self.m_buffer_cb_mgr)
        self.m_hw_inter      = Frelon.Interface(self.m_acq, self.m_buffer_mgr,
                                                self.m_cam)

        self.m_ct            = CtControl(self.m_hw_inter)
        self.m_ct_acq        = self.m_ct.acquisition()
        self.m_ct_saving     = self.m_ct.saving()
        self.m_ct_image      = self.m_ct.image()
        self.m_ct_buffer     = self.m_ct.buffer()


    @DEB_MEMBER_FUNCT
    def __del__(self):
        del self.m_ct_buffer, self.m_ct_image, self.m_ct_saving, self.m_ct_acq
        del self.m_ct;			gc.collect()

        del self.m_hw_inter;		gc.collect()
        del self.m_buffer_mgr;		gc.collect()
        del self.m_cam;			gc.collect()
        del self.m_eserline;		gc.collect()
        del self.m_buffer_cb_mgr;	gc.collect()
        del self.m_acq;			gc.collect()
        del self.m_edev;		gc.collect()

    def getEspiaDev(self):
        return self.m_edev

    def getEspiaAcq(self):
        return self.m_acq

    def getEspiaBufferMgr(self):
        return self.m_buffer_cb_mgr

    def getEspiaSerialLine(self):
        return self.m_eserline

    def getFrelonCamera(self):
        return self.m_cam

    def getBufferMgr(self):
        return self.m_buffer_mgr

    def getFrelonInterface(self):
        return self.m_hw_inter

    def getGlobalControl(self):
        return self.m_ct

    def getAcqControl(self):
        return self.m_ct_acq
    
    def getSavingControl(self):
        return self.m_ct_saving

    def getImageControl(self):
        return self.m_ct_image

    def getBufferControl(self):
        return self.m_ct_buffer


class FrelonTacoAcq(TacoCcdAcq):

    DEB_CLASS(DebModApplication, "FrelonTacoAcq")

    @DEB_MEMBER_FUNCT
    def __init__(self, dev_name, dev_class=None, cmd_list=None):
        TacoCcdAcq.__init__(self, dev_name, dev_class, cmd_list)
        
        espia_dev_nb = 0
        self.m_acq = FrelonAcq(espia_dev_nb)

    @DEB_MEMBER_FUNCT
    def __del__(self):
        pass

    @DEB_MEMBER_FUNCT
    def reset(self):
        deb.Trace("Reseting the device!")
        ct = self.m_acq.getGlobalControl()
        ct.reset()
        
    @DEB_MEMBER_FUNCT
    def getState(self):
        deb.Trace('Query device state ...')
        self.state = DevCcdReady
        deb.Return('Device state: 0x%08x (%d)' % (state, state))
        return self.state

    @DEB_MEMBER_FUNCT
    def getStatus(self):
        state_desc = { DevCcdReady:     'CCD is Ready',
                       DevCcdAcquiring: 'CCD is Acquiring' }
        state = self.getState()
        status = state_desc[state]
        deb.Return('Device status: %s (0x%08x)' % (status, state))
        return status

    @DEB_MEMBER_FUNCT
    def getFrameDim(self, max_dim=False):
        ct_image = self.m_acq.getImageControl()
        if max_dim:
            max_size = ct_image.getMaxImageSize()
            fdim = FrameDim(max_size, ct_image.getImageType())
        else:
            fdim = ct_image.getImageDim()
        deb.Return('Frame dim: %s' % fdim)
        return fdim
    
    @DEB_MEMBER_FUNCT
    def getType(self):
        type_nb = 0
        deb.Return('Getting type: %s (#%s)' % (ccd_type, type_nb))
        return type_nb

    @DEB_MEMBER_FUNCT
    def getLstErrMsg(self):
        err_msg = ''
        deb.Return('Getting last err. msg: %s' % err_msg)
        return err_msg
    
    @DEB_MEMBER_FUNCT
    def setTrigger(self, ext_trig):
        deb.Param('Setting trigger: %s' % ext_trig)
    
    @DEB_MEMBER_FUNCT
    def getTrigger(self):
        ext_trig = 0
        deb.Return('Getting trigger: %s' % ext_trig)
        return ext_trig
    
    @DEB_MEMBER_FUNCT
    def setNbFrames(self, nb_frames):
        deb.Param('Setting nb. frames: %s' % nb_frames)
    
    @DEB_MEMBER_FUNCT
    def getNbFrames(self):
        nb_frames = 1
        deb.Return('Getting nb. frames: %s' % nb_frames)
        return nb_frames
    
    @DEB_MEMBER_FUNCT
    def setExpTime(self, exp_time):
        deb.Param('Setting exp. time: %s' % exp_time)
        pass
    
    @DEB_MEMBER_FUNCT
    def getExpTime(self):
        exp_time = 1
        deb.Return('Getting exp. time: %s' % exp_time)
        return exp_time

    @DEB_MEMBER_FUNCT
    def setBin(self, bin):
        # SPEC format Y,X -> incompat. with getBin ...
        bin = Bin(bin[1], bin[0])
        deb.Param('Setting binning: %s' % bin)

    @DEB_MEMBER_FUNCT
    def getBin(self):
        bin = Bin(1, 1)
        deb.Return('Getting binning: %s' % bin)
        return [bin.getX(), bin.getY()]

    @DEB_MEMBER_FUNCT
    def setRoi(self, roi):
        roi = Roi(Point(roi[0], roi[1]), Point(roi[2], roi[3]))
        deb.Param('Setting roi: %s' % roi)

    @DEB_MEMBER_FUNCT
    def getRoi(self):
        roi = Roi()
        deb.Return('Getting roi: %s' % roi)
        tl = roi.getTopLeft()
        br = roi.getBottomRight()
        return [tl.getX(), tl.getY(), br.getX(), br.getY()]
            
    @DEB_MEMBER_FUNCT
    def setFilePar(self, file_par_arr):
        file_par = CcdFilePar(from_arr=file_par_arr)
        config = self.getConfig()
        config.setParam('FilePar', file_par)
        config.apply()

    @DEB_MEMBER_FUNCT
    def getFilePar(self):
        config = self.getConfig()
        file_par = config.getParam('FilePar')
        return file_par.strArray()

    @DEB_MEMBER_FUNCT
    def setChannel(self, input_chan):
        deb.Param('Setting input channel: %s' % input_chan)
        pass
    
    @DEB_MEMBER_FUNCT
    def getChannel(self):
        input_chan = 0
        deb.Return('Getting input channel: %s' % input_chan)
        return input_chan
        
    @DEB_MEMBER_FUNCT
    def setMode(self, mode):
        deb.Param('Setting mode: %s (0x%x)' % (mode, mode))
        auto_save = (mode & self.AutoSave) != 0
        
    @DEB_MEMBER_FUNCT
    def getMode(self):
        auto_save = False
        mode = (auto_save and self.AutoSave) or 0
        deb.Return('Getting mode: %s (0x%x)' % (mode, mode))
        return mode

    @DEB_MEMBER_FUNCT
    def setHwPar(self, hw_par_str):
        hw_par = map(int, string.split(hw_par_str))
        deb.Param('Setting hw par: %s' % hw_par)
        
    @DEB_MEMBER_FUNCT
    def getHwPar(self):
        hw_par = []
        deb.Return('Getting hw par: %s' % hw_par)
        hw_par_str = string.join(map(str, hw_par))
        return hw_par_str
        
    @DEB_MEMBER_FUNCT
    def setKinetics(self, kinetics):
        deb.Param('Setting the profile: %s' % kinetics)
        pass
    
    @DEB_MEMBER_FUNCT
    def getKinetics(self):
        kinetics = 0
        deb.Return('Getting the profile: %s' % kinetics)
        return kinetics
    
    @DEB_MEMBER_FUNCT
    def startAcq(self):
        deb.Trace('Starting the device')
        pass
    
    @DEB_MEMBER_FUNCT
    def stopAcq(self):
        deb.Trace('Stopping the device')
        pass
    
    @DEB_MEMBER_FUNCT
    def readFrame(self, frame_data):
        frame_nb, frame_size = frame_data
        frame_dim = self.getFrameDim()
        if frame_size != frame_dim.getMemSize():
            raise ValueError, ('Client expects %d bytes, frame has %d' % \
                               (frame_size, frame_dim.getMemSize()))
        data = N.zeros((frame_dim.getHeight(), frame_dim.getWidth()), N.int16)
        s = data.tostring()
        if len(s) != frame_size:
            raise ValueError, ('Client expects %d bytes, data str has %d' % \
                               (frame_size, len(s)))
        return s

    @DEB_MEMBER_FUNCT
    def startLive(self):
        pass
    
    @DEB_MEMBER_FUNCT
    def getCurrent(self):
        last_frame_nb = 0
        return last_frame_nb

    @DEB_MEMBER_FUNCT
    def execCommand(self, cmd):
        deb.Param('Sending cmd: %s' % cmd)
        cam = self.m_acq.getFrelonCamera()
        ser_line = cam.getSerialLine()
        ser_line.write(cmd)
        resp = ser_line.readLine()
        deb.Return('Received response:')
        for line in resp.split('\r\n'):
            deb.Return(line)
        return resp

    @DEB_MEMBER_FUNCT
    def getChanges(self):
        changes = 0
        deb.Trace('Getting changes: %s' % changes)
        return changes

    
class FrelonServer(CcdServer):

    DEB_CLASS(DebModApplication, "FrelonServer")
    
    @DEB_MEMBER_FUNCT
    def __init__(self, bin_name, pers_name):
        CcdServer.__init__(self, bin_name, pers_name)

        dev_name_list = self.getDevNameList()

        for dev_name in dev_name_list:
            dev = FrelonTacoAcq(dev_name)
            self.addDev(dev)

        self.startup()
        
