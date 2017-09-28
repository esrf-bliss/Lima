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
class CtControl {
%TypeHeaderCode
#include "lima/CtControl.h"
#include "processlib/LinkTask.h"
using namespace lima;
%End  
  public:

    enum ApplyPolicy {
      All,
      Changes,
      HwSyncChanges,
    };

    struct ImageStatus {
      ImageStatus();
      ImageStatus(long lastImgAcq, long lastBaseImgReady, long lastImgReady,
                  long lastImgSaved, long lastCntReady);
      void reset();

      long	LastImageAcquired;
      long	LastBaseImageReady;
      long	LastImageReady;
      long	LastImageSaved;
      long	LastCounterReady;

      const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	const std::string& tmpString = str.str();
	sipRes = tmpString.c_str();
%End

%PickleCode
      sipRes = Py_BuildValue("lllll", sipCpp->LastImageAcquired,
                             sipCpp->LastBaseImageReady,
                             sipCpp->LastImageReady,
                             sipCpp->LastImageSaved,
                             sipCpp->LastCounterReady);
%End
    };

    class ImageStatusCallback 
    {
    public:
      enum RatePolicy {
	RateAsFastAsPossible,
	RateAllFrames,
      };

      ImageStatusCallback();
      virtual ~ImageStatusCallback();

      void setRatePolicy(CtControl::ImageStatusCallback::RatePolicy policy);
      void getRatePolicy(CtControl::ImageStatusCallback::RatePolicy& policy);
    protected:
      virtual void imageStatusChanged(const CtControl::ImageStatus& status)=0;
    };

    enum ErrorCode {NoError,
		    SaveUnknownError,SaveOpenError,SaveCloseError,
		    SaveAccessError,SaveOverwriteError,SaveDiskFull,SaveOverun,
		    ProcessingOverun,
		    CameraError};
    
    enum CameraErrorCode {NoCameraError}; /* @todo fix this */

    struct Status
    {
      Status();
      Status(AcqStatus acq_status, CtControl::ErrorCode err,
             CtControl::CameraErrorCode cam_err,
             const CtControl::ImageStatus& img_status);
      void reset();

      AcqStatus		         AcquisitionStatus;
      CtControl::ErrorCode       Error;
      CtControl::CameraErrorCode CameraStatus;
      CtControl::ImageStatus	 ImageCounters;

      SIP_PYOBJECT __repr__() const /TypeHint="str"/;
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	const std::string& tmpString = str.str();

	#if PY_MAJOR_VERSION >= 3
		sipRes = PyUnicode_FromString(tmpString.c_str());
	#else
		sipRes = PyString_FromString(tmpString.c_str());
	#endif
%End

%PickleCode
      PyObject* image_status = sipConvertFromType(&(sipCpp->ImageCounters),
                                                  sipType_CtControl_ImageStatus,
                                                  NULL);
      sipRes = Py_BuildValue("iiiO", sipCpp->AcquisitionStatus,
                             sipCpp->Error,
                             sipCpp->CameraStatus,
                             image_status);
      Py_DECREF(image_status);

%End
    };

    CtControl(HwInterface *hw /KeepReference/);
    ~CtControl();

    void prepareAcq();
    void startAcq();
    void stopAcq();
    void abortAcq();

    void abortAcq(AcqStatus acq_status, CtControl::ErrorCode error_code, 
		  Data &data, bool ctrl_mutex_locked=false);

    CtAcquisition* acquisition();
    CtSaving* saving();
%If (WITH_SPS_IMAGE)    
%If (POSIX_PLATFORM)
    CtSpsImage* display();
%End
%End
    CtImage* image();
    CtBuffer* buffer();
    CtShutter* shutter();
    CtAccumulation* accumulation();
    CtVideo* video();
    CtEvent* event();
%If (WITH_CONFIG)
    CtConfig* config();
%End
    SoftOpExternalMgr* externalOperation();
    
    HwInterface* hwInterface();

    void setApplyPolicy(ApplyPolicy policy);
    void getApplyPolicy(ApplyPolicy &policy /Out/) const;

    void getStatus(Status& status /Out/) const;
    void getImageStatus(ImageStatus &imageStatus /Out/) const;

    void ReadImage(Data& data /Out/,long frameNumber = -1, 
				    long readBlockLen = 1);
    void ReadBaseImage(Data& data /Out/,long frameNumber = -1,
					long readBlockLen = 1);

    void reset();
    void resetStatus(bool only_acq_status);

    void registerImageStatusCallback(ImageStatusCallback& cb);
    void unregisterImageStatusCallback(ImageStatusCallback& cb);

    void setReconstructionTask(LinkTask*);

    void setPrepareTimeout(double timeout);
    void getPrepareTimeout(double& timeout /Out/) const;

  protected:
    bool newFrameReady(Data& data);
    void newFrameToSave(Data& data);
    void newBaseImageReady(Data &data);
    void newImageReady(Data &data);
    void newCounterReady(Data &data);
    void newImageSaved(Data &data);
};
