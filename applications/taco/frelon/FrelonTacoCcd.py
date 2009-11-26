from TacoCcd import *

class FrelonAcq:

    deb_params = DebParams(DebModApplication, "FrelonAcq")
    
    def __init__(self, espia_dev_nb):
        deb_obj = DebObj(FrelonAcq.deb_params, "__init__")
        
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


    def __del__(self):
        deb_obj = DebObj(FrelonAcq.deb_params, "__del__")

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

    deb_params = DebParams(DebModApplication, "FrelonTacoAcq")
    
    def __init__(self, dev_name, dev_class=None, cmd_list=None):
        deb_obj = DebObj(FrelonTacoAcq.deb_params, "__init__")
        TacoCcdAcq.__init__(self, dev_name, dev_class, cmd_list)
        
        espia_dev_nb = 0
        self.m_acq = FrelonAcq(espia_dev_nb)

    def __del__(self):
        deb_obj = DebObj(FrelonTacoAcq.deb_params, "__del__")

    def reset(self):
        #self.debugReset('Reseting the device!')
        pass
        
    def getState(self):
        #self.debugState('Query device state ...')
        self.state = DevCcdReady
        #self.debugState('Device state: 0x%08x (%d)' % (state, state))
        return self.state

    def getStatus(self):
        state_desc = { DevCcdReady:     'CCD is Ready',
                       DevCcdAcquiring: 'CCD is Acquiring' }
        state = self.getState()
        status = state_desc[state]
        #self.debugState('Device status: %s (0x%08x)' % (status, state))
        return status

    def getFrameDim(self, max_dim=False):
        deb_obj = DebObj(FrelonTacoAcq.deb_params, "getFrameDim")
       
        ct_image = self.m_acq.getImageControl()
        if max_dim:
            max_size = ct_image.getMaxImageSize()
            fdim = FrameDim(max_size, ct_image.getImageType())
        else:
            fdim = ct_image.getImageDim()
        deb_obj.Trace()#self.debugCmd('Frame dim: %s' % fdim)
        return fdim
    
    def getType(self):
        type_nb = 0
        #self.debugCmd('Getting type: %s (#%s)' % (ccd_type, type_nb))
        return type_nb

    def getLstErrMsg(self):
        err_msg = ''
        #self.debugCmd('Getting last err. msg: %s' % err_msg)
        return err_msg
    
    def setTrigger(self, ext_trig):
        #self.debugCmd('Setting trigger: %s' % ext_trig)
        pass
    
    def getTrigger(self):
        ext_trig = 0
        #self.debugCmd('Getting trigger: %s' % ext_trig)
        return ext_trig
    
    def setNbFrames(self, nb_frames):
        #self.debugCmd('Setting nb. frames: %s' % nb_frames)
        pass
    
    def getNbFrames(self):
        nb_frames = 1
        #self.debugCmd('Getting nb. frames: %s' % nb_frames)
        return nb_frames
    
    def setExpTime(self, exp_time):
        #self.debugCmd('Setting exp. time: %s' % exp_time)
        pass
    
    def getExpTime(self):
        exp_time = 1
        #self.debugCmd('Getting exp. time: %s' % exp_time)
        return exp_time

    def setBin(self, bin):
        # SPEC format Y,X -> incompat. with getBin ...
        bin = Bin(bin[1], bin[0])
        #self.debugCmd('Setting binning: %s' % bin)

    def getBin(self):
        bin = Bin(1, 1)
        #self.debugCmd('Getting binning: %s' % bin)
        return [bin.getX(), bin.getY()]

    def setRoi(self, roi):
        roi = Roi(Point(roi[0], roi[1]), Point(roi[2], roi[3]))
        #self.debugCmd('Setting roi: %s' % roi)

    def getRoi(self):
        roi = Roi()
        #self.debugCmd('Getting roi: %s' % roi)
        tl = roi.getTopLeft()
        br = roi.getBottomRight()
        return [tl.getX(), tl.getY(), br.getX(), br.getY()]
            
    def setFilePar(self, file_par_arr):
        file_par = CcdFilePar(from_arr=file_par_arr)
        config = self.getConfig()
        config.setParam('FilePar', file_par)
        config.apply()

    def getFilePar(self):
        config = self.getConfig()
        file_par = config.getParam('FilePar')
        return file_par.strArray()

    def setChannel(self, input_chan):
        #self.debugCmd('Setting input channel: %s' % input_chan)
        pass
    
    def getChannel(self):
        input_chan = 0
        #self.debugCmd('Getting input channel: %s' % input_chan)
        return input_chan
        
    def setMode(self, mode):
        #self.debugCmd('Setting mode: %s (0x%x)' % (mode, mode))
        auto_save = (mode & self.AutoSave) != 0
        
    def getMode(self):
        auto_save = False
        mode = (auto_save and self.AutoSave) or 0
        #self.debugCmd('Getting mode: %s (0x%x)' % (mode, mode))
        return mode

    def setHwPar(self, hw_par_str):
        hw_par = map(int, string.split(hw_par_str))
        #self.debugCmd('Setting hw par: %s' % hw_par)
        
    def getHwPar(self):
        hw_par = []
        #self.debugCmd('Getting hw par: %s' % hw_par)
        hw_par_str = string.join(map(str, hw_par))
        return hw_par_str
        
    def setKinetics(self, kinetics):
        #self.debugCmd('Setting the profile: %s' % kinetics)
        pass
    
    def getKinetics(self):
        kinetics = 0
        #self.debugCmd('Getting the profile: %s' % kinetics)
        return kinetics
    
    def startAcq(self):
        #self.debugCmd('Starting the device')
        pass
    
    def stopAcq(self):
        #self.debugCmd('Stopping the device')
        pass
    
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

    def startLive(self):
        pass
    
    def getCurrent(self):
        last_frame_nb = 0
        return last_frame_nb

    def execCommand(self, cmd):
        #self.debugCmd('Sending cmd: %s' % cmd)
        resp = ''
        #self.debugCmd('Received response:')
        #for line in resp.split('\r\n'):
            #self.debugCmd(line)
        return resp

    def getChanges(self):
        changes = 0
        #self.debugCmd('Getting changes: %s' % changes)
        return changes

    
class FrelonServer(CcdServer):

    deb_params = DebParams(DebModApplication, "FrelonServer")
    
    def __init__(self, bin_name, pers_name):
        deb_obj = DebObj(FrelonServer.deb_params, "__init__")
        
        CcdServer.__init__(self, bin_name, pers_name)

        dev_name_list = self.getDevNameList()

        for dev_name in dev_name_list:
            dev = FrelonTacoAcq(dev_name)
            self.addDev(dev)

        self.startup()
        
