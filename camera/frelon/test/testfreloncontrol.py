import os, sys, string, gc, time
import lima
import processlib

deb_params = lima.DebParams(lima.DebModTest)

class ImageStatusCallback(lima.CtControl.ImageStatusCallback):

    deb_params = lima.DebParams(lima.DebModTest, "ImageStatusCallback")
    
    def __init__(self, ct, acq_state, print_time=1):
        deb_params = lima.DebObj(self.deb_params, "__init__")

        lima.CtControl.ImageStatusCallback.__init__(self)

        self.m_ct = ct
        self.m_acq_state = acq_state
        self.m_nb_frames = 0
        
        self.m_last_print_ts = 0
        self.m_print_time = print_time
        
    def imageStatusChanged(self, img_status):
        deb_params = lima.DebObj(self.deb_params, "imageStatusChanged")
        
        last_acq_frame_nb = img_status.LastImageAcquired;
        last_saved_frame_nb = img_status.LastImageSaved;

        if last_acq_frame_nb == 0:
            ct_acq = self.m_ct.acquisition()
            self.m_nb_frames = ct_acq.getAcqNbFrames()

        acq_state_changed = False
        msg = ''
        if ((last_acq_frame_nb == self.m_nb_frames - 1) and 
            (self.m_acq_state.get() == lima.AcqState.Acquiring)):
            msg = 'All frames acquired!'
            self.m_acq_state.set(lima.AcqState.Saving)
            acq_state_changed = True

        if last_saved_frame_nb == self.m_nb_frames - 1:
            msg = 'All frames saved!'
            self.m_acq_state.set(lima.AcqState.Finished)
            acq_state_changed = True
            
        now = time.time()
        if ((now - self.m_last_print_ts >= self.m_print_time) or
            acq_state_changed):
            deb_msg = ("Last Acquired: %8d, Last Saved: %8d" % 
                       (last_acq_frame_nb, last_saved_frame_nb))
            print >> deb_obj.Always(), deb_msg
                      
            self.m_last_print_ts = now

        if msg:
            print >> deb_obj.Always(), msg


class FrelonAcq:

    deb_params = lima.DebParams(lima.DebModTest, "FrelonAcq")
    
    def __init__(self, espia_dev_nb, use_events=False, print_time=1):
        deb_obj = lima.DebObj(self.deb_params, "__init__")
        
        self.m_edev          = lima.Espia.Dev(espia_dev_nb)
        self.m_acq           = lima.Espia.Acq(self.m_edev)
        self.m_buffer_cb_mgr = lima.Espia.BufferMgr(self.m_acq)
        self.m_eserline      = lima.Espia.SerialLine(self.m_edev)
        self.m_cam           = lima.Frelon.Camera(self.m_eserline)
        self.m_buffer_mgr    = lima.BufferCtrlMgr(self.m_buffer_cb_mgr)
        self.m_hw_inter      = lima.Frelon.Interface(self.m_acq,
                                                     self.m_buffer_mgr,
                                                     self.m_cam)
        self.m_acq_state     = lima.AcqState()
        self.m_ct            = lima.CtControl(self.m_hw_inter)
        self.m_ct_acq        = self.m_ct.acquisition()
        self.m_ct_saving     = self.m_ct.saving()
        self.m_ct_image      = self.m_ct.image()
        self.m_ct_buffer     = self.m_ct.buffer()

        self.m_use_events    = use_events
        self.m_print_time    = print_time
        
        if self.m_use_events:
            cb = ImageStatusCallback(self.m_ct, self.m_acq_state, print_time)
            self.m_img_status_cb = cb
            self.m_ct.registerImageStatusCallback(self.m_img_status_cb)
        else:
            self.m_poll_time = 0.1

    def __del__(self):
        deb_obj = lima.DebObj(self.deb_params, "__del__")

        if self.m_use_events:
            del self.m_img_status_cb;	gc.collect()
            
        del self.m_ct_buffer, self.m_ct_image, self.m_ct_saving, self.m_ct_acq
        del self.m_ct;			gc.collect()
        del self.m_acq_state;		gc.collect()
        del self.m_hw_inter;		gc.collect()
        del self.m_buffer_mgr;		gc.collect()
        del self.m_cam;			gc.collect()
        del self.m_eserline;		gc.collect()
        del self.m_buffer_cb_mgr;	gc.collect()
        del self.m_acq;			gc.collect()
        del self.m_edev;		gc.collect()

    def start(self):
        deb_obj = lima.DebObj(self.deb_params, "start")

        self.m_ct.prepareAcq()
        self.m_acq_state.set(lima.AcqState.Acquiring)
        self.m_ct.startAcq()

    def wait(self):
        deb_obj = lima.DebObj(self.deb_params, "wait")

        if self.m_use_events:
            state_mask = lima.AcqState.Acquiring | lima.AcqState.Saving
            self.m_acq_state.waitNot(state_mask)
        else:
            nb_frames = self.m_ct_acq.getAcqNbFrames()
            last_print_ts = 0
            running_states = [lima.AcqState.Acquiring, lima.AcqState.Saving]
            while self.m_acq_state.get() in running_states:
                img_status = self.m_ct.getImageStatus()
                last_acq_frame_nb = img_status.LastImageAcquired;
                last_saved_frame_nb = img_status.LastImageSaved;

                acq_state_changed = False
                msg = ''
                if ((last_acq_frame_nb == nb_frames - 1) and 
                    (self.m_acq_state.get() == lima.AcqState.Acquiring)):
                    msg = 'All frames acquired!'
                    self.m_acq_state.set(lima.AcqState.Saving)
                    acq_state_changed = True

                if last_saved_frame_nb == nb_frames - 1:
                    msg = 'All frames saved!'
                    self.m_acq_state.set(lima.AcqState.Finished)
                    acq_state_changed = True
            
                now = time.time()
                if ((now - last_print_ts >= self.m_print_time) or
                    acq_state_changed):
                    deb_msg = ("Last Acquired: %8d, Last Saved: %8d" % 
                               (last_acq_frame_nb, last_saved_frame_nb))
                    print >> deb_obj.Always(), deb_msg
                    last_print_ts = now

                if msg:
                    print msg

                time.sleep(self.m_poll_time)

        pool_thread_mgr = processlib.PoolThreadMgr.get()
        pool_thread_mgr.wait()

    def run(self):
        deb_obj = lima.DebObj(self.deb_params, "run")
        
        self.start()
        self.wait()

    def initSaving(self, dir, prefix, suffix, idx, fmt, mode, frames_per_file):
        deb_obj = lima.DebObj(self.deb_params, "initSaving")

        self.m_ct_saving.setDirectory(dir)
        self.m_ct_saving.setPrefix(prefix)
        self.m_ct_saving.setSuffix(suffix)
        self.m_ct_saving.setNextNumber(idx)
        self.m_ct_saving.setFormat(fmt)
        self.m_ct_saving.setSavingMode(mode)
        self.m_ct_saving.setFramesPerFile(frames_per_file)
        
    def setExpTime(self, exp_time):
        deb_obj = lima.DebObj(self.deb_params, "setExpTime")
        self.m_ct_acq.setAcqExpoTime(exp_time)

    def setNbAcqFrames(self, nb_acq_frames):
        deb_obj = lima.DebObj(self.deb_params, "setNbAcqFrames")
        self.m_ct_acq.setAcqNbFrames(nb_acq_frames)

    def setBin(self, bin):
        deb_obj = lima.DebObj(self.deb_params, "setBin")
        self.m_ct_image.setBin(bin)

    def setRoi(self, roi):
        deb_obj = lima.DebObj(self.deb_params, "setRoi")
        self.m_ct_image.setRoi(roi)

        
def test_frelon_control(enable_debug):

    deb_obj = lima.DebObj(deb_params, "test_frelon_control")
    
    if not enable_debug:
        lima.DebParams.disableModuleFlags(lima.DebParams.AllFlags)

    print >> deb_obj.Always(), "Creating FrelonAcq"
    espia_dev_nb = 0
    use_events = False
    acq = FrelonAcq(espia_dev_nb, use_events)
    print >> deb_obj.Always(), "Done!"
    
    acq.initSaving("data", "img", ".edf", 0, lima.CtSaving.EDF, 
                   lima.CtSaving.AutoFrame, 1);

    print >> deb_obj.Always(), "First run with default pars"
    acq.run()
    print >> deb_obj.Always(), "Done!"
    
    exp_time = 1e-6
    acq.setExpTime(exp_time)

    nb_acq_frames = 500
    acq.setNbAcqFrames(nb_acq_frames)

    print >> deb_obj.Always(), ("Run exp_time=%s, nb_acq_frames=%s" % 
                                (exp_time, nb_acq_frames))
    acq.run()
    print >> deb_obj.Always(), "Done!"
    
    bin = lima.Bin(2, 2)
    acq.setBin(bin)

    nb_acq_frames = 5
    acq.setNbAcqFrames(nb_acq_frames)

    print >> deb_obj.Always(), ("Run bin=<%sx%s>, nb_acq_frames=%s" % 
                                (bin.getX(), bin.getY(), nb_acq_frames))
    acq.run()
    print >> deb_obj.Always(), "Done!"
    
    roi = lima.Roi(lima.Point(256, 256), lima.Size(512, 512));
    acq.setRoi(roi);

    roi_tl, roi_size = roi.getTopLeft(), roi.getSize()
    print >> deb_obj.Always(), ("Run roi=<%s,%s>-<%sx%s>" %
                                (roi_tl.x, roi_tl.y,
                                 roi_size.getWidth(), roi_size.getHeight()))
    acq.run()
    print >> deb_obj.Always(), "Done!"
    
    roi = lima.Roi(lima.Point(267, 267), lima.Size(501, 501));
    acq.setRoi(roi);

    roi_tl, roi_size = roi.getTopLeft(), roi.getSize()
    print >> deb_obj.Always(), ("Run roi=<%s,%s>-<%sx%s>" %
                                (roi_tl.x, roi_tl.y,
                                 roi_size.getWidth(), roi_size.getHeight()))
    acq.run()
    print >> deb_obj.Always(), "Done!"
    

def main(argv):

    enable_debug = False
    if len(argv) > 1:
        enable_debug = (argv[1] == 'debug')

    test_frelon_control(enable_debug)
        
        
    

if __name__ == '__main__':
    main(sys.argv)
