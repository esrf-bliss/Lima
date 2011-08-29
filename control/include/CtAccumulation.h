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
#ifndef CTACCUMULATION_H
#define CTACCUMULATION_H

#include "LimaCompatibility.h"
#include <list>
#include <deque>

#include "CtControl.h"
#include "SinkTaskMgr.h"

namespace lima
{
  class LIMACORE_API CtAccumulation
  {
    DEB_CLASS_NAMESPC(DebModControl,"Accumulation","Control");
    class _ImageReady4AccCallback;
  public:
    friend class CtControl;
    friend class CtBuffer;
    friend class CtBufferFrameCB;
    friend class _ImageReady4AccCallback;

    typedef std::list<std::list<long long> > saturatedCounterResult;

    struct LIMACORE_API Parameters
    {
      DEB_CLASS_NAMESPC(DebModControl,"Accumulation::Parameters","Control");
    public:
      Parameters();
      void reset();
      
      bool		active;	///< if true do the calculation
      long long		pixelThresholdValue; ///< value which determine the threshold of the calculation

      bool	  	savingFlag; ///< saving flag if true save saturatedImageCounter
      std::string 	savePrefix; ///< prefix filename of saturatedImageCounter (default is saturated_image_counter)
    };
    
    class ThresholdCallback
    {
      DEB_CLASS_NAMESPC(DebModControl,"Accumulation::ThresholdCallback", 
			"Control");
      friend class CtAccumulation;
    public:
      ThresholdCallback() {};
      virtual ~ThresholdCallback() {};

      int m_max;

    protected:
      virtual void aboveMax(Data&,long long value) = 0;
    };
    CtAccumulation(CtControl&);
    ~CtAccumulation();

    // --- accumulation and concatenation parameters

    void setParameters(const Parameters &pars);
    void getParameters(Parameters& pars) const;
    
    void setActive(bool activeFlag);
    void getActive(bool &activeFlag);

    void setPixelThresholdValue(long long pixelThresholdValue);
    void getPixelThresholdValue(long long &pixelThresholdValue) const;

    void getBufferSize(int &aBufferSize) const;

    void setSavingFlag(bool savingFlag);
    void getSavingFlag(bool &savingFlag) const;

    void setSavePrefix(const std::string &savePrefix);
    void getSavePrefix(std::string &savePrefix) const;

    // --- variable and data result of Concatenation or Accumulation

    void readSaturatedImageCounter(Data&,long frameNumber = -1);
    void readSaturatedSumCounter(saturatedCounterResult &result,int from = -1);

    // --- Mask image to calculate sum counter
    void setMask(Data&);


    // --- Callback to monitor detector saturation

    void registerThresholdCallback(ThresholdCallback &cb);
    void unregisterThresholdCallback(ThresholdCallback &cb);
  private:
    struct _CounterResult;
    typedef SinkTaskMgr<_CounterResult> _CalcSaturatedTaskMgr;
    struct _CounterResult
    {
      _CounterResult() : value(-1),frameNumber(-1),errorCode(_CalcSaturatedTaskMgr::OK) {}
      explicit _CounterResult(int aFameNumber) :
	value(0),frameNumber(aFameNumber),errorCode(_CalcSaturatedTaskMgr::OK) {}
      explicit _CounterResult(_CalcSaturatedTaskMgr::ErrorCode anErrorCode) :
	value(-1),frameNumber(-1),errorCode(anErrorCode) {}

      long long 			value;
      int 				frameNumber;
      _CalcSaturatedTaskMgr::ErrorCode 	errorCode;
    };

    class _CalcSaturatedTask;
    friend class _CalcSaturatedTask;
    class _CalcEndCBK;
    friend class _CalcEndCBK;
    class _ImageReady4AccCallback : public TaskEventCallback
    {
    public:
      _ImageReady4AccCallback(CtAccumulation &);
      virtual void finished(Data &aData);
    private:
      CtAccumulation &m_acc;
    };

    Parameters 				m_pars;
    long				m_buffers_size;
    std::deque<Data> 			m_datas;
    std::deque<Data> 			m_saturated_images;
    CtControl& 				m_ct;
    bool				m_calc_ready;
    std::deque<std::pair<Data,Data> >	m_calc_pending_data;
    TaskEventCallback* 			m_calc_end;
    _CalcSaturatedTaskMgr*		m_calc_mgr;
    Data				m_calc_mask;
    mutable Cond 			m_cond;
    ThresholdCallback*			m_threshold_cb;
    int 				m_last_acc_frame_nb;
    bool 				m_last_continue_flag;

    // --- Methodes for acquisition
    void prepare();
    bool _newFrameReady(Data&);
    bool _newBaseFrameReady(Data&);

    void getFrame(Data &,int frameNumber);

    void _accFrame(Data &src,Data &dst);
    void _calcSaturatedImageNCounters(Data &src,Data &dst);

    inline void _callIfNeedThresholdCallback(Data &aData,long long value);
  };
}
#endif
