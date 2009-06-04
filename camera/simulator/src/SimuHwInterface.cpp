#include "SimuHwInterface.h"

using namespace lima;
using namespace std;

/*******************************************************************
 * \brief SimuDetInfoCtrlObj constructor
 *******************************************************************/

SimuDetInfoCtrlObj::SimuDetInfoCtrlObj(Simulator& simu)
	: m_simu(simu)
{
	m_iscb_act = false;
}

SimuDetInfoCtrlObj::~SimuDetInfoCtrlObj()
{
}

void SimuDetInfoCtrlObj::getMaxImageSize(Size& max_image_size)
{
	FrameDim fdim;
	m_simu.getFrameDim(fdim);
	max_image_size = fdim.getSize();
}

void SimuDetInfoCtrlObj::getDetectorImageSize(Size& det_image_size)
{
	m_simu.getMaxImageSize(det_image_size);
}

void SimuDetInfoCtrlObj::getDefImageType(ImageType& def_image_type)
{
	def_image_type = Bpp16;
}

void SimuDetInfoCtrlObj::setCurrImageType(ImageType curr_image_type)
{
	FrameDim fdim;
	m_simu.getFrameDim(fdim);
	fdim.setImageType(curr_image_type);
	m_simu.setFrameDim(fdim);
}

void SimuDetInfoCtrlObj::getCurrImageType(ImageType& curr_image_type)
{
	FrameDim fdim;
	m_simu.getFrameDim(fdim);
	curr_image_type = fdim.getImageType();
}

void SimuDetInfoCtrlObj::getPixelSize(double& pixel_size)
{
	pixel_size = 1e-6;
}

void SimuDetInfoCtrlObj::getDetectorType(string& det_type)
{
	det_type = "Simulator";
}

void SimuDetInfoCtrlObj::getDetectorModel(string& det_model)
{
	det_model = "PeakGenerator";
}

void SimuDetInfoCtrlObj::setMaxImageSizeCallbackActive(bool cb_active)
{
	m_iscb_act = cb_active;
}


/*******************************************************************
 * \brief SimuBufferCtrlObj constructor
 *******************************************************************/

SimuBufferCtrlObj::SimuBufferCtrlObj(Simulator& simu)
	: m_simu(simu), 
	  m_buffer_mgr(simu.getBufferMgr())
{
}

SimuBufferCtrlObj::~SimuBufferCtrlObj()
{
}

void SimuBufferCtrlObj::setFrameDim(const FrameDim& frame_dim)
{
	m_buffer_mgr.setFrameDim(frame_dim);
}

void SimuBufferCtrlObj::getFramedim(FrameDim& frame_dim)
{
	m_buffer_mgr.getFrameDim(frame_dim);
}

void SimuBufferCtrlObj::setNbBuffers(int nb_buffers)
{
	m_buffer_mgr.setNbBuffers(nb_buffers);
}

void SimuBufferCtrlObj::getNbBuffers(int& nb_buffers)
{
	m_buffer_mgr.getNbBuffers(nb_buffers);
}

void SimuBufferCtrlObj::setNbConcatFrames(int nb_concat_frames)
{
	m_buffer_mgr.setNbConcatFrames(nb_concat_frames);
}

void SimuBufferCtrlObj::getNbConcatFrames(int& nb_concat_frames)
{
	m_buffer_mgr.getNbConcatFrames(nb_concat_frames);
}

void SimuBufferCtrlObj::setNbAccFrames(int nb_acc_frames)
{
	m_buffer_mgr.setNbAccFrames(nb_acc_frames);
}

void SimuBufferCtrlObj::getNbAccFrames(int& nb_acc_frames)
{
	m_buffer_mgr.getNbAccFrames(nb_acc_frames);
}

void SimuBufferCtrlObj::getMaxNbBuffers(int& max_nb_buffers)
{
	m_buffer_mgr.getMaxNbBuffers(max_nb_buffers);
}

void *SimuBufferCtrlObj::getBufferPtr(int buffer_nb)
{
	return m_buffer_mgr.getBufferPtr(buffer_nb);
}

void *SimuBufferCtrlObj::getFramePtr(int acq_frame_nb)
{
	return m_buffer_mgr.getFramePtr(acq_frame_nb);
}

void SimuBufferCtrlObj::getStartTimestamp(Timestamp& start_ts)
{
	m_buffer_mgr.getStartTimestamp(start_ts);
}

void SimuBufferCtrlObj::getFrameInfo(int acq_frame_nb, HwFrameInfoType& info)
{
	m_buffer_mgr.getFrameInfo(acq_frame_nb, info);
}

void SimuBufferCtrlObj::registerFrameCallback(HwFrameCallback *frame_cb)
{
	m_buffer_mgr.registerFrameCallback(frame_cb);
}

void SimuBufferCtrlObj::unregisterFrameCallback(HwFrameCallback *frame_cb)
{
	m_buffer_mgr.unregisterFrameCallback(frame_cb);
}


/*******************************************************************
 * \brief SimuSyncCtrlObj constructor
 *******************************************************************/

SimuSyncCtrlObj::SimuSyncCtrlObj(Simulator& simu)
	: m_simu(simu)
{
}

SimuSyncCtrlObj::~SimuSyncCtrlObj()
{
}

void SimuSyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
	trig_mode = IntTrig;
}

void SimuSyncCtrlObj::getTrigMode(TrigMode& trig_mode)
{
	if (trig_mode != IntTrig)
		throw LIMA_HW_EXC(InvalidValue, "Invalid (external) trigger");
}

void SimuSyncCtrlObj::setExpTime(double exp_time)
{
	m_simu.setExpTime(exp_time);
}

void SimuSyncCtrlObj::getExpTime(double& exp_time)
{
	m_simu.getExpTime(exp_time);
}

void SimuSyncCtrlObj::setLatTime(double lat_time)
{
	m_simu.setLatTime(lat_time);
}

void SimuSyncCtrlObj::getLatTime(double& lat_time)
{
	m_simu.getLatTime(lat_time);
}

void SimuSyncCtrlObj::setNbFrames(int nb_frames)
{
	m_simu.setNbFrames(nb_frames);
}

void SimuSyncCtrlObj::getNbFrames(int& nb_frames)
{
	m_simu.getNbFrames(nb_frames);
}

void SimuSyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges)
{
	double min_time = 10e-9;;
	double max_time = 1e6;
	valid_ranges.min_exp_time = min_time;
	valid_ranges.max_exp_time = max_time;
	valid_ranges.min_lat_time = min_time;
	valid_ranges.max_lat_time = max_time;
}


/*******************************************************************
 * \brief SimuBinCtrlObj constructor
 *******************************************************************/

SimuBinCtrlObj::SimuBinCtrlObj(Simulator& simu)
	: m_simu(simu)
{
}

SimuBinCtrlObj::~SimuBinCtrlObj()
{
}

void SimuBinCtrlObj::setBin(const Bin& bin)
{
	m_simu.setBin(bin);
}

void SimuBinCtrlObj::getBin(Bin& bin)
{
	m_simu.getBin(bin);
}

void SimuBinCtrlObj::checkBin(Bin& bin)
{
	m_simu.checkBin(bin);
}


/*******************************************************************
 * \brief SimuHwInterface constructor
 *******************************************************************/

SimuHwInterface::SimuHwInterface(Simulator& simu)
	: m_simu(simu), m_det_info(simu), m_buffer(simu), m_sync(simu),
	  m_bin(simu)
{
	HwDetInfoCtrlObj *det_info = &m_det_info;
	m_cap_list.push_back(HwCap(det_info));

	HwBufferCtrlObj *buffer = &m_buffer;
	m_cap_list.push_back(HwCap(buffer));

	HwSyncCtrlObj *sync = &m_sync;
	m_cap_list.push_back(HwCap(sync));

	HwBinCtrlObj *bin = &m_bin;
	m_cap_list.push_back(HwCap(bin));
}

SimuHwInterface::~SimuHwInterface()
{
}

const HwInterface::CapList& SimuHwInterface::getCapList() const
{
	return m_cap_list;
}

void SimuHwInterface::reset(ResetLevel reset_level)
{
	m_simu.reset();
}

void SimuHwInterface::prepareAcq()
{
}

void SimuHwInterface::startAcq()
{
	m_simu.startAcq();
}

void SimuHwInterface::stopAcq()
{
	m_simu.stopAcq();
}

void SimuHwInterface::getStatus(StatusType& status)
{
	Simulator::Status simu_status = m_simu.getStatus();
	switch (simu_status) {
	case Simulator::Ready:
		status.acq = AcqReady;
		status.det = DetIdle;
		break;
	case Simulator::Exposure:
		status.det = Exposure;
		goto Running;
	case Simulator::Readout:
		status.det = Readout;
		goto Running;
	case Simulator::Latency:
		status.det = Latency;
	Running:
		status.acq = AcqRunning;
		break;
	}
	status.det_mask = DetStatus(Exposure | Readout | Latency);
}

int SimuHwInterface::getNbAcquiredFrames()
{
	return m_simu.getNbAcquiredFrames();
}

