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
#ifndef CTCONTROL_H
#define CTCONTROL_H

#include <set>

#include "LimaCompatibility.h"
#include "ThreadUtils.h"

#include "HwInterface.h"

#include "Data.h"
#include "LinkTask.h"


namespace lima 
{

  class CtAcquisition;
  class CtImage;
  class CtBuffer;
  class CtBufferFrameCB;
  class CtSaving;
#ifdef WITH_SPS_IMAGE
  class CtSpsImage;
#endif
  class CtShutter;
  class CtAccumulation;
  class CtVideo;
  class CtEvent;
#ifdef WITH_CONFIG
  class CtConfig;
#endif
  class SoftOpInternalMgr;
  class SoftOpExternalMgr;
  /** @brief Main client class.
   *
   * With this class you have access to all LImA advance feature:
   * - Saving (CtSaving)
   * - Image control (CtImage)
   * - Acquisition control (CtAcquisition)
   * - Accumulation (CtAccumulation)
   * - Video (CtVideo)
   * - Software operation (SoftOpExternalMgr)
  */
  class LIMACORE_API CtControl {
    DEB_CLASS_NAMESPC(DebModControl,"Control","Control");

    friend class CtBufferFrameCB;
    friend class CtAccumulation;
    friend class CtSaving;	// just to set saving error in stat
    class _ReconstructionChangeCallback;
  public:

    enum ApplyPolicy {
      All,
      Changes,
      HwSyncChanges,
    };


    struct LIMACORE_API ImageStatus {
      DEB_CLASS_NAMESPC(DebModControl,"Control::ImageStatus","Control");
    public:
      ImageStatus();
      void reset();

      long	LastImageAcquired;
      long	LastBaseImageReady;
      long	LastImageReady;
      long	LastImageSaved;
      long	LastCounterReady;
    };


    class LIMACORE_API ImageStatusCallback 
    {
      DEB_CLASS_NAMESPC(DebModControl,"Control::ImageStatusCallback", 
			"Control");
    public:
      enum {
	RateAsFastAsPossible,
	RateAllFrames,
      };

      ImageStatusCallback();
      virtual ~ImageStatusCallback();

      void setRatePolicy(int rate_policy);
      void getRatePolicy(int& rate_policy);
    protected:
      virtual void imageStatusChanged(const ImageStatus& img_status) = 0;
    private:
      friend class CtControl;
      void setImageStatusCallbackGen(CtControl *cb_gen);
      CtControl *m_cb_gen;
      int m_rate_policy;
    };

    enum ErrorCode {NoError,
		    SaveUnknownError,SaveOpenError,SaveCloseError,
		    SaveAccessError,SaveOverwriteError,SaveDiskFull,SaveOverun,
		    ProcessingOverun,
		    CameraError,
		    EventOther}; /* @todo convert to typedef Event::Code */
    
    enum CameraErrorCode {NoCameraError}; /* @todo fix this */

    struct LIMACORE_API Status
    {
      DEB_CLASS_NAMESPC(DebModControl,"Control::Status","Control");
    public:
      Status();
      void reset();

      AcqStatus		AcquisitionStatus;
      ErrorCode		Error;
      CameraErrorCode	CameraStatus;
      ImageStatus	ImageCounters;
    };

    CtControl(HwInterface *hw);
    ~CtControl();

    void prepareAcq();
    void startAcq();
    void stopAcq();

    void abortAcq(AcqStatus acq_status, ErrorCode error_code, Data &data,
		  bool ctrl_mutex_locked=false);

#ifdef WIN32
    CtAcquisition* 	acquisition();
    CtSaving* 		saving();
#ifdef WITH_SPS_IMAGE
    CtSpsImage* 	display();
#endif
    CtImage* 		image();
    CtBuffer* 		buffer();
    CtAccumulation* 	accumulation();
    CtVideo*		video();
    CtShutter* 		shutter();
    CtEvent*		event();
#ifdef WITH_CONFIG
    CtConfig*		config();
#endif
    SoftOpExternalMgr* 	externalOperation();

    HwInterface* 	hwInterface();
#else //unix
    CtAcquisition* 	acquisition() 		{ return m_ct_acq; }
    CtSaving* 		saving() 		{ return m_ct_saving; }
#ifdef WITH_SPS_IMAGE
    CtSpsImage* 	display() 		{ return m_ct_sps_image; }
#endif
    CtImage* 		image() 		{ return m_ct_image; }
    CtBuffer* 		buffer() 		{ return m_ct_buffer; }
    CtAccumulation* 	accumulation() 		{ return m_ct_accumulation; }
    CtVideo*		video()			{ return m_ct_video;}
    CtShutter* 		shutter() 		{ return m_ct_shutter; }
    CtEvent* 		event() 		{ return m_ct_event; }
#ifdef WITH_CONFIG
    CtConfig*		config()		{ return m_ct_config; }
#endif

    SoftOpExternalMgr* 	externalOperation() 	{return m_op_ext;}

    HwInterface* 	hwInterface() 		{return m_hw;}

#endif

    void setApplyPolicy(ApplyPolicy policy);
    void getApplyPolicy(ApplyPolicy &policy) const;

    void getStatus(Status& status) const; // from HW
    void getImageStatus(ImageStatus& status) const;

    void ReadImage(Data&,long frameNumber = -1, long readBlockLen = 1);
    void ReadBaseImage(Data&,long frameNumber = -1, long readBlockLen = 1);

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

  private:
    struct ltData
    {
      inline bool operator()(const Data& d1, const Data& d2) const
      {
	return d1.frameNumber < d2.frameNumber;
      }
    };
    class SoftOpErrorHandler;

    class _LastBaseImageReadyCallback;
    friend class _LastBaseImageReadyCallback;
    class _LastImageReadyCallback;
    friend class _LastImageReadyCallback;
    class _LastImageSavedCallback;
    friend class _LastImageSavedCallback;
    class _LastCounterReadyCallback;
    friend class _LastCounterReadyCallback;

    class _AbortAcqCallback;
    friend class _AbortAcqCallback;

    class ImageStatusThread;

    HwInterface		*m_hw;
    mutable Cond	m_cond;
    mutable Status      m_status;
    
    CtSaving		*m_ct_saving;
#ifdef WITH_SPS_IMAGE
    CtSpsImage		*m_ct_sps_image;
#endif
    CtAcquisition	*m_ct_acq;
    CtImage		*m_ct_image;
    CtBuffer		*m_ct_buffer;
    CtShutter		*m_ct_shutter;
    CtAccumulation	*m_ct_accumulation;
    CtVideo		*m_ct_video;
    CtEvent		*m_ct_event;
#ifdef WITH_CONFIG
    CtConfig		*m_ct_config;
#endif

    SoftOpInternalMgr   *m_op_int;
    SoftOpExternalMgr	*m_op_ext;

    bool		m_op_int_active;
    bool		m_op_ext_link_task_active;
    bool		m_op_ext_sink_task_active;

    std::set<Data,ltData> m_base_images_ready;
    std::set<Data,ltData> m_images_ready;

    std::map<int,Data>    m_images_buffer;
    int			  m_images_buffer_size;

    ApplyPolicy		m_policy;
    bool		m_ready;
    bool		m_autosave;
    bool		m_running;
#ifdef WITH_SPS_IMAGE
    bool		m_display_active_flag;
#endif
    ImageStatusThread   *m_img_status_thread;
    SoftOpErrorHandler* m_soft_op_error_handler;
    _ReconstructionChangeCallback* m_reconstruction_cbk;

    inline bool _checkOverrun(Data&);
    inline void _calcAcqStatus();

    void readBlock(Data&, long frameNumber, long readBlockLen,
		   bool baseImage);
    void readOneImageBuffer(Data&, long frameNumber, long readBlockLen,
			    bool baseImage);
  };

  inline std::ostream& operator<<(std::ostream &os,
				  const CtControl::ImageStatus &status)
  {
    os << "<"
       << "LastImageAcquired=" << status.LastImageAcquired << ", "
       << "LastBaseImageReady=" << status.LastBaseImageReady << ", "
       << "LastImageReady=" << status.LastImageReady << ", "
       << "LastImageSaved=" << status.LastImageSaved << ", "
       << "LastCounterReady=" << status.LastCounterReady
       << ">";
    return os;
  }

  
  inline std::ostream& operator<<(std::ostream &os,
				  const CtControl::ErrorCode &err_code)
  {
    const char *desc = "Unknown";
    switch (err_code)
    {
    case CtControl::NoError:           desc = "No error"; break;
    case CtControl::SaveUnknownError:  desc = "Saving  error"; break;
    case CtControl::SaveOpenError:     desc = "Save file open error"; break;
    case CtControl::SaveCloseError:    desc = "Save file close error"; break;
    case CtControl::SaveAccessError:   desc = "Save access error"; break;
    case CtControl::SaveOverwriteError: desc = "Save overwrite error"; break;
    case CtControl::SaveDiskFull:      desc = "Save disk full"; break;
    case CtControl::SaveOverun:        desc = "Save overrun"; break;
    case CtControl::ProcessingOverun:  desc = "Soft Processing overrun"; break;
      // should read CameraStatus instead @todo fix me
    case CtControl::CameraError:       desc = "Camera Error"; break;
    case CtControl::EventOther:        desc = "Other enexpected event"; break;
    }
    return os << desc;
  }
    
  inline std::ostream& operator<<(std::ostream &os,
				  const CtControl::Status &status)
  {
    os << "<";
    os << "AcquisitionStatus=" << status.AcquisitionStatus;
    if(status.AcquisitionStatus == AcqFault)
	os << ", Error=" << status.Error;
    os << ", ImageCounters=" << status.ImageCounters;
    return os;
  }

  inline bool operator <(const CtControl::ImageStatus& a, 
			 const CtControl::ImageStatus& b)
  {
    return ((a.LastImageAcquired < b.LastImageAcquired) ||
	    (a.LastBaseImageReady < b.LastBaseImageReady) ||
	    (a.LastImageReady < b.LastImageReady) ||
	    (a.LastImageSaved < b.LastImageSaved) ||
	    (a.LastCounterReady < b.LastCounterReady));
  }

  inline bool operator ==(const CtControl::ImageStatus& a, 
			 const CtControl::ImageStatus& b)
  {
    return ((a.LastImageAcquired == b.LastImageAcquired) &&
	    (a.LastBaseImageReady == b.LastBaseImageReady) &&
	    (a.LastImageReady == b.LastImageReady) &&
	    (a.LastImageSaved == b.LastImageSaved) &&
	    (a.LastCounterReady == b.LastCounterReady));
  }

  inline bool operator <=(const CtControl::ImageStatus& a, 
			  const CtControl::ImageStatus& b)
  {
    return (a < b) || (a == b);
  }

  inline bool operator >(const CtControl::ImageStatus& a, 
			 const CtControl::ImageStatus& b)
  {
    return !(a <= b);
  }

  inline bool operator >=(const CtControl::ImageStatus& a, 
			  const CtControl::ImageStatus& b)
  {
    return !(a < b);
  }

} // namespace lima

#endif // CTCONTROL_H
