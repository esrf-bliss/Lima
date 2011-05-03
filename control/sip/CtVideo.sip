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
#include <CtVideo.h>
using namespace lima;
%End

  public:
    CtVideo(CtControl&);
    ~CtVideo();
    
    struct Parameters
    {
      Parameters();
      void reset();
      
      bool 	live;
      double 	exposure;
      double 	gain;
      VideoMode mode;
      Roi 	roi;
      Bin 	bin;
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
  sipRes = PyByteArray_FromStringAndSize(buffer ? buffer : "",size);
  Py_END_ALLOW_THREADS
%End
      int 			width() 	const;
      int 			height() 	const;
      VideoMode 		mode() 		const;
      int			size()		const;
      int			frameNumber()	const;
      const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	sipRes = str.str().c_str();
%End  
    };

    class ImageCallback
    {
    public:
      ImageCallback();
      virtual ~ImageCallback();

      virtual void newImage(const CtVideo::Image&) = 0;
    };
    // --- parameters
    void setParameters(const Parameters &pars);
    void getParameters(Parameters &pars /Out/) const;

    void setLive(bool liveFlag);
    void getLive(bool &liveFlag /Out/) const;

    void setExposure(double);
    void getExposure(double& /Out/) const;

    void setGain(double aGain);
    void getGain(double &aGain /Out/) const;

    void setMode(VideoMode aMode);
    void getMode(VideoMode &aMode /Out/) const;

    void setRoi(const Roi &aRoi);
    void getRoi(Roi &aRoi /Out/) const;

    void setBin(const Bin &aBin);
    void getBin(Bin &aBin /Out/) const;

    // --- images
    void getLastImage(CtVideo::Image &anImage /Out/) const;
    void getLastImageCounter(int &anImageCounter /Out/) const;

    void registerImageCallback(CtVideo::ImageCallback &cb);
    void unregisterImageCallback(CtVideo::ImageCallback &cb);

    // --- video mode
    void getSupportedVideoMode(std::list<VideoMode> &modeList /Out/);
  };
