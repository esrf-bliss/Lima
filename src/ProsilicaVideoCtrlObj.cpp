#include "ProsilicaVideoCtrlObj.h"
#include "ProsilicaCamera.h"
#include "ProsilicaSyncCtrlObj.h"

using namespace lima;
using namespace lima::Prosilica;

const tPvUint32 MAX_GAIN = 29;

VideoCtrlObj::VideoCtrlObj(Camera *cam) :
  m_cam(cam),
  m_handle(cam->getHandle()),
  m_live(false)
{
}

VideoCtrlObj::~VideoCtrlObj()
{
}

void VideoCtrlObj::getSupportedVideoMode(std::list<VideoMode> &aList) const
{
  aList.push_back(Y8);
  if(!m_cam->isMonochrome())
    {
      aList.push_back(BAYER_RG8);
      aList.push_back(BAYER_RG16);
    }
  else
    aList.push_back(Y16);
}

void VideoCtrlObj::getVideoMode(VideoMode &aMode) const
{
  aMode = m_cam->getVideoMode();
}

void VideoCtrlObj::setVideoMode(VideoMode aMode)
{
  m_cam->setVideoMode(aMode);
}

void VideoCtrlObj::setLive(bool flag)
{
  m_live = flag;
  if(flag)
    m_sync->startAcq();
  else
    m_sync->stopAcq();
}

void VideoCtrlObj::getLive(bool &flag) const
{
  flag = m_live;
}

void VideoCtrlObj::getGain(double &aGain) const
{
  tPvUint32 localGain;
  tPvErr error=PvAttrUint32Get(m_handle, "GainValue", &localGain);
  aGain = double(localGain / MAX_GAIN);
}

void VideoCtrlObj::setGain(double aGain)
{
  tPvUint32 localGain = tPvUint32(aGain * MAX_GAIN);
  tPvErr error=PvAttrUint32Set(m_handle, "GainValue", localGain);
  if(error)
    throw LIMA_HW_EXC(Error,"Can't set gain to asked value");
}

void VideoCtrlObj::checkBin(Bin& bin)
{
  bin = Bin(1,1);		// Do not manage Hw Bin
}

void VideoCtrlObj::checkRoi(const Roi&, Roi& hw_roi)
{
  tPvUint32 width, height;
  tPvErr error = PvAttrUint32Get(m_handle,"Width",&width);
  error = PvAttrUint32Get(m_handle,"Height",&height);

  hw_roi = Roi(0,0,width,height); // Do not manage Hw Roi
}
