#include "SimuHwInterface.h"

using namespace lima;

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
	int max_dim = 8 * 1024;
	max_image_size = Size(max_dim, max_dim);
}

void SimuDetInfoCtrlObj::getDetectorImageSize(Size& det_image_size)
{
	FrameDim fdim;
	m_simu.getFrameDim(fdim);
	det_image_size = fdim.getSize();
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

void SimuDetInfoCtrlObj::getDetectorType(std::string& det_type)
{
	det_type = "Simulator";
}

void SimuDetInfoCtrlObj::getDetectorModel(std::string& det_model)
{
	det_model = "PeakGenerator";
}

void SimuDetInfoCtrlObj::setMaxImageSizeCallbackActive(bool cb_active)
{
	m_iscb_act = cb_active;
}



SimuHwInterface::SimuHwInterface(Simulator& simu)
	: m_simu(simu), m_det_info(simu)
{
	HwCap cap = HwCap(HwCap::DetInfo, &m_det_info);
	m_cap_list.push_back(cap);


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


}

void SimuHwInterface::prepareAcq()
{

}

void SimuHwInterface::startAcq()
{
	BufferCtrlMgr& buffer_mgr = m_simu.getBufferMgr();
	buffer_mgr.setStartTimestamp(Timestamp::now());
}

void SimuHwInterface::stopAcq()
{

}

void SimuHwInterface::getStatus(StatusType& status)
{

}

int SimuHwInterface::getNbAcquiredFrames()
{
	return m_simu.getNbAcquiredFrames();
}

