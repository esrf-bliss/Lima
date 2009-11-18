import sys
import lima
import processlib
import time
import numpy as N

Data_UNDEF  = 0
Data_UINT8  = 1
Data_INT8   = 2
Data_UINT16 = 3
Data_INT16  = 4
Data_UINT32 = 5
Data_INT32  = 6
Data_UINT64 = 7
Data_INT64  = 8
Data_FLOAT  = 9
Data_DOUBLE = 10
	

class SoftRoiCallback( processlib.TaskEventCallback ):

	DataType2ImageType = {
		N.int8:   lima.Bpp8,
		N.uint8:  lima.Bpp8,
		N.int16:  lima.Bpp16,
		N.uint16: lima.Bpp16,
		N.int32:  lima.Bpp32,
		N.uint32: lima.Bpp32
	}
		
	def __init__(self, hw_inter, buffer_save, acq_state):
		processlib.TaskEventCallback.__init__(self)
		self.m_hw_inter = hw_inter
		self.m_buffer_save = buffer_save
		self.m_acq_state = acq_state

	def finished(self, data):
		finfo, fdim = self.data2FrameInfo(data)
		self.m_buffer_save.writeFrame(finfo)

		hw_sync = self.m_hw_inter.getHwCtrlObj(lima.HwCap.Sync)
		nb_frames = hw_sync.getNbFrames()
		if finfo.acq_frame_nb == nb_frames - 1:
			self.m_acq_state.set(lima.AcqState.Finished)

	def data2FrameInfo(self, data):
		arr = data.buffer
		arr_type = arr.dtype.type
		arr_height, arr_width = arr.shape
		
		image_type = self.DataType2ImageType[arr_type]

		buffer_ctrl = self.m_hw_inter.getHwCtrlObj(lima.HwCap.Buffer)
		start_ts = buffer_ctrl.getStartTimestamp()

		fdim = lima.FrameDim(arr_width, arr_height, image_type)
		timestamp = lima.Timestamp(data.timestamp)
		valid_pixels = lima.Point(fdim.getSize()).getArea()

		finfo = lima.HwFrameInfoType(data.frameNumber, arr, fdim,
					     timestamp, valid_pixels)
		return finfo, fdim

	
class TestFrameCallback( lima.HwFrameCallback ):

	ImageType2DataType = {
		lima.Bpp8:  Data_UINT8,
		lima.Bpp16: Data_UINT16,
		lima.Bpp32: Data_UINT32
	}
	
	def __init__(self, hw_inter, soft_roi, buffer_save, acq_state):
		lima.HwFrameCallback.__init__(self)
		self.m_hw_inter = hw_inter
		self.m_soft_roi = soft_roi
		self.m_acq_state = acq_state
		self.m_roi_task = processlib.Tasks.SoftRoi()
		self.m_roi_cb   = SoftRoiCallback(hw_inter, buffer_save, 
						  acq_state)

	def newFrameReady(self, frame_info):
		msg  = 'acq_frame_nb=%d, ' % frame_info.acq_frame_nb
		fdim = frame_info.frame_dim
		size = fdim.getSize()
		msg += 'frame_dim=%dx%dx%d, ' % \
		       (size.getWidth(), size.getHeight(), fdim.getDepth())
		msg += 'frame_timestamp=%.6f, ' % frame_info.frame_timestamp
		msg += 'valid_pixels=%d' % frame_info.valid_pixels
		print 'newFrameReady:', msg

		data = self.frameInfo2Data(frame_info)
		
		if self.m_soft_roi.isActive():
			pass
		else:
			self.m_roi_cb.finished(data)
			
		return True

	def frameInfo2Data(self, frame_info):
		data = processlib.Data()
		data.buffer = frame_info.frame_ptr
		data.frameNumber = frame_info.acq_frame_nb
		data.timestamp = frame_info.frame_timestamp
		
		return data


class MaxImageSizeCallback( lima.HwMaxImageSizeCallback ):

	def maxImageSizeChanged(self, size, image_type):
		fdim = lima.FrameDim(size, image_type)
		msg = "size=%sx%s, image_type=%s, depth=%d" % \
		      (size.getWidth(), size.getHeight(), image_type, \
		       fdim.getDepth())
		print "MaxImageSizeChanged:", msg


def main(argv):
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
	cam = lima.Frelon.Camera(eser_line)

	print "Creating the Hw Interface ... "
	hw_inter = lima.Frelon.Interface(acq, buffer_mgr, cam)

	print "Creating BufferSave"
	buffer_save = lima.BufferSave(lima.BufferSave.EDF, "img", 0, ".edf",
				      True, 1)

	print "Getting HW detector info"
	hw_det_info = hw_inter.getHwCtrlObj(lima.HwCap.DetInfo)

	print "Getting HW buffer"
	hw_buffer = hw_inter.getHwCtrlObj(lima.HwCap.Buffer)

	print "Getting HW Sync"
	hw_sync = hw_inter.getHwCtrlObj(lima.HwCap.Sync)

	print "Getting HW Bin"
	hw_bin = hw_inter.getHwCtrlObj(lima.HwCap.Bin)

	print "Getting HW RoI"
	hw_roi = hw_inter.getHwCtrlObj(lima.HwCap.Roi)

	mis_cb = MaxImageSizeCallback()
	hw_det_info.registerMaxImageSizeCallback(mis_cb);

	print "Setting FTM";
	cam.setFrameTransferMode(lima.Frelon.FTM)
	print "Setting FFM";
	cam.setFrameTransferMode(lima.Frelon.FFM)
	
	soft_roi = lima.Roi()
	acq_state = lima.AcqState()
	print "Creating a TestFrameCallback"
	cb = TestFrameCallback(hw_inter, soft_roi, buffer_save, acq_state)

	do_reset = False
	if do_reset:
		print "Reseting hardware ..."
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
	hw_buffer.registerFrameCallback(cb)

	hw_sync.setExpTime(2)
	hw_sync.setNbFrames(3)

	print "Starting Acquisition"
	acq_state.set(lima.AcqState.Running)
	hw_inter.startAcq()

	print "Waiting acq finished..."
	acq_state.waitNot(lima.AcqState.Running)
	print "Acq finished!!"

	print "Stopping Acquisition"
	hw_inter.stopAcq()

	print "This is the End..."


if __name__ == '__main__':
	main(sys.argv)
