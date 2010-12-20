#ifndef CTACCUMULATION_H
#define CTACCUMULATION_H

#include "Compatibility.h"
#include <list>
#include <deque>

#include "CtControl.h"

namespace lima
{
  class DLL_EXPORT CtAccumulation
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
      DEB_CLASS_NAMESPC(DebModControl,"AccConcat::ThresholdCallback", 
			"Control");
    public:
      ThresholdCallback() {};
      virtual ~ThresholdCallback() {};

      long long m_max;

    protected:
      virtual void aboveMax(Data&,long long value) = 0;
    };
    CtAccumulation(CtControl&);
    ~CtAccumulation();

    // --- accumulation adn concatenation parameters

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
    void readSaturatedSumCounter(int from,saturatedCounterResult &result);

    // --- Mask image to calculate sum counter
    void setMask(Data&);


    // --- Callback to monitor detector saturation

    void registerThresholdCallback(ThresholdCallback &cb);
    void unregisterThresholdCallback(ThresholdCallback &cb);
  private:
    Parameters 		m_pars;
    long		m_buffers_size;
    std::deque<Data> 	m_datas;
    std::deque<Data> 	m_saturated_images;
    CtControl& 		m_ct;
    mutable Mutex 	m_lock;

    // --- Methodes for acquisition
    void prepare();
    bool newFrameReady(Data&);
    void getFrame(Data &,int frameNumber);

    void _accFrame(Data &src,Data &dst);
    void _calcSaturatedImage(Data &src);
  };
}
#endif
