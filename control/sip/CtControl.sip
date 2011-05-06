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
#include <CtControl.h>
#include <LinkTask.h>
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
	sipRes = str.str().c_str();
%End
    };

    class ImageStatusCallback 
    {
    public:
      ImageStatusCallback();
      virtual ~ImageStatusCallback();
    protected:
      virtual void imageStatusChanged(const CtControl::ImageStatus& status)=0;
    };

    enum ErrorCode {NoError,
		    SaveAccessError,SaveDiskFull,SaveOverun,
		    ProcessingOverun,
		    CameraError};
    
    enum CameraErrorCode {NoCameraError}; /* @todo fix this */

    struct Status
    {
      Status();
      void reset();

      AcqStatus		         AcquisitionStatus;
      CtControl::ErrorCode       Error;
      CtControl::CameraErrorCode CameraStatus;
      CtControl::ImageStatus	 ImageCounters;

      const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	sipRes = str.str().c_str();
%End
    };

    CtControl(HwInterface *hw);
    ~CtControl();

    void prepareAcq();
    void startAcq();
    void stopAcq();

    CtAcquisition* acquisition();
    CtSaving* saving();
%If (POSIX_PLATFORM)
    CtSpsImage* display();
%End
    CtImage* image();
    CtBuffer* buffer();
    CtShutter* shutter();
    CtAccumulation* accumulation();

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

  protected:
    bool newFrameReady(Data& data);
    void newFrameToSave(Data& data);
    void newBaseImageReady(Data &data);
    void newImageReady(Data &data);
    void newCounterReady(Data &data);
    void newImageSaved(Data &data);
};
