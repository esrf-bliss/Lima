#include <list>
#include "CtVideo.h"
using namespace lima;


CtVideo::CtVideo(CtControl&)
{
}

CtVideo::~CtVideo()
{
}

// --- parameters
void CtVideo::setParameters(const Parameters &pars)
{
  m_pars = pars;
}
void CtVideo::getParameters(Parameters &pars) const
{
  pars = m_pars;
}

void CtVideo::setLive(bool liveFlag)
{
  m_pars.live = liveFlag;
}
void CtVideo::getLive(bool &liveFlag) const
{
  liveFlag = m_pars.live;
}

void CtVideo::setFrameRate(double aFrameRate)
{
  m_pars.framerate = aFrameRate;
}
void CtVideo::getFrameRate(double &aFrameRate) const
{
  aFrameRate = m_pars.framerate;
}

void CtVideo::setBrightness(double aBrightness)
{
  m_pars.brightness = aBrightness;
}
void CtVideo::getBrightness(double &aBrightness) const
{
  aBrightness = m_pars.brightness;
}

void CtVideo::setGain(double aGain)
{
  m_pars.gain = aGain;
}
void CtVideo::getGain(double &aGain) const
{
  aGain = m_pars.gain;
}

void CtVideo::setMode(VideoMode aMode)
{
  m_pars.mode = aMode;
}
void CtVideo::getMode(VideoMode &aMode) const
{
  aMode = m_pars.mode;
}

void CtVideo::setRoi(const Roi &aRoi)
{
  m_pars.roi = aRoi;
}
void CtVideo::getRoi(Roi &aRoi) const
{
  aRoi = m_pars.roi;
}

void CtVideo::setBin(const Bin &aBin)
{
}
void CtVideo::getBin(Bin &aBin) const
{
}

// --- images
void CtVideo::getLastImage(Image &anImage) const
{
}
void CtVideo::getLastImageCounter(int &anImageCounter) const
{
}

void CtVideo::registerImageCallback(ImageCallback &cb)
{
}
void CtVideo::unregisterImageCallback(ImageCallback &cb)
{
}

// --- video mode
void CtVideo::getSupportedVideoMode(std::list<VideoMode> &modeList)
{
}
//============================================================================
//			 CtVideo::Parameters
//============================================================================
CtVideo::Parameters::Parameters()
{
  reset();
}
void CtVideo::Parameters::reset()
{
  live = false;
  framerate = -1.;
  exposure = 1;
  brightness = .5;
  gain = .5;
  mode = UNDEF;
  roi.reset();
  bin.reset();
}
