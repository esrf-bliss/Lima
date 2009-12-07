import os, sys, string, gc, time
from lima import *
from Debug import *
import processlib

DEB_GLOBAL(DebModTest)

class ImageStatusCallback(CtControl.ImageStatusCallback):

    DEB_CLASS(DebModTest, "ImageStatusCallback")

    @DEB_MEMBER_FUNCT
    def __init__(self, ct, acq_state, print_time=1):
        CtControl.ImageStatusCallback.__init__(self)

        self.m_ct = ct
        self.m_acq_state = acq_state
        self.m_nb_frames = 0
        
        self.m_last_print_ts = 0
        self.m_print_time = print_time
        
    @DEB_MEMBER_FUNCT
    def imageStatusChanged(self, img_status):
        last_acq_frame_nb = img_status.LastImageAcquired;
        last_saved_frame_nb = img_status.LastImageSaved;

        if last_acq_frame_nb == 0:
            ct_acq = self.m_ct.acquisition()
            self.m_nb_frames = ct_acq.getAcqNbFrames()

        acq_state_changed = False
        msg = ''
        if ((last_acq_frame_nb == self.m_nb_frames - 1) and 
            (self.m_acq_state.get() == AcqState.Acquiring)):
            msg = 'All frames acquired!'
            self.m_acq_state.set(AcqState.Saving)
            acq_state_changed = True

        if last_saved_frame_nb == self.m_nb_frames - 1:
            msg = 'All frames saved!'
            self.m_acq_state.set(AcqState.Finished)
            acq_state_changed = True
            
        now = time.time()
        if ((now - self.m_last_print_ts >= self.m_print_time) or
            acq_state_changed):
            deb.Always("Last Acquired: %8d, Last Saved: %8d" % 
                       (last_acq_frame_nb, last_saved_frame_nb))
                      
            self.m_last_print_ts = now

        if msg:
            deb.Always(msg)


class FrelonAcq:

    DEB_CLASS(DebModTest, "FrelonAcq")
    
    @DEB_MEMBER_FUNCT
    def __init__(self, espia_dev_nb, use_events=False, print_time=1):
        self.m_edev          = Espia.Dev(espia_dev_nb)
        self.m_acq           = Espia.Acq(self.m_edev)
        self.m_buffer_cb_mgr = Espia.BufferMgr(self.m_acq)
        self.m_eserline      = Espia.SerialLine(self.m_edev)
        self.m_cam           = Frelon.Camera(self.m_eserline)
        self.m_buffer_mgr    = BufferCtrlMgr(self.m_buffer_cb_mgr)
        self.m_hw_inter      = Frelon.Interface(self.m_acq,
                                                     self.m_buffer_mgr,
                                                     self.m_cam)
        self.m_acq_state     = AcqState()
        self.m_ct            = CtControl(self.m_hw_inter)
        self.m_ct_acq        = self.m_ct.acquisition()
        self.m_ct_saving     = self.m_ct.saving()
        self.m_ct_image      = self.m_ct.image()
        self.m_ct_buffer     = self.m_ct.buffer()
        self.m_ct_display    = self.m_ct.display()

        self.m_use_events    = use_events
        self.m_print_time    = print_time
        
        if self.m_use_events:
            cb = ImageStatusCallback(self.m_ct, self.m_acq_state, print_time)
            self.m_img_status_cb = cb
            self.m_ct.registerImageStatusCallback(self.m_img_status_cb)
        else:
            self.m_poll_time = 0.1

        self.m_ct_display.setNames('_ccd_ds_', 'frelon_live')
        self.m_ct_display.setActive(True)
        

    @DEB_MEMBER_FUNCT
    def __del__(self):
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

    @DEB_MEMBER_FUNCT
    def start(self):
        self.m_ct.prepareAcq()
        self.m_acq_state.set(AcqState.Acquiring)
        self.m_ct.startAcq()

    @DEB_MEMBER_FUNCT
    def wait(self):
        if self.m_use_events:
            state_mask = AcqState.Acquiring | AcqState.Saving
            self.m_acq_state.waitNot(state_mask)
        else:
            nb_frames = self.m_ct_acq.getAcqNbFrames()
            last_print_ts = 0
            running_states = [AcqState.Acquiring, AcqState.Saving]
            while self.m_acq_state.get() in running_states:
                img_status = self.m_ct.getImageStatus()
                last_acq_frame_nb = img_status.LastImageAcquired;
                last_saved_frame_nb = img_status.LastImageSaved;

                acq_state_changed = False
                msg = ''
                if ((last_acq_frame_nb == nb_frames - 1) and 
                    (self.m_acq_state.get() == AcqState.Acquiring)):
                    msg = 'All frames acquired!'
                    self.m_acq_state.set(AcqState.Saving)
                    acq_state_changed = True

                if last_saved_frame_nb == nb_frames - 1:
                    msg = 'All frames saved!'
                    self.m_acq_state.set(AcqState.Finished)
                    acq_state_changed = True
            
                now = time.time()
                if ((now - last_print_ts >= self.m_print_time) or
                    acq_state_changed):
                    deb.Always("Last Acquired: %8d, Last Saved: %8d" % 
                               (last_acq_frame_nb, last_saved_frame_nb))
                    last_print_ts = now

                if msg:
                    print msg

                time.sleep(self.m_poll_time)

        pool_thread_mgr = processlib.PoolThreadMgr.get()
        pool_thread_mgr.wait()

    @DEB_MEMBER_FUNCT
    def run(self):
        self.start()
        self.wait()

    @DEB_MEMBER_FUNCT
    def initSaving(self, dir, prefix, suffix, idx, fmt, mode, frames_per_file):
        self.m_ct_saving.setDirectory(dir)
        self.m_ct_saving.setPrefix(prefix)
        self.m_ct_saving.setSuffix(suffix)
        self.m_ct_saving.setNextNumber(idx)
        self.m_ct_saving.setFormat(fmt)
        self.m_ct_saving.setSavingMode(mode)
        self.m_ct_saving.setFramesPerFile(frames_per_file)
        
    @DEB_MEMBER_FUNCT
    def setExpTime(self, exp_time):
        self.m_ct_acq.setAcqExpoTime(exp_time)

    @DEB_MEMBER_FUNCT
    def setNbAcqFrames(self, nb_acq_frames):
        self.m_ct_acq.setAcqNbFrames(nb_acq_frames)

    @DEB_MEMBER_FUNCT
    def setBin(self, bin):
        self.m_ct_image.setBin(bin)

    @DEB_MEMBER_FUNCT
    def setRoi(self, roi):
        self.m_ct_image.setRoi(roi)


@DEB_GLOBAL_FUNCT
def test_frelon_control(enable_debug):

    if not enable_debug:
        DebParams.disableModuleFlags(DebParams.AllFlags)

    deb.Always("Creating FrelonAcq")
    espia_dev_nb = 0
    use_events = False
    acq = FrelonAcq(espia_dev_nb, use_events)
    deb.Always("Done!")
    
    acq.initSaving("data", "img", ".edf", 0, CtSaving.EDF, 
                   CtSaving.AutoFrame, 1);

    deb.Always("First run with default pars")
    acq.run()
    deb.Always("Done!")
    
    exp_time = 1e-6
    acq.setExpTime(exp_time)

    nb_acq_frames = 500
    acq.setNbAcqFrames(nb_acq_frames)

    deb.Always("Run exp_time=%s, nb_acq_frames=%s" % (exp_time, nb_acq_frames))
    acq.run()
    deb.Always("Done!")
    
    bin = Bin(2, 2)
    acq.setBin(bin)

    nb_acq_frames = 5
    acq.setNbAcqFrames(nb_acq_frames)

    deb.Always("Run bin=<%sx%s>, nb_acq_frames=%s" % 
               (bin.getX(), bin.getY(), nb_acq_frames))
    acq.run()
    deb.Always("Done!")
    
    roi = Roi(Point(256, 256), Size(512, 512));
    acq.setRoi(roi);

    roi_tl, roi_size = roi.getTopLeft(), roi.getSize()
    deb.Always("Run roi=<%s,%s>-<%sx%s>" %
               (roi_tl.x, roi_tl.y,
                roi_size.getWidth(), roi_size.getHeight()))
    acq.run()
    deb.Always("Done!")
    
    roi = Roi(Point(267, 267), Size(501, 501));
    acq.setRoi(roi);

    roi_tl, roi_size = roi.getTopLeft(), roi.getSize()
    deb.Always("Run roi=<%s,%s>-<%sx%s>" %
               (roi_tl.x, roi_tl.y,
                roi_size.getWidth(), roi_size.getHeight()))
    acq.run()
    deb.Always("Done!")
    

def main(argv):

    enable_debug = False
    if len(argv) > 1:
        enable_debug = (argv[1] == 'debug')

    test_frelon_control(enable_debug)
        
        
    

if __name__ == '__main__':
    main(sys.argv)
