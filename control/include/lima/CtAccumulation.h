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

#include "lima/BufferHelper.h"
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

    void getMaxNbBuffers(int &max_nb_buffers);

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

    void setBufferParameters(const BufferHelper::Parameters &pars);
    void getBufferParameters(BufferHelper::Parameters& pars) const;

    void setHwNbBuffers(int hw_nb_buffers);
    void getHwNbBuffers(int& hw_nb_buffers) const;
    
    // --- variable and data result of Concatenation or Accumulation

    void readSaturatedImageCounter(Data&,long frameNumber = -1);
    void readSaturatedSumCounter(saturatedCounterResult &result,int from = -1);

    // --- Mask image to calculate sum counter
    void setMask(Data&);


    // --- Callback to monitor detector saturation

    void registerThresholdCallback(ThresholdCallback &cb);
    void unregisterThresholdCallback(ThresholdCallback &cb);

  private:
    static const int ACC_DEF_HW_BUFFERS = 64;
    static const int ACC_MIN_HW_BUFFERS = 16;
    static const int ACC_MAX_PARALLEL_PROC = 32;

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

    class _ProcAccTask;
    friend class _ProcAccTask;
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
    class _DataBuffer;

    enum ImgType {AccImg, SatImg, TmpImg, NbImgTypes};
    static const char *toString(ImgType type);

    // This helper class implements a circular buffer for asynchronous readout
    // It provides two main functionalities:
    //  * An ordered, sequential queue of frames. Non-sequential frames are
    //    kept in an auxiliary container until the missing frames are inserted
    //  * Ensure that a maximum capacity is always respected, requiring the
    //    removal of the oldest frame (if already full) before next insert.
    //    The rationale behind is to guarantee that there is always one
    //    available frame in the memory allocation subsystem if the
    //    reqMemSizePercent was fixed in the BufferHelper::Parameters

    class _SortedDataQueue
    {
      DEB_CLASS_NAMESPC(DebModControl,"Accumulation::_SortedDataQueue", 
			"Control");
    public:
      void setMaxSize(long max_size);
      void removeOldestIfFull();
      void insert(const Data& data);
      void clear();

      const std::deque<Data>& getSequentialQueue() const { return m_sequential; }

    private:
      std::deque<Data>			m_sequential;
      CtControl::SortedDataType		m_non_sequential;
      long				m_max_size{1};
      long				m_pending_inserts{0};
    };

    struct _ProcAccInfo
    {
      int				frame_nb;
      int				acc_frames{0};
      std::map<int,Data>		new_pending_data;
      // Temporary data where frames are stored to compute the median
      // std::vector<Data>		tmp_datas;
      Data				data[NbImgTypes];

      _ProcAccInfo(int frame) : frame_nb(frame) {}
    };
      
    Parameters 				m_pars;
    long				m_buffers_size;
    // Circular buffer of output data (used by getFrame)
    _SortedDataQueue			m_datas;
    // Circular buffer of saturated images (used by readSaturatedImageCounter)
    _SortedDataQueue		        m_saturated_images;
    CtControl& 				m_ct;
    bool				m_calc_ready;
    std::deque<std::pair<Data,Data> >	m_calc_pending_data;
    TaskEventCallback* 			m_calc_end;
    _CalcSaturatedTaskMgr*		m_calc_mgr;
    Data				m_calc_mask;
    std::map<int,_ProcAccInfo>		m_proc_info_map;
    int					m_acc_nb_frames;
    mutable Cond 			m_cond;
    ThresholdCallback*			m_threshold_cb;
    bool 				m_last_continue_flag;
    int					m_hw_img_depth;
    int					m_hw_nb_buffers;
    FrameDim				m_frame_dim[NbImgTypes];
    BufferHelper::Parameters		m_buffer_params;
    BufferHelper			m_buffer_helper[NbImgTypes];

    // --- Methodes for acquisition
    void clear();
    void prepare();
    bool _newFrameReady(Data&);
    void _newBaseFrameReady(Data&);
    void _processBaseFrame(_ProcAccInfo&,Data&,AutoMutex&);
    void stop();

    void _calcImgFrameDims();
    void _calcBufferHelperParams(BufferHelper::Parameters params[NbImgTypes]);

    void getFrame(Data &,int frameNumber);
    BufferBase *_getDataBuffer(ImgType type, int size);

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
