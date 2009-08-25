import lima
import time


print "Creating Espia.Dev"
edev = lima.Espia.Dev(0)

print "Creating Espia.Acq"
acq = lima.Espia.Acq(edev)

acqstat = acq.getStatus()
print "Whether the Acquisition is running : ", acqstat.running

print "Creating Espia.BufferMgr"
buffer_cb_mgr = lima.Espia.BufferMgr(acq)

print "Creating BufferCtrlMgr"
buffer_mgr = lima.BufferCtrlMgr(buffer_cb_mgr)

print "Creating Espia.SerialLine"
eser_line = lima.Espia.SerialLine(edev)

print "Creating Frelon.Camera"
cam = lima.Frelon.Camera(eser_line);

print "Creating the Hw Interface ... "
hw_inter = lima.Frelon.Interface(acq, buffer_mgr, cam)

print "Creating BufferSave"
buffer_save = lima.BufferSave(lima.BufferSave.EDF, "img", 0, ".edf", True, 1)

print "Getting HW detector info"
hw_det_info = hw_inter.getHwCtrlObj(lima.HwCap.DetInfo)
print type(hw_det_info)

print "Getting HW buffer"
hw_buffer = hw_inter.getHwCtrlObj(lima.HwCap.Buffer)
print type(hw_buffer)

print "Getting HW Sync"
hw_sync = hw_inter.getHwCtrlObj(lima.HwCap.Sync)
print type(hw_sync)

print "Getting HW Bin"
hw_bin = hw_inter.getHwCtrlObj(lima.HwCap.Bin)
print type(hw_bin)

print "Getting HW RoI"
hw_roi = hw_inter.getHwCtrlObj(lima.HwCap.Roi)
print type(hw_roi)


class TestFrameCallback( lima.HwFrameCallback ):
	def __init__(self, hw_inter, soft_roi, buffer_save, acq_finished):
		self.m_hw_inter = hw_inter
		self.m_soft_roi = soft_roi
		self.m_roi_task = lima.SoftRoi()
		self.m_roi_cb   = lima.SoftRoiCallback(hw_inter, buffer_save, 
		                                       acq_finished)

	def newFrameReady(self, frame_info):
		print "newFrameReady!"

soft_roi = lima.Roi()
#acq_finished = lima.Cond()  # Thread utils are not wrapped!
#print "Creating a TestFrameCallback"
#cb = TestFrameCallback(hw_inter, soft_roi, buffer_save, acq_finished)

s = raw_input('Reset the hardware? (y/n):')
if s[0] == 'y' or s[0] == 'Y':
	hw_inter.reset(lima.HwInterface.HardReset)
	print "  Done!"

size = hw_det_info.getMaxImageSize()
image_type = hw_det_info.getCurrImageType()
frame_dim = lima.FrameDim(size, image_type)

bin = lima.Bin(lima.Point(1))
hw_bin.setBin(bin)

#Roi set_roi, real_roi;
#set_hw_roi(hw_roi, set_roi, real_roi, soft_roi);

effect_frame_dim = lima.FrameDim(frame_dim)  # was (frame_dim / bin)
hw_buffer.setFrameDim(effect_frame_dim)
hw_buffer.setNbBuffers(10)
#hw_buffer.registerFrameCallback(cb)

hw_sync.setExpTime(2)
hw_sync.setNbFrames(3)

print "Starting Acquisition"
hw_inter.startAcq()

#acq_finished.wait()
#PoolThreadMgr::get().wait();
print "Waiting 10 seconds..."
time.sleep(10)

print "Stopping Acquisition"
hw_inter.stopAcq()


print "This is the End..."
