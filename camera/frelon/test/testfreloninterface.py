import sys
from lima import *
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
	

glob_deb_params = DebParams(DebModTest)

class SoftRoiCallback( processlib.TaskEventCallback ):

	deb_params = DebParams(DebModTest, "SoftRoiCallback")

	DataType2ImageType = {
		N.int8:   Bpp8,
		N.uint8:  Bpp8,
		N.int16:  Bpp16,
		N.uint16: Bpp16,
		N.int32:  Bpp32,
		N.uint32: Bpp32
	}
		
	def __init__(self, hw_inter, buffer_save, acq_state):
		deb_obj = DebObj(self.deb_params, "__init__")
		
		processlib.TaskEventCallback.__init__(self)
		self.m_hw_inter = hw_inter
		self.m_buffer_save = buffer_save
		self.m_acq_state = acq_state

	def finished(self, data):
		deb_obj = DebObj(self.deb_params, "finished")
		
		finfo, fdim = self.data2FrameInfo(data)
		self.m_buffer_save.writeFrame(finfo)

		hw_sync = self.m_hw_inter.getHwCtrlObj(HwCap.Sync)
		nb_frames = hw_sync.getNbFrames()
		if finfo.acq_frame_nb == nb_frames - 1:
			self.m_acq_state.set(AcqState.Finished)

	def data2FrameInfo(self, data):
		deb_obj = DebObj(self.deb_params, "data2FrameInfo")
		
		arr = data.buffer
		arr_type = arr.dtype.type
		arr_height, arr_width = arr.shape
		
		image_type = self.DataType2ImageType[arr_type]

		buffer_ctrl = self.m_hw_inter.getHwCtrlObj(HwCap.Buffer)
		start_ts = buffer_ctrl.getStartTimestamp()

		fdim = FrameDim(arr_width, arr_height, image_type)
		timestamp = Timestamp(data.timestamp)
		valid_pixels = Point(fdim.getSize()).getArea()

		finfo = HwFrameInfoType(data.frameNumber, arr, fdim,
					timestamp, valid_pixels)
		return finfo, fdim

	
class TestFrameCallback( HwFrameCallback ):

	deb_params = DebParams(DebModTest, "TestFrameCallback")

	ImageType2DataType = {
		Bpp8:  Data_UINT8,
		Bpp16: Data_UINT16,
		Bpp32: Data_UINT32
	}
	
	def __init__(self, hw_inter, soft_roi, buffer_save, acq_state):
		deb_obj = DebObj(self.deb_params, "__init__")
		
		HwFrameCallback.__init__(self)
		self.m_hw_inter = hw_inter
		self.m_soft_roi = soft_roi
		self.m_acq_state = acq_state
		self.m_roi_task = processlib.Tasks.SoftRoi()
		self.m_roi_cb   = SoftRoiCallback(hw_inter, buffer_save, 
						  acq_state)

	def newFrameReady(self, frame_info):
		deb_obj = DebObj(self.deb_params, "newFrameReady")
		
		msg  = 'acq_frame_nb=%d, ' % frame_info.acq_frame_nb
		fdim = frame_info.frame_dim
		size = fdim.getSize()
		msg += 'frame_dim=%dx%dx%d, ' % \
		       (size.getWidth(), size.getHeight(), fdim.getDepth())
		msg += 'frame_timestamp=%.6f, ' % frame_info.frame_timestamp
		msg += 'valid_pixels=%d' % frame_info.valid_pixels
		deb_obj.Always("newFrameReady: %s" % msg)

		data = self.frameInfo2Data(frame_info)
		
		if self.m_soft_roi.isActive():
			pass
		else:
			self.m_roi_cb.finished(data)
			
		return True

	def frameInfo2Data(self, frame_info):
		deb_obj = DebObj(self.deb_params, "frameInfo2Data")
		
		data = processlib.Data()
		data.buffer = frame_info.frame_ptr
		data.frameNumber = frame_info.acq_frame_nb
		data.timestamp = frame_info.frame_timestamp
		
		return data


class MaxImageSizeCallback( HwMaxImageSizeCallback ):

	deb_params = DebParams(DebModTest, "MaxImageSizeCallback")
	
	def maxImageSizeChanged(self, size, image_type):
		deb_obj = DebObj(self.deb_params, "maxImageSizeChanged")
		
		fdim = FrameDim(size, image_type)
		msg = "size=%sx%s, image_type=%s, depth=%d" % \
		      (size.getWidth(), size.getHeight(), image_type, \
		       fdim.getDepth())
		deb_obj.Always("MaxImageSizeChanged: " % msg)


def main(argv):

	deb_obj = DebObj(glob_deb_params, "main")
	
	deb_obj.Always("Creating Espia.Dev")
	edev = Espia.Dev(0)

	deb_obj.Always("Creating Espia.Acq")
	acq = Espia.Acq(edev)

	acqstat = acq.getStatus()
	deb_obj.Always("Whether the Acquisition is running : %s" %
		       acqstat.running)

	deb_obj.Always("Creating Espia.BufferMgr")
	buffer_cb_mgr = Espia.BufferMgr(acq)

	deb_obj.Always("Creating BufferCtrlMgr")
	buffer_mgr = BufferCtrlMgr(buffer_cb_mgr)

	deb_obj.Always("Creating Espia.SerialLine")
	eser_line = Espia.SerialLine(edev)

	deb_obj.Always("Creating Frelon.Camera")
	cam = Frelon.Camera(eser_line)

	deb_obj.Always("Creating the Hw Interface ... ")
	hw_inter = Frelon.Interface(acq, buffer_mgr, cam)

	deb_obj.Always("Creating BufferSave")
	buffer_save = BufferSave(BufferSave.EDF, "img", 0, ".edf", True, 1)

	deb_obj.Always("Getting HW detector info")
	hw_det_info = hw_inter.getHwCtrlObj(HwCap.DetInfo)

	deb_obj.Always("Getting HW buffer")
	hw_buffer = hw_inter.getHwCtrlObj(HwCap.Buffer)

	deb_obj.Always("Getting HW Sync")
	hw_sync = hw_inter.getHwCtrlObj(HwCap.Sync)

	deb_obj.Always("Getting HW Bin")
	hw_bin = hw_inter.getHwCtrlObj(HwCap.Bin)

	deb_obj.Always("Getting HW RoI")
	hw_roi = hw_inter.getHwCtrlObj(HwCap.Roi)

	mis_cb = MaxImageSizeCallback()
	hw_det_info.registerMaxImageSizeCallback(mis_cb)

	deb_obj.Always("Setting FTM")
	cam.setFrameTransferMode(Frelon.FTM)
	deb_obj.Always("Setting FFM")
	cam.setFrameTransferMode(Frelon.FFM)
	
	soft_roi = Roi()
	acq_state = AcqState()
	deb_obj.Always("Creating a TestFrameCallback")
	cb = TestFrameCallback(hw_inter, soft_roi, buffer_save, acq_state)

	do_reset = False
	if do_reset:
		deb_obj.Always("Reseting hardware ...")
		hw_inter.reset(HwInterface.HardReset)
		deb_obj.Always("  Done!")

	size = hw_det_info.getMaxImageSize()
	image_type = hw_det_info.getCurrImageType()
	frame_dim = FrameDim(size, image_type)

	bin = Bin(Point(1))
	hw_bin.setBin(bin)

	#Roi set_roi, real_roi;
	#set_hw_roi(hw_roi, set_roi, real_roi, soft_roi);

	effect_frame_dim = FrameDim(frame_dim)  # was (frame_dim / bin)
	hw_buffer.setFrameDim(effect_frame_dim)
	hw_buffer.setNbBuffers(10)
	hw_buffer.registerFrameCallback(cb)

	hw_sync.setExpTime(2)
	hw_sync.setNbFrames(3)

	deb_obj.Always("Starting Acquisition")
	acq_state.set(AcqState.Acquiring)
	hw_inter.startAcq()

	deb_obj.Always("Waiting acq finished...")
	acq_state.waitNot(AcqState.Acquiring)
	deb_obj.Always("Acq finished!!")

	deb_obj.Always("Stopping Acquisition")
	hw_inter.stopAcq()

	deb_obj.Always("This is the End...")


if __name__ == '__main__':
	main(sys.argv)
