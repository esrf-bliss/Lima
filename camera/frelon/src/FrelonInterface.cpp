#include "FrelonInterface.h"
#include <algorithm>

using namespace lima;
using namespace lima::Espia;
using namespace lima::Frelon;
using namespace std;

/*******************************************************************
 * \brief DetInfoCtrlObj constructor
 *******************************************************************/

DetInfoCtrlObj::DetInfoCtrlObj(Camera& cam)
	: m_cam(cam)
{
	DEB_CONSTRUCTOR();
}

DetInfoCtrlObj::~DetInfoCtrlObj()
{
	DEB_DESTRUCTOR();
}

void DetInfoCtrlObj::getMaxImageSize(Size& max_image_size)
{
	DEB_MEMBER_FUNCT();
	FrameDim max_frame_dim;
	m_cam.getFrameDim(max_frame_dim);
	max_image_size = max_frame_dim.getSize();
}

void DetInfoCtrlObj::getDetectorImageSize(Size& det_image_size)
{
	DEB_MEMBER_FUNCT();
	det_image_size = MaxFrameDim.getSize();
}

void DetInfoCtrlObj::getDefImageType(ImageType& def_image_type)
{
	DEB_MEMBER_FUNCT();
	def_image_type = MaxFrameDim.getImageType();
}

void DetInfoCtrlObj::getCurrImageType(ImageType& curr_image_type)
{
	DEB_MEMBER_FUNCT();
	FrameDim max_frame_dim;
	m_cam.getFrameDim(max_frame_dim);
	curr_image_type = max_frame_dim.getImageType();
}

void DetInfoCtrlObj::setCurrImageType(ImageType curr_image_type)
{
	DEB_MEMBER_FUNCT();
	ImageType unique_image_type;
	getCurrImageType(unique_image_type);
	if (curr_image_type != unique_image_type)
		THROW_HW_ERROR(InvalidValue) 
			<< "Only " << DEB_VAR1(unique_image_type) << "allowed";
}

void DetInfoCtrlObj::getPixelSize(double& pixel_size)
{
	DEB_MEMBER_FUNCT();
	Model& model = m_cam.getModel();
	pixel_size = model.getPixelSize();
	DEB_RETURN() << DEB_VAR1(pixel_size);
}

void DetInfoCtrlObj::getDetectorType(std::string& det_type)
{
	DEB_MEMBER_FUNCT();
	det_type = "Frelon";
	DEB_RETURN() << DEB_VAR1(det_type);
}

void DetInfoCtrlObj::getDetectorModel(std::string& det_model)
{
	DEB_MEMBER_FUNCT();
	Model& model = m_cam.getModel();
	det_model = model.getName();
	DEB_RETURN() << DEB_VAR1(det_model);
}

void DetInfoCtrlObj::registerMaxImageSizeCallback(
					HwMaxImageSizeCallback& cb)
{
	DEB_MEMBER_FUNCT();
	m_cam.registerMaxImageSizeCallback(cb);
}

void DetInfoCtrlObj::unregisterMaxImageSizeCallback(
					HwMaxImageSizeCallback& cb)
{
	DEB_MEMBER_FUNCT();
	m_cam.unregisterMaxImageSizeCallback(cb);
}


/*******************************************************************
 * \brief BufferCtrlObj constructor
 *******************************************************************/

BufferCtrlObj::BufferCtrlObj(BufferCtrlMgr& buffer_mgr)
	: m_buffer_mgr(buffer_mgr)
{
	DEB_CONSTRUCTOR();
}

BufferCtrlObj::~BufferCtrlObj()
{
	DEB_DESTRUCTOR();
}

void BufferCtrlObj::setFrameDim(const FrameDim& frame_dim)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.setFrameDim(frame_dim);
}

void BufferCtrlObj::getFrameDim(FrameDim& frame_dim)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.getFrameDim(frame_dim);
}

void BufferCtrlObj::setNbBuffers(int nb_buffers)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.setNbBuffers(nb_buffers);
}

void BufferCtrlObj::getNbBuffers(int& nb_buffers)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.getNbBuffers(nb_buffers);
}

void BufferCtrlObj::setNbConcatFrames(int nb_concat_frames)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.setNbConcatFrames(nb_concat_frames);
}

void BufferCtrlObj::getNbConcatFrames(int& nb_concat_frames)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.getNbConcatFrames(nb_concat_frames);
}

void BufferCtrlObj::setNbAccFrames(int nb_acc_frames)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.setNbAccFrames(nb_acc_frames);
}

void BufferCtrlObj::getNbAccFrames(int& nb_acc_frames)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.getNbAccFrames(nb_acc_frames);
}

void BufferCtrlObj::getMaxNbBuffers(int& max_nb_buffers)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.getMaxNbBuffers(max_nb_buffers);

}

void *BufferCtrlObj::getBufferPtr(int buffer_nb, int concat_frame_nb)
{
	DEB_MEMBER_FUNCT();
	return m_buffer_mgr.getBufferPtr(buffer_nb, concat_frame_nb);
}

void *BufferCtrlObj::getFramePtr(int acq_frame_nb)
{
	DEB_MEMBER_FUNCT();
	return m_buffer_mgr.getFramePtr(acq_frame_nb);
}

void BufferCtrlObj::getStartTimestamp(Timestamp& start_ts)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.getStartTimestamp(start_ts);
}

void BufferCtrlObj::getFrameInfo(int acq_frame_nb, HwFrameInfoType& info)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.getFrameInfo(acq_frame_nb, info);
}

void BufferCtrlObj::registerFrameCallback(HwFrameCallback& frame_cb)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.registerFrameCallback(frame_cb);
}

void BufferCtrlObj::unregisterFrameCallback(HwFrameCallback& frame_cb)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.unregisterFrameCallback(frame_cb);
}


/*******************************************************************
 * \brief SyncCtrlObj constructor
 *******************************************************************/

SyncCtrlObj::SyncCtrlObj(Acq& acq, Camera& cam, BufferCtrlObj& buffer_ctrl)
	: HwSyncCtrlObj(buffer_ctrl), m_acq(acq), m_cam(cam), m_acq_end_cb(cam)
{
	DEB_CONSTRUCTOR();
}

SyncCtrlObj::~SyncCtrlObj()
{
	DEB_DESTRUCTOR();
}

bool SyncCtrlObj::checkTrigMode(TrigMode trig_mode)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(trig_mode);

	bool valid_mode;
	switch (trig_mode) {
	case IntTrig:
	case ExtTrigSingle:
	case ExtTrigMult:
	case ExtGate:
		valid_mode = true;
		break;

	default:
		valid_mode = false;
	}

	DEB_RETURN() << DEB_VAR1(valid_mode);
	return valid_mode;
}

void SyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
	DEB_MEMBER_FUNCT();

	if (!checkTrigMode(trig_mode))
		THROW_HW_ERROR(InvalidValue) << "Invalid " 
					     << DEB_VAR1(trig_mode);
	m_cam.setTrigMode(trig_mode);
}

void SyncCtrlObj::getTrigMode(TrigMode& trig_mode)
{
	DEB_MEMBER_FUNCT();
	m_cam.getTrigMode(trig_mode);
}

void SyncCtrlObj::setExpTime(double exp_time)
{
	DEB_MEMBER_FUNCT();
	m_cam.setExpTime(exp_time);
}

void SyncCtrlObj::getExpTime(double& exp_time)
{
	DEB_MEMBER_FUNCT();
	m_cam.getExpTime(exp_time);
}

void SyncCtrlObj::setLatTime(double lat_time)
{
	DEB_MEMBER_FUNCT();
	m_cam.setLatTime(lat_time);
}

void SyncCtrlObj::getLatTime(double& lat_time)
{
	DEB_MEMBER_FUNCT();
	m_cam.getLatTime(lat_time);
}

void SyncCtrlObj::setNbHwFrames(int nb_frames)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(nb_frames);

	m_acq.setNbFrames(nb_frames);

	int cam_nb_frames = nb_frames;
	if (cam_nb_frames > MaxRegVal) {
		DEB_TRACE() << "Too many frames: setting camera endless acq";
		cam_nb_frames = 0;
		if (!m_acq_end_cb.getAcq())
			m_acq.registerAcqEndCallback(m_acq_end_cb);
	} else if (m_acq_end_cb.getAcq())
			m_acq.unregisterAcqEndCallback(m_acq_end_cb);

	m_cam.setNbFrames(cam_nb_frames);
}

void SyncCtrlObj::getNbHwFrames(int& nb_frames)
{
	DEB_MEMBER_FUNCT();
	m_acq.getNbFrames(nb_frames);
}

void SyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges)
{
	DEB_MEMBER_FUNCT();

	const double MinTimeUnit = TimeUnitFactorMap[Microseconds];
	const double MaxTimeUnit = TimeUnitFactorMap[Milliseconds];
	const double LatTimeUnit = TimeUnitFactorMap[Milliseconds];

	valid_ranges.min_exp_time = 1 * MinTimeUnit;
	valid_ranges.max_exp_time = MaxRegVal * MaxTimeUnit;

	valid_ranges.min_lat_time = 0 * LatTimeUnit;
	valid_ranges.max_lat_time = MaxRegVal * LatTimeUnit;

	DEB_RETURN() << DEB_VAR2(valid_ranges.min_exp_time, 
				 valid_ranges.max_exp_time);
	DEB_RETURN() << DEB_VAR2(valid_ranges.min_lat_time, 
				 valid_ranges.max_lat_time);
}


/*******************************************************************
 * \brief SyncCtrlObj::AcqEndCallback constructor
 *******************************************************************/

SyncCtrlObj::AcqEndCallback::AcqEndCallback(Camera& cam) 
	: m_cam(cam) 
{
	DEB_CONSTRUCTOR();
}

SyncCtrlObj::AcqEndCallback::~AcqEndCallback()
{
	DEB_DESTRUCTOR();
}

void SyncCtrlObj::AcqEndCallback::acqFinished(const HwFrameInfoType& /*finfo*/)
{
	DEB_MEMBER_FUNCT();
	m_cam.stop();
}


/*******************************************************************
 * \brief BinCtrlObj constructor
 *******************************************************************/

BinCtrlObj::BinCtrlObj(Camera& cam)
	: m_cam(cam)
{
	DEB_CONSTRUCTOR();
}

BinCtrlObj::~BinCtrlObj()
{
	DEB_DESTRUCTOR();
}

void BinCtrlObj::setBin(const Bin& bin)
{
	DEB_MEMBER_FUNCT();
	m_cam.setBin(bin);
}

void BinCtrlObj::getBin(Bin& bin)
{
	DEB_MEMBER_FUNCT();
	m_cam.getBin(bin);
}

void BinCtrlObj::checkBin(Bin& bin)
{
	DEB_MEMBER_FUNCT();
	m_cam.checkBin(bin);
}


/*******************************************************************
 * \brief RoiCtrlObj constructor
 *******************************************************************/

RoiCtrlObj::RoiCtrlObj(Camera& cam)
	: m_cam(cam)
{
	DEB_CONSTRUCTOR();
}

RoiCtrlObj::~RoiCtrlObj()
{
	DEB_DESTRUCTOR();
}

void RoiCtrlObj::checkRoi(const Roi& set_roi, Roi& hw_roi)
{
	DEB_MEMBER_FUNCT();
	m_cam.checkRoi(set_roi, hw_roi);
}

void RoiCtrlObj::setRoi(const Roi& roi)
{
	DEB_MEMBER_FUNCT();
	m_cam.setRoi(roi);
}

void RoiCtrlObj::getRoi(Roi& roi)
{
	DEB_MEMBER_FUNCT();
	m_cam.getRoi(roi);
}


/*******************************************************************
 * \brief FlipCtrlObj constructor
 *******************************************************************/

FlipCtrlObj::FlipCtrlObj(Camera& cam)
	: m_cam(cam)
{
	DEB_CONSTRUCTOR();
}

FlipCtrlObj::~FlipCtrlObj()
{
	DEB_DESTRUCTOR();
}

void FlipCtrlObj::setFlip(const Flip& flip)
{
	DEB_MEMBER_FUNCT();
	m_cam.setFlip(flip);
}

void FlipCtrlObj::getFlip(Flip& flip)
{
	DEB_MEMBER_FUNCT();
	m_cam.getFlip(flip);
}

void FlipCtrlObj::checkFlip(Flip& flip)
{
	DEB_MEMBER_FUNCT();
	m_cam.checkFlip(flip);
}


/*******************************************************************
 * \brief ShutterCtrlObj constructor
 *******************************************************************/

ShutterCtrlObj::ShutterCtrlObj(Camera& cam)
	: m_cam(cam)
{
	DEB_CONSTRUCTOR();
}

ShutterCtrlObj::~ShutterCtrlObj()
{
	DEB_DESTRUCTOR();
}

bool ShutterCtrlObj::checkMode(ShutterMode shut_mode) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(shut_mode);

	bool valid_mode;
	switch (shut_mode) {
	case ShutterManual:
	case ShutterAutoFrame:
		valid_mode = true;
		break;
	default:
		valid_mode = false;
	}

	DEB_RETURN() << DEB_VAR1(valid_mode);
	return valid_mode;
}

void ShutterCtrlObj::getModeList(ShutterModeList& mode_list) const
{
	DEB_MEMBER_FUNCT();
	mode_list.push_back(lima::ShutterManual);
	mode_list.push_back(lima::ShutterAutoFrame);	
}

void ShutterCtrlObj::setMode(ShutterMode shut_mode)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(shut_mode);

	if (!checkMode(shut_mode))
		THROW_HW_ERROR(NotSupported) << "Invalid (not supported) " 
					     << DEB_VAR1(shut_mode);

	ShutMode cam_mode;
	cam_mode = (shut_mode == ShutterAutoFrame) ? Frelon::AutoFrame : Frelon::Off;
	m_cam.setShutMode(cam_mode);
}

void ShutterCtrlObj::getMode(ShutterMode& shut_mode) const
{
	DEB_MEMBER_FUNCT();

	ShutMode cam_mode;
	m_cam.getShutMode(cam_mode);
	shut_mode = (cam_mode == Frelon::AutoFrame) ? ShutterAutoFrame : ShutterManual;
	DEB_RETURN() << DEB_VAR1(shut_mode);
}

void ShutterCtrlObj::setState(bool open)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(open);

	ShutterMode mode;
	getMode(mode);
	if (mode != ShutterManual)
		THROW_HW_ERROR(NotSupported) << "Not in manual shutter mode";
	else if (open)
		THROW_HW_ERROR(NotSupported) << "Manual shutter open "
					        "not supported";
}

void ShutterCtrlObj::getState(bool& open) const
{
	DEB_MEMBER_FUNCT();

	ShutterMode mode;
	getMode(mode);
	if (mode != ShutterManual)
		THROW_HW_ERROR(NotSupported) << "Not in manual shutter mode";

	open = false;
	DEB_RETURN() << DEB_VAR1(open);
}

void ShutterCtrlObj::setOpenTime(double shut_open_time)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(shut_open_time);

	if (shut_open_time != 0)
		THROW_HW_ERROR(NotSupported) << "Shutter open time not "
						"supported";
}

void ShutterCtrlObj::getOpenTime(double& shut_open_time) const
{
	DEB_MEMBER_FUNCT();
	shut_open_time = 0;
	DEB_RETURN() << DEB_VAR1(shut_open_time);
}

void ShutterCtrlObj::setCloseTime(double shut_close_time)
{
	DEB_MEMBER_FUNCT();
	m_cam.setShutCloseTime(shut_close_time);
}

void ShutterCtrlObj::getCloseTime(double& shut_close_time) const
{
	DEB_MEMBER_FUNCT();
	m_cam.getShutCloseTime(shut_close_time);
}


/*******************************************************************
 * \brief Hw Interface constructor
 *******************************************************************/

Interface::Interface(Acq& acq, BufferCtrlMgr& buffer_mgr,
		     Camera& cam)
	: m_acq(acq), m_buffer_mgr(buffer_mgr), m_cam(cam),
	  m_det_info(cam), m_buffer(buffer_mgr), m_sync(acq, cam, m_buffer), 
	  m_bin(cam), m_roi(cam), m_flip(cam), m_shutter(cam)
{
	DEB_CONSTRUCTOR();

	HwDetInfoCtrlObj *det_info = &m_det_info;
	m_cap_list.push_back(HwCap(det_info));

	HwBufferCtrlObj *buffer = &m_buffer;
	m_cap_list.push_back(HwCap(buffer));

	HwSyncCtrlObj *sync = &m_sync;
	m_cap_list.push_back(HwCap(sync));

	HwBinCtrlObj *bin = &m_bin;
	m_cap_list.push_back(HwCap(bin));

	HwRoiCtrlObj *roi = &m_roi;
	m_cap_list.push_back(HwCap(roi));

	HwFlipCtrlObj *flip = &m_flip;
	m_cap_list.push_back(HwCap(flip));

	HwShutterCtrlObj *shutter = &m_shutter;
	m_cap_list.push_back(HwCap(shutter));

	reset(SoftReset);
}

Interface::~Interface()
{
	DEB_DESTRUCTOR();
}

void Interface::getCapList(HwInterface::CapList &cap_list) const
{
	DEB_MEMBER_FUNCT();
	cap_list = m_cap_list;
}

void Interface::reset(ResetLevel reset_level)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(reset_level);

	stopAcq();

	if (reset_level == HardReset) {
		DEB_TRACE() << "Performing camera hard reset";
		m_cam.hardReset();
	}

	m_cam.setFrameTransferMode(FFM);
	InputChan input_chan;
	m_cam.getDefInputChan(FFM, input_chan);
	m_cam.setInputChan(input_chan);

	m_flip.setFlip(Flip(false));

	m_sync.setNbFrames(1);
	m_sync.setExpTime(1.0);
	m_sync.setLatTime(0.0);
	m_sync.setTrigMode(IntTrig);

	m_cam.setShutCloseTime(0.0);

	m_bin.setBin(Bin(1));
	m_roi.setRoi(Roi());
	
	Size image_size;
	m_det_info.getMaxImageSize(image_size);
	ImageType image_type;
	m_det_info.getDefImageType(image_type);
	FrameDim frame_dim(image_size, image_type);
	m_buffer.setFrameDim(frame_dim);

	m_buffer.setNbConcatFrames(1);
	m_buffer.setNbAccFrames(1);
	m_buffer.setNbBuffers(1);
}

void Interface::prepareAcq()
{
	DEB_MEMBER_FUNCT();
}

void Interface::startAcq()
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.setStartTimestamp(Timestamp::now());
	m_acq.start();
	m_cam.start();
}

void Interface::stopAcq()
{
	DEB_MEMBER_FUNCT();
	m_cam.stop();
	m_acq.stop();
}

void Interface::getStatus(StatusType& status)
{
	DEB_MEMBER_FUNCT();
	Acq::Status acq;
	m_acq.getStatus(acq);
	status.acq = acq.running ? AcqRunning : AcqReady;

	static const DetStatus det_mask = 
		(DetWaitForTrigger | DetExposure    | DetShutterClose   | 
		 DetChargeShift    | DetReadout     | DetLatency);

	status.det_mask = det_mask;
	status.det = DetIdle;
	Frelon::Status cam;
	m_cam.getStatus(cam);
	if (cam & Frelon::Wait)
		status.det |= DetWaitForTrigger;
	if (cam & Frelon::Exposure)
		status.det |= DetExposure;
	if (cam & Frelon::Shutter)
		status.det |= DetShutterClose;
	if (cam & Frelon::Transfer)
		status.det |= DetChargeShift;
	if (cam & Frelon::Readout)
		status.det |= DetReadout;
	if (cam & Frelon::Latency)
		status.det |= DetLatency;

	DEB_RETURN() << DEB_VAR1(status);
}

int Interface::getNbHwAcquiredFrames()
{
	DEB_MEMBER_FUNCT();
	Acq::Status acq_status;
	m_acq.getStatus(acq_status);
	int nb_hw_acq_frames = acq_status.last_frame_nb + 1;
	DEB_RETURN() << DEB_VAR1(nb_hw_acq_frames);
	return nb_hw_acq_frames;
}


