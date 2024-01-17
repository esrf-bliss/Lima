//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2015
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

#include "lima/LimaCompatibility.h"
#include <list>
#include <deque>

#include "lima/CtControl.h"
#include "lima/CtConfig.h"
#include "processlib/SinkTaskMgr.h"

namespace lima
{
  
  /// Control image accumulation settings
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

    enum Filter { FILTER_NONE, FILTER_THRESHOLD_MIN, FILTER_OFFSET_THEN_THRESHOLD_MIN };    ///< Filter pixels that contribute to the accumulation
    enum Operation { ACC_SUM, ACC_MEAN, ACC_MEDIAN };                                       ///< Type of accumulation

    static const long ACC_MIN_BUFFER_SIZE = 64L;

    struct LIMACORE_API Parameters
    {
      DEB_CLASS_NAMESPC(DebModControl,"Accumulation::Parameters","Control");
    public:
      enum Mode {STANDARD, THRESHOLD_BEFORE, OFFSET_THEN_THRESHOLD_BEFORE};

      Parameters();
      void reset();
      
      bool      active;	             ///< if true do the calculation
      long long pixelThresholdValue; ///< value which determine the threshold of the calculation
      ImageType pixelOutputType;     ///< pixel ouptut type (after casting)

      bool        savingFlag;     ///< saving flag if true save saturatedImageCounter
      std::string savePrefix;     ///< prefix filename of saturatedImageCounter (default is saturated_image_counter)
      Mode        mode;
      Filter      filter;         ///< Filter pixels that contribute to the accumulation
      Operation   operation;      ///< Type of accumulation
      long long   thresholdB4Acc; ///< value used in mode THRESHOLD_BEFORE
      long long   offsetB4Acc;    ///< value used in OFFSET_THEN_THRESHOLD_BEFORE
    };
    
    class ThresholdCallback
    {
      DEB_CLASS_NAMESPC(DebModControl,"Accumulation::ThresholdCallback", 
			"Control");
      friend class CtAccumulation;
    public:
      ThresholdCallback() : m_max(0) {};
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

    void setOutputType(ImageType pixelOutputType);
    void getOutputType(ImageType& pixelOutputType) const;

    void getBufferSize(int &aBufferSize) const;

    void setSavingFlag(bool savingFlag);
    void getSavingFlag(bool &savingFlag) const;

    void setSavePrefix(const std::string &savePrefix);
    void getSavePrefix(std::string &savePrefix) const;

    void getMode(Parameters::Mode& mode) const;
    void setMode(Parameters::Mode mode);

    void getFilter(Filter& filter) const;
    void setFilter(Filter filter);

    void getOperation(Operation& acc) const;
    void setOperation(Operation acc);

    void getThresholdBefore(long long&) const;
    void setThresholdBefore(const long long&);

    void getOffsetBefore(long long&) const;
    void setOffsetBefore(const long long&);

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
    long				    m_buffers_size;
    Data                    m_tmp_data;  // Temporary data where frames are accumulated before copied to output data
    std::vector<Data>       m_tmp_datas; // Temporary data where frames are stored to compute the median
    std::deque<Data>        m_datas;     // Circular buffer of output data (used with getFrame())
    std::deque<Data>        m_saturated_images;
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
    bool				m_stopped;

    // --- Methodes for acquisition
    void clear();
    void prepare();
    bool _newFrameReady(Data&);
    bool _newBaseFrameReady(Data&);
    void stop();

    void getFrame(Data &,int frameNumber);

    void _accFrame(Data& src, Data& dst) const;

    void _calcSaturatedImageNCounters(Data &src,Data &dst);

    inline void _callIfNeedThresholdCallback(Data &aData,long long value);
    
#ifdef WITH_CONFIG
    class _ConfigHandler;
    CtConfig::ModuleTypeCallback* _getConfigHandler();
#endif //WITH_CONFIG

  };

  inline std::ostream& operator<<(std::ostream &os,
				  const CtAccumulation::Parameters& params)
    {
      os << "<"
	 << "active=" << (params.active ? "Yes" : "No") << ", "
	 << "pixelThresholdValue=" << params.pixelThresholdValue << ", "
	 << "savingFlag=" << (params.savingFlag ? "Yes" : "No") << ", "
	 << "savingPrefix=" << params.savePrefix
	 << ">";
      return os;
    }
}
#endif
