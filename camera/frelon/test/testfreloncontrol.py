import os, sys, string
import lima

class ImageStatusCallback(lima.CtControl.ImageStatusCallback):

    def __init__(self, ct, acq_state):
        lima.CtControl.ImageStatusCallback(self)
        self.m_ct = ct
        self.m_acq_state = acq_state
        self.m_nb_frames = 0

    def imageStatusChanged(self, img_status):
        last_acq_frame_nb = img_status.LastImageAcquired;
        last_saved_frame_nb = img_status.LastImageSaved;

        if last_acq_frame_nb == 0:
            ct_acq = m_ct.acquisition()
            self.m_nb_frames = ct_acq.getAcqNbFrames()

        if (last_acq_frame_nb == m_nb_frames - 1) and \
           (self.m_acq_state.get() == lima.AcqState.Acquiring):
            print "All frames acquired!"
            m_acq_state.set(lima.AcqState.Saving)

        if last_saved_frame_nb == nb_frames - 1:
            print "All frames saved!"
            m_acq_state.set(lima.AcqState.Finished)
            
            
class FrelonAcq:

    def __init__(self, espia_dev_nb):
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

        
def test_frelon_control(enable_debug):

    acq = FrelonAcq(0)
    


def main(argv):

    enable_debug = False
    if len(argv) > 1:
        enable_debug = (argv[1] == 'debug')

    test_frelon_control(enable_debug)
        
        
    

if __name__ == '__main__':
    main(sys.argv)
