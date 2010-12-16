#ifndef CTACCUMULATION_H
#define CTACCUMULATION_H

#include <list>
#include <deque>

#include "CtControl.h"
#include "SinkTaskMgr.h"

namespace lima
{
  class CtAccumulation
  {
    DEB_CLASS_NAMESPC(DebModControl,"Accumulation","Control");
  public:
    friend class CtControl;
    friend class CtBuffer;
    friend class CtBufferFrameCB;

    typedef std::list<std::list<long long> > saturatedCounterResult;

    struct Parameters
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

    // --- Methodes for acquisition
    void prepare();
    bool newFrameReady(Data&);
    void getFrame(Data &,int frameNumber);

    void _accFrame(Data &src,Data &dst);
    void _calcSaturatedImageNCounters(Data &src,Data &dst);

    inline void _callIfNeedThresholdCallback(Data &aData,long long value);
  };
}
#endif
