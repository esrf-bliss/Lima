#include "XpadInterface.h"
#include <algorithm>

using namespace lima;
//using namespace lima::Xpad;
using namespace std;

/*******************************************************************
 * \brief XpadDetInfoCtrlObj constructor
 *******************************************************************/
XpadDetInfoCtrlObj::XpadDetInfoCtrlObj(XpadCamera& cam):m_cam(cam)
{
    DEB_CONSTRUCTOR();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
XpadDetInfoCtrlObj::~XpadDetInfoCtrlObj()
{
    DEB_DESTRUCTOR();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadDetInfoCtrlObj::getMaxImageSize(Size& size)
{
    DEB_MEMBER_FUNCT();
    m_cam.getImageSize(size);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadDetInfoCtrlObj::getDetectorImageSize(Size& size)
{
    DEB_MEMBER_FUNCT();
    m_cam.getImageSize(size);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadDetInfoCtrlObj::getDefImageType(ImageType& image_type)
{
    DEB_MEMBER_FUNCT();

	m_cam.getPixelDepth(image_type);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadDetInfoCtrlObj::getCurrImageType(ImageType& image_type)
{
    DEB_MEMBER_FUNCT();
	m_cam.getPixelDepth(image_type);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadDetInfoCtrlObj::setCurrImageType(ImageType image_type)
{
    DEB_MEMBER_FUNCT();
    m_cam.setPixelDepth(image_type);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadDetInfoCtrlObj::getPixelSize(double& size)
{
    DEB_MEMBER_FUNCT();
    m_cam.getPixelSize(size);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadDetInfoCtrlObj::getDetectorType(std::string& type)
{
    DEB_MEMBER_FUNCT();
    m_cam.getDetectorType(type);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadDetInfoCtrlObj::getDetectorModel(std::string& model)
{
    DEB_MEMBER_FUNCT();
    m_cam.getDetectorModel(model);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadDetInfoCtrlObj::registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{
    DEB_MEMBER_FUNCT();
    m_cam.registerMaxImageSizeCallback(cb);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadDetInfoCtrlObj::unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{
    DEB_MEMBER_FUNCT();
    m_cam.unregisterMaxImageSizeCallback(cb);
}



/*******************************************************************
 * \brief XpadBufferCtrlObj constructor
 *******************************************************************/
XpadBufferCtrlObj::XpadBufferCtrlObj(XpadCamera& cam)
	: m_buffer_mgr(cam.getBufferMgr())
{
	DEB_CONSTRUCTOR();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
XpadBufferCtrlObj::~XpadBufferCtrlObj()
{
	DEB_DESTRUCTOR();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadBufferCtrlObj::setFrameDim(const FrameDim& frame_dim)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.setFrameDim(frame_dim);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadBufferCtrlObj::getFrameDim(FrameDim& frame_dim)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.getFrameDim(frame_dim);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadBufferCtrlObj::setNbBuffers(int nb_buffers)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.setNbBuffers(nb_buffers);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadBufferCtrlObj::getNbBuffers(int& nb_buffers)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.getNbBuffers(nb_buffers);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadBufferCtrlObj::setNbConcatFrames(int nb_concat_frames)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.setNbConcatFrames(nb_concat_frames);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadBufferCtrlObj::getNbConcatFrames(int& nb_concat_frames)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.getNbConcatFrames(nb_concat_frames);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadBufferCtrlObj::getMaxNbBuffers(int& max_nb_buffers)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.getMaxNbBuffers(max_nb_buffers);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void *XpadBufferCtrlObj::getBufferPtr(int buffer_nb, int concat_frame_nb)
{
	DEB_MEMBER_FUNCT();
	return m_buffer_mgr.getBufferPtr(buffer_nb, concat_frame_nb);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void *XpadBufferCtrlObj::getFramePtr(int acq_frame_nb)
{
	DEB_MEMBER_FUNCT();
	return m_buffer_mgr.getFramePtr(acq_frame_nb);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadBufferCtrlObj::getStartTimestamp(Timestamp& start_ts)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.getStartTimestamp(start_ts);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadBufferCtrlObj::getFrameInfo(int acq_frame_nb, HwFrameInfoType& info)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.getFrameInfo(acq_frame_nb, info);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadBufferCtrlObj::registerFrameCallback(HwFrameCallback& frame_cb)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.registerFrameCallback(frame_cb);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadBufferCtrlObj::unregisterFrameCallback(HwFrameCallback& frame_cb)
{
	DEB_MEMBER_FUNCT();
	m_buffer_mgr.unregisterFrameCallback(frame_cb);
}



/*******************************************************************
 * \brief XpadSyncCtrlObj constructor
 *******************************************************************/
XpadSyncCtrlObj::XpadSyncCtrlObj(XpadCamera& cam, HwBufferCtrlObj& buffer_ctrl)
	: HwSyncCtrlObj(buffer_ctrl), m_cam(cam)
{
}

//-----------------------------------------------------
//
//-----------------------------------------------------
XpadSyncCtrlObj::~XpadSyncCtrlObj()
{
}

//-----------------------------------------------------
//
//-----------------------------------------------------
bool XpadSyncCtrlObj::checkTrigMode(TrigMode trig_mode)
{
	bool valid_mode = false;
	switch (trig_mode)
	{
		case IntTrig:
		case ExtTrigSingle:
			valid_mode = true;
		break;

		default:
			valid_mode = false;
	}
	return valid_mode;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadSyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
	DEB_MEMBER_FUNCT();    
	if (!checkTrigMode(trig_mode))
		THROW_HW_ERROR(InvalidValue) << "Invalid " << DEB_VAR1(trig_mode);
	m_cam.setTrigMode(trig_mode);
	
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadSyncCtrlObj::getTrigMode(TrigMode& trig_mode)
{
	m_cam.getTrigMode(trig_mode);
}

void XpadSyncCtrlObj::setExpTime(double exp_time)
{
	m_cam.setExpTime(exp_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadSyncCtrlObj::getExpTime(double& exp_time)
{
	m_cam.getExpTime(exp_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadSyncCtrlObj::setNbHwFrames(int nb_frames)
{
	m_cam.setNbFrames(nb_frames);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadSyncCtrlObj::getNbHwFrames(int& nb_frames)
{
	m_cam.getNbFrames(nb_frames);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadSyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges)
{
	double min_time = 10e-9;;
	double max_time = 10e6;
	valid_ranges.min_exp_time = min_time;
	valid_ranges.max_exp_time = max_time;
	valid_ranges.min_lat_time = min_time;
	valid_ranges.max_lat_time = max_time;
}


/*******************************************************************
 * \brief Hw Interface constructor
 *******************************************************************/

XpadInterface::XpadInterface(XpadCamera& cam)
	: m_cam(cam),m_det_info(cam), m_buffer(cam),m_sync(cam, m_buffer)
{
	DEB_CONSTRUCTOR();

	HwDetInfoCtrlObj *det_info = &m_det_info;
	m_cap_list.push_back(HwCap(det_info));

	HwBufferCtrlObj *buffer = &m_buffer;
	m_cap_list.push_back(HwCap(buffer));
	
	HwSyncCtrlObj *sync = &m_sync;
	m_cap_list.push_back(HwCap(sync));
}

//-----------------------------------------------------
//
//-----------------------------------------------------
XpadInterface::~XpadInterface()
{
	DEB_DESTRUCTOR();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadInterface::getCapList(HwInterface::CapList &cap_list) const
{
	DEB_MEMBER_FUNCT();
	cap_list = m_cap_list;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadInterface::reset(ResetLevel reset_level)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(reset_level);

	stopAcq();

	Size image_size;
	m_det_info.getMaxImageSize(image_size);
	ImageType image_type;
	m_det_info.getDefImageType(image_type);
	FrameDim frame_dim(image_size, image_type);
	m_buffer.setFrameDim(frame_dim);

	m_buffer.setNbConcatFrames(1);
	m_buffer.setNbBuffers(1);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadInterface::prepareAcq()
{
	DEB_MEMBER_FUNCT();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadInterface::startAcq()
{
	DEB_MEMBER_FUNCT();
	m_cam.start();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadInterface::stopAcq()
{
	DEB_MEMBER_FUNCT();
	m_cam.stop();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadInterface::getStatus(StatusType& status)
{
	XpadCamera::Status xpad_status = XpadCamera::Ready;
	m_cam.getStatus(xpad_status);
	switch (xpad_status)
	{
		case XpadCamera::Ready:
			status.acq = AcqReady;
			status.det = DetIdle;
			break;
		case XpadCamera::Exposure:
			status.det = DetExposure;
			status.acq = AcqRunning;
			break;
		case XpadCamera::Readout:
			status.det = DetReadout;
			status.acq = AcqRunning;
			break;
	}
	status.det_mask = DetExposure | DetReadout;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
int XpadInterface::getNbHwAcquiredFrames()
{
	DEB_MEMBER_FUNCT();
	/*Acq::Status acq_status;
	m_acq.getStatus(acq_status);
	int nb_hw_acq_frames = acq_status.last_frame_nb + 1;
	DEB_RETURN() << DEB_VAR1(nb_hw_acq_frames);
	return nb_hw_acq_frames;*/
	return 0;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadInterface::setFParameters(unsigned deadtime, unsigned init,
			unsigned shutter, unsigned ovf,    unsigned mode,
			unsigned n,       unsigned p,
			unsigned GP1,     unsigned GP2,    unsigned GP3,      unsigned GP4)
{
	DEB_MEMBER_FUNCT();
	m_cam.setFParameters(deadtime,init,shutter,ovf,mode,n,p,GP1,GP2,GP3,GP4);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadInterface::setAcquisitionType(short acq_type)
{
	DEB_MEMBER_FUNCT();
	m_cam.setAcquisitionType(acq_type);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadInterface::loadFlatConfig(unsigned flat_value)
{
	DEB_MEMBER_FUNCT();
	m_cam.loadFlatConfig(flat_value);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadInterface::loadAllConfigG()
{
	DEB_MEMBER_FUNCT();
	m_cam.loadAllConfigG();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void XpadInterface::loadConfigG(unsigned* config_g_and_value)
{
	DEB_MEMBER_FUNCT();
	m_cam.loadConfigG(configg_and_value);
}

