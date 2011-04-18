#include "ProsilicaInterface.h"
#include "ProsilicaCamera.h"
#include "ProsilicaDetInfoCtrlObj.h"
#include "ProsilicaBufferCtrlObj.h"
#include "ProsilicaVideoCtrlObj.h"
#include "ProsilicaSyncCtrlObj.h"

using namespace lima;
using namespace lima::Prosilica;


Interface::Interface(Camera *cam) :
  m_cam(cam)
{
  m_det_info = new DetInfoCtrlObj(cam);
  if(m_cam->isMonochrome())
    {
      m_buffer = new BufferCtrlObj(cam);
      m_video = NULL;
    }
  else
    {
      m_video = new VideoCtrlObj(cam);
      cam->_allocBuffer();
      cam->m_video = m_video;
      m_buffer = NULL;
    }
  m_sync = new SyncCtrlObj(cam,m_video ? NULL : m_buffer);
  cam->m_sync = m_sync;
  m_buffer->m_sync = m_sync;
  if(m_video)
    m_video->m_sync = m_sync;
}

Interface::~Interface()
{
  if(m_video)
    {
      delete m_video;
    }
  else
    delete m_buffer;
  delete m_det_info;
  delete m_sync;
}

void Interface::getCapList(CapList &cap_list) const
{
  cap_list.push_back(HwCap(m_sync));
  cap_list.push_back(HwCap(m_det_info));
  if(m_video)
    { 
      cap_list.push_back(HwCap(m_video));
      cap_list.push_back(HwCap(&(m_video->getHwBufferCtrlObj())));
    }
  else
    cap_list.push_back(HwCap(m_buffer));
}

void Interface::reset(ResetLevel reset_level)
{
  m_sync->stopAcq();
  m_cam->reset();
}

void Interface::prepareAcq()
{
}

void Interface::startAcq()
{
  m_sync->startAcq();
}

void Interface::stopAcq()
{
  m_sync->stopAcq();
}

void Interface::getStatus(StatusType& status)
{
  
}

int Interface::getNbAcquiredFrames()
{
  if(m_buffer)
    return m_buffer->getNbAcquiredFrames();
  else
    return m_cam->getNbAcquiredFrames();
}

int Interface::getNbHwAcquiredFrames()
{
  return getNbAcquiredFrames();
}

