/***************************************************************//**
 * @file   FoclaHwInterface.cpp
 * @brief  This file contains Focla Hardware Interface implementation
 *
 * @author A.Kirov
 * @date   15/07/2009
 *******************************************************************/
#include "FoclaHwInterface.h"

using namespace std;
using namespace lima;
using namespace Espia::Focla;


/***************************************************************//**
 * @brief Espia::Focla::DetInfoCtrlObj class constructor
 *
 * @param[in] focla        reference to Espia::Focla::Dev object
 *******************************************************************/
DetInfoCtrlObj::DetInfoCtrlObj(Dev &focla)
	: m_focla(focla)
{
}


DetInfoCtrlObj::~DetInfoCtrlObj()
{
}


/***************************************************************//**
 * @brief Espia::Focla::BufferCtrlObj class constructor
 *
 * @param[in] buffer_mgr   reference to BufferCtrlMgr object
 *******************************************************************/
BufferCtrlObj::BufferCtrlObj( BufferCtrlMgr &buffer_mgr )
	: m_buffer_mgr(buffer_mgr)
{
}


BufferCtrlObj::~BufferCtrlObj()
{
}


void BufferCtrlObj::setFrameDim(const FrameDim &frame_dim)
{
	m_buffer_mgr.setFrameDim(frame_dim);
}


void BufferCtrlObj::getFramedim(FrameDim &frame_dim)
{
	m_buffer_mgr.getFrameDim(frame_dim);
}


void BufferCtrlObj::setNbBuffers(int nb_buffers)
{
	m_buffer_mgr.setNbBuffers(nb_buffers);
}


void BufferCtrlObj::getNbBuffers(int &nb_buffers)
{
	m_buffer_mgr.getNbBuffers(nb_buffers);
}


void BufferCtrlObj::setNbConcatFrames(int nb_concat_frames)
{
	m_buffer_mgr.setNbConcatFrames(nb_concat_frames);
}


void BufferCtrlObj::getNbConcatFrames(int &nb_concat_frames)
{
	m_buffer_mgr.getNbConcatFrames(nb_concat_frames);
}


void BufferCtrlObj::setNbAccFrames(int nb_acc_frames)
{
	m_buffer_mgr.setNbAccFrames(nb_acc_frames);
}


void BufferCtrlObj::getNbAccFrames(int &nb_acc_frames)
{
	m_buffer_mgr.getNbAccFrames(nb_acc_frames);
}


void BufferCtrlObj::getMaxNbBuffers(int &max_nb_buffers)
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


void BufferCtrlObj::getStartTimestamp(Timestamp &start_ts)
{
	m_buffer_mgr.getStartTimestamp(start_ts);
}


void BufferCtrlObj::getFrameInfo(int acq_frame_nb, HwFrameInfoType &info)
{
	m_buffer_mgr.getFrameInfo(acq_frame_nb, info);
}


void BufferCtrlObj::registerFrameCallback(HwFrameCallback &frame_cb)
{
	m_buffer_mgr.registerFrameCallback(frame_cb);
}


void BufferCtrlObj::unregisterFrameCallback(HwFrameCallback &frame_cb)
{
	m_buffer_mgr.unregisterFrameCallback(frame_cb);
}


/***************************************************************//**
 * @brief Espia::Focla::SyncCtrlObj class constructor
 *
 * @param[in] acq          reference to Espia::Acq object
 * @param[in] focla        reference to Espia::Focla::Dev object
 *******************************************************************/
SyncCtrlObj::SyncCtrlObj( Espia::Acq &acq, Dev &focla, 
			  HwBufferCtrlObj& buffer_ctrl )
	: HwSyncCtrlObj( buffer_ctrl ), m_acq(acq), m_focla(focla)
{
}


SyncCtrlObj::~SyncCtrlObj()
{
}


void SyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
}


void SyncCtrlObj::getTrigMode(TrigMode &trig_mode)
{
}


void SyncCtrlObj::setExpTime(double exp_time)
{
}


void SyncCtrlObj::getExpTime(double &exp_time)
{
}


void SyncCtrlObj::setLatTime(double lat_time)
{
}


void SyncCtrlObj::getLatTime(double &lat_time)
{
}


void SyncCtrlObj::setNbHwFrames(int nb_frames)
{
	m_acq.setNbFrames(nb_frames);
}


void SyncCtrlObj::getNbHwFrames(int &nb_frames)
{
	m_acq.getNbFrames(nb_frames);
}


void SyncCtrlObj::getValidRanges(ValidRangesType &valid_ranges)
{
}


/***************************************************************//**
 * @brief Espia::Focla::Interface class constructor
 *
 * @param[in] acq          reference to Espia::Acq object
 * @param[in] buffer_mgr   reference to BufferCtrlMgr object
 * @param[in] focla        reference to Espia::Focla::Dev object
 *******************************************************************/
Interface::Interface( Espia::Acq &acq, BufferCtrlMgr &buffer_mgr, 
                      Espia::Focla::Dev &focla )
	: m_acq(acq), m_buffer_mgr(buffer_mgr), m_focla(focla),
	  /*m_det_info(focla),*/ m_buffer(buffer_mgr), 
	  m_sync(acq, focla, m_buffer)
{
/*	HwDetInfoCtrlObj *det_info = &m_det_info;
	m_cap_list.push_back(HwCap(det_info)); */

	HwBufferCtrlObj *buffer = &m_buffer;
	m_cap_list.push_back(HwCap(buffer));

	HwSyncCtrlObj *sync = &m_sync;
	m_cap_list.push_back(HwCap(sync));

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

	if( reset_level == HardReset )  // ???
		m_focla.setParam( TEST_IMAGE, 0 );

	m_sync.setNbFrames(1);

	m_buffer.setNbConcatFrames(1);
	m_buffer.setNbAccFrames(1);
//	m_buffer.setNbBuffers(1);  // We need to set the FrameDim before this!
}


void Interface::prepareAcq()
{
}


void Interface::startAcq()
{
	m_acq.start();
	m_focla.setParam( TEST_IMAGE, 1 );
}


void Interface::stopAcq()
{
	m_focla.setParam( TEST_IMAGE, 0 );
	m_acq.stop();
}


void Interface::getStatus(StatusType& status)
{
	Acq::Status acq;
	m_acq.getStatus(acq);
	status.acq = acq.running ? AcqRunning : AcqReady;

	static const DetStatus det_mask = DetIdle;  // ???
	status.det_mask = det_mask;
}


int Interface::getNbHwAcquiredFrames()
{
	Acq::Status acq_status;
	m_acq.getStatus(acq_status);
	return acq_status.last_frame_nb + 1;
}
