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
}

DetInfoCtrlObj::~DetInfoCtrlObj()
{
}

void DetInfoCtrlObj::getMaxImageSize(Size& max_image_size)
{
	FrameDim max_frame_dim;
	m_cam.getFrameDim(max_frame_dim);
	max_image_size = max_frame_dim.getSize();
}

void DetInfoCtrlObj::getDetectorImageSize(Size& det_image_size)
{
	det_image_size = MaxFrameDim.getSize();
}

void DetInfoCtrlObj::getDefImageType(ImageType& def_image_type)
{
	def_image_type = MaxFrameDim.getImageType();
}

void DetInfoCtrlObj::getCurrImageType(ImageType& curr_image_type)
{
	FrameDim max_frame_dim;
	m_cam.getFrameDim(max_frame_dim);
	curr_image_type = max_frame_dim.getImageType();
}

void DetInfoCtrlObj::setCurrImageType(ImageType curr_image_type)
{
	ImageType unique_image_type;
	getCurrImageType(unique_image_type);
	if (curr_image_type != unique_image_type)
		throw LIMA_HW_EXC(InvalidValue, "Only one image type allowed");
}

void DetInfoCtrlObj::getPixelSize(double& pixel_size)
{
	bool is_frelon_4m;
	m_cam.isFrelon4M(is_frelon_4m);
	ChipType chip_type = is_frelon_4m ? Kodak : Atmel;
	pixel_size = ChipPixelSizeMap[chip_type];
}

void DetInfoCtrlObj::getDetectorType(std::string& det_type)
{
	det_type = "Frelon";
}

void DetInfoCtrlObj::getDetectorModel(std::string& det_model)
{
	bool is_frelon_4m;
	m_cam.isFrelon4M(is_frelon_4m);

	det_model = is_frelon_4m ? "4M" : "2k";
	if (!is_frelon_4m) {
		bool is_frelon_2k16;
		m_cam.isFrelon2k16(is_frelon_2k16);
		det_model += is_frelon_2k16 ? "16" : "14";
	}

	bool has_taper;
	m_cam.hasTaper(has_taper);
	if (has_taper)
		det_model += "T";
}

void DetInfoCtrlObj::setMaxImageSizeCallbackActive(bool cb_active)
{
}


/*******************************************************************
 * \brief BufferCtrlObj constructor
 *******************************************************************/

BufferCtrlObj::BufferCtrlObj(BufferCtrlMgr& buffer_mgr)
	: m_buffer_mgr(buffer_mgr)
{
}

BufferCtrlObj::~BufferCtrlObj()
{
}

void BufferCtrlObj::setFrameDim(const FrameDim& frame_dim)
{
	m_buffer_mgr.setFrameDim(frame_dim);
}

void BufferCtrlObj::getFramedim(FrameDim& frame_dim)
{
	m_buffer_mgr.getFrameDim(frame_dim);
}

void BufferCtrlObj::setNbBuffers(int nb_buffers)
{
	m_buffer_mgr.setNbBuffers(nb_buffers);
}

void BufferCtrlObj::getNbBuffers(int& nb_buffers)
{
	m_buffer_mgr.getNbBuffers(nb_buffers);
}

void BufferCtrlObj::setNbConcatFrames(int nb_concat_frames)
{
	m_buffer_mgr.setNbConcatFrames(nb_concat_frames);
}

void BufferCtrlObj::getNbConcatFrames(int& nb_concat_frames)
{
	m_buffer_mgr.getNbConcatFrames(nb_concat_frames);
}

void BufferCtrlObj::setNbAccFrames(int nb_acc_frames)
{
	m_buffer_mgr.setNbAccFrames(nb_acc_frames);
}

void BufferCtrlObj::getNbAccFrames(int& nb_acc_frames)
{
	m_buffer_mgr.getNbAccFrames(nb_acc_frames);
}

void BufferCtrlObj::getMaxNbBuffers(int& max_nb_buffers)
{
	m_buffer_mgr.getMaxNbBuffers(max_nb_buffers);

}

void *BufferCtrlObj::getBufferPtr(int buffer_nb, int concat_frame_nb)
{
	return m_buffer_mgr.getBufferPtr(buffer_nb, concat_frame_nb);
}

void *BufferCtrlObj::getFramePtr(int acq_frame_nb)
{
	return m_buffer_mgr.getFramePtr(acq_frame_nb);
}

void BufferCtrlObj::getStartTimestamp(Timestamp& start_ts)
{
	m_buffer_mgr.getStartTimestamp(start_ts);
}

void BufferCtrlObj::getFrameInfo(int acq_frame_nb, HwFrameInfoType& info)
{
	m_buffer_mgr.getFrameInfo(acq_frame_nb, info);
}

void BufferCtrlObj::registerFrameCallback(HwFrameCallback& frame_cb)
{
	m_buffer_mgr.registerFrameCallback(frame_cb);
}

void BufferCtrlObj::unregisterFrameCallback(HwFrameCallback& frame_cb)
{
	m_buffer_mgr.unregisterFrameCallback(frame_cb);
}


/*******************************************************************
 * \brief SyncCtrlObj constructor
 *******************************************************************/

SyncCtrlObj::SyncCtrlObj(Acq& acq, Camera& cam, BufferCtrlObj& buffer_ctrl)
	: m_acq(acq), m_cam(cam), m_buffer_ctrl(buffer_ctrl)
{
}

SyncCtrlObj::~SyncCtrlObj()
{
}

void SyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
	m_cam.setTrigMode(trig_mode);
}

void SyncCtrlObj::getTrigMode(TrigMode& trig_mode)
{
	m_cam.getTrigMode(trig_mode);
}

void SyncCtrlObj::setExpTime(double exp_time)
{
	m_cam.setExpTime(exp_time);
}

void SyncCtrlObj::getExpTime(double& exp_time)
{
	m_cam.getExpTime(exp_time);
}

void SyncCtrlObj::setLatTime(double lat_time)
{
	m_cam.setLatTime(lat_time);
}

void SyncCtrlObj::getLatTime(double& lat_time)
{
	m_cam.getLatTime(lat_time);
}

void SyncCtrlObj::setNbFrames(int nb_frames)
{
	int nb_acc_frames;
	m_buffer_ctrl.getNbAccFrames(nb_acc_frames);
	int real_nb_frames = nb_frames * nb_acc_frames;
	m_acq.setNbFrames(real_nb_frames);
	m_cam.setNbFrames(real_nb_frames);
}

void SyncCtrlObj::getNbFrames(int& nb_frames)
{
	m_acq.getNbFrames(nb_frames);
	int nb_acc_frames;
	m_buffer_ctrl.getNbAccFrames(nb_acc_frames);
	nb_frames /= nb_acc_frames;
}

void SyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges)
{
}


/*******************************************************************
 * \brief BinCtrlObj constructor
 *******************************************************************/

BinCtrlObj::BinCtrlObj(Camera& cam)
	: m_cam(cam)
{
}

BinCtrlObj::~BinCtrlObj()
{
}

void BinCtrlObj::setBin(const Bin& bin)
{
	m_cam.setBin(bin);
}

void BinCtrlObj::getBin(Bin& bin)
{
	m_cam.getBin(bin);
}

void BinCtrlObj::checkBin(Bin& bin)
{
	m_cam.checkBin(bin);
}


/*******************************************************************
 * \brief RoiCtrlObj constructor
 *******************************************************************/

RoiCtrlObj::RoiCtrlObj(Camera& cam)
	: m_cam(cam)
{
}

RoiCtrlObj::~RoiCtrlObj()
{
}

void RoiCtrlObj::checkRoi(const Roi& set_roi, Roi& hw_roi)
{
	m_cam.checkRoi(set_roi, hw_roi);
}

void RoiCtrlObj::setRoi(const Roi& roi)
{
	m_cam.setRoi(roi);
}

void RoiCtrlObj::getRoi(Roi& roi)
{
	m_cam.getRoi(roi);
}


/*******************************************************************
 * \brief Hw Interface constructor
 *******************************************************************/

Interface::Interface(Acq& acq, BufferCtrlMgr& buffer_mgr,
		     Camera& cam)
	: m_acq(acq), m_buffer_mgr(buffer_mgr), m_cam(cam),
	  m_det_info(cam), m_buffer(buffer_mgr), m_sync(acq, cam, m_buffer), 
	  m_bin(cam), m_roi(cam)
{
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

	reset(SoftReset);
}

Interface::~Interface()
{
}

const HwInterface::CapList& Interface::getCapList() const
{
	return m_cap_list;
}

void Interface::reset(ResetLevel reset_level)
{
	m_acq.stop();

	if (reset_level == HardReset)
		m_cam.hardReset();

	m_cam.setFrameTransferMode(FFM);

	m_sync.setNbFrames(1);
	m_sync.setExpTime(1.0);
	m_sync.setLatTime(0.0);
	m_sync.setTrigMode(IntTrig);

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
}

void Interface::startAcq()
{
	m_buffer_mgr.setStartTimestamp(Timestamp::now());
	m_acq.start();
	m_cam.start();
}

void Interface::stopAcq()
{
	m_cam.stop();
	m_acq.stop();
}

void Interface::getStatus(StatusType& status)
{
	Acq::Status acq;
	m_acq.getStatus(acq);
	status.acq = acq.running ? AcqRunning : AcqReady;

	static const DetStatus det_mask = (WaitForTrigger | Exposure    | 
					   ShutterClose   | ChargeShift | 
					   Readout        | Latency);
	status.det_mask = det_mask;
	status.det = DetIdle;
	Frelon::Status cam;
	m_cam.getStatus(cam);
	if (cam & Frelon::Wait)
		status.det |= WaitForTrigger;
	if (cam & Frelon::Exposure)
		status.det |= Exposure;
	if (cam & Frelon::Shutter)
		status.det |= ShutterClose;
	if (cam & Frelon::Transfer)
		status.det |= ChargeShift;
	if (cam & Frelon::Readout)
		status.det |= Readout;
	if (cam & Frelon::Latency)
		status.det |= Latency;
}

int Interface::getNbAcquiredFrames()
{
	Acq::Status acq_status;
	m_acq.getStatus(acq_status);
	return acq_status.last_frame_nb + 1;
}


