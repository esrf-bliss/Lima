//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE 
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
  class CtVideo
  {
%TypeHeaderCode
#include "lima/CtVideo.h"
using namespace lima;
%End

  public:
    enum AutoGainMode {
      OFF,	  ///< Always off
      ON,	  ///< Always on
      ON_LIVE	  ///< ON during live and OFF for standard Acquisition
    };
    typedef std::vector<AutoGainMode> AutoGainModeList;

    enum VideoSource {
      BASE_IMAGE,		///< video image source is BaseImage
      LAST_IMAGE		///< video image source is the last image (after all processing)
    };

    CtVideo(CtControl&);
    ~CtVideo();
    
    struct Parameters
    {
      Parameters();
      void reset();
      
      bool			live;
      double			exposure;	///< exposure time in second
      double			gain;		///< % of gain (0. <= gain <= 1.)
      CtVideo::AutoGainMode	auto_gain_mode;
      CtVideo::VideoSource	video_source;
      VideoMode			mode;
      Roi			roi;
      Bin			bin;
      SIP_PYOBJECT __repr__() const;
%MethodCode
      LIMA_REPR_CODE
%End
	};

    class Image
    {
    public:
      Image();
      ~Image();
      Image(const CtVideo::Image&);

      SIP_PYOBJECT		buffer() 	const;
%MethodCode
  Py_BEGIN_ALLOW_THREADS
  const char* buffer = sipCpp->buffer();
  Py_ssize_t size = sipCpp->size();
  if (buffer)
     sipRes = PyBytes_FromStringAndSize(buffer, size);
  else
     sipRes = Py_None;
  Py_END_ALLOW_THREADS
%End
      int 			width() 	const;
      int 			height() 	const;
      VideoMode 		mode() 		const;
      int			size()		const;
      long long			frameNumber()	const;
      SIP_PYOBJECT __repr__() const;
%MethodCode
      LIMA_REPR_CODE
%End  
    };

    class ImageCallback
    {
    public:
      ImageCallback();
      virtual ~ImageCallback();

      virtual void newImage(const CtVideo::Image&) = 0;
    };
    void setActive(bool aFlag);
    bool isActive() const;

    // --- parameters
    void setParameters(const Parameters &pars);
    void getParameters(Parameters &pars /Out/) const;

    void startLive();
    void stopLive();
    void getLive(bool &liveFlag /Out/) const;

    void setExposure(double);
    void getExposure(double& /Out/) const;

    void setGain(double aGain);
    void getGain(double &aGain /Out/) const;
    bool checkAutoGainMode(AutoGainMode mode) const;
    void getAutoGainModeList(AutoGainModeList& modes /Out/) const;
    void setAutoGainMode(AutoGainMode mode);
    void getAutoGainMode(AutoGainMode& mode /Out/) const;

    void setVideoSource(VideoSource source);
    void getVideoSource(VideoSource& source /Out/) const;

    void setMode(VideoMode aMode);
    void getMode(VideoMode &aMode /Out/) const;

    void setRoi(const Roi &aRoi);
    void getRoi(Roi &aRoi /Out/) const;

    void setBin(const Bin &aBin);
    void getBin(Bin &aBin /Out/) const;

    // --- images
    void getLastImage(CtVideo::Image &anImage /Out/) const;
    void getLastImageCounter(long long &anImageCounter /Out/) const;

    void registerImageCallback(CtVideo::ImageCallback &cb);
    void unregisterImageCallback(CtVideo::ImageCallback &cb);

    // --- video mode
    void getSupportedVideoMode(std::list<VideoMode> &modeList /Out/) const;
    
  private:
    CtVideo(const CtVideo&);
  };

%MappedType CtVideo::AutoGainModeList
{
%TypeHeaderCode
#include <vector>
#include "lima/CtVideo.h"
using namespace lima;
%End

%ConvertToTypeCode
  if(sipIsErr == NULL)
    {
      bool aReturnFlag = !!PySequence_Check(sipPy);
      for(int i = 0;aReturnFlag && i < PySequence_Size(sipPy);++i)
	aReturnFlag = !!sipCanConvertToType(PySequence_Fast_GET_ITEM(sipPy,i),
					  sipType_CtVideo_AutoGainMode, SIP_NOT_NONE);
      return aReturnFlag;
    }
  std::vector<CtVideo::AutoGainMode> *modeListPt = new std::vector<CtVideo::AutoGainMode>();
  for(int i = 0;i < PySequence_Size(sipPy);++i)
    modeListPt->push_back(static_cast<CtVideo::AutoGainMode>(sipLong_AsLong(PySequence_Fast_GET_ITEM(sipPy,i))));
  *sipCppPtr = modeListPt;
  return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
  PyObject *l;
  if(!(l = PyList_New(sipCpp->size())))
    return NULL;

  int listPos = 0;
  for(std::vector<CtVideo::AutoGainMode>::iterator i = sipCpp->begin();
      i != sipCpp->end();++i,++listPos)
    {
      PyObject *tobj = sipConvertFromEnum(*i, sipType_CtVideo_AutoGainMode);
      if(!tobj)
        {
	  Py_DECREF(l);
	  return NULL;
        }
      PyList_SET_ITEM(l, listPos, tobj);
    }
  return l;
%End
};
