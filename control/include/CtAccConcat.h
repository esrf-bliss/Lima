#ifndef CTACCCONCAT_H
#define CTACCCONCAT_H

namespace lima
{
  class CtAccConcat
  {
    DEB_CLASS_NAMESPC(DebModControl,"AccConcat","Control");
  public:
    struct Parameters
    {
      DEB_CLASS_NAMESPC(DebModControl,"AccConcat::Parameters","Control");
    public:
      Parameters();
      void reset();
      
      bool		active;	///< if true do the calculation
      long long		pixelThresholdValue; ///< value which determine the threshold of the calculation

      int		saturatedImageCounterBufferSize; ///< default 64
      
      bool	  	savingFlag; ///< saving flag if true save saturatedImageCounter
      std::string 	savePrefix; ///< prefix filename of saturatedImageCounter (default is saturated_image_counter)
    };
    
    class ThresholdCallback
    {
      DEB_CLASS_NAMESPC(DebModControl,"AccConcat::ThresholdCallback", 
			"Control");
    public:
      ThresholdCallback();
      virtual ~ThresholdCallback();
    protected:
      virtual void aboveMax(Data&,long long value) = 0;
      
      long long m_max;
    };
    CtAccConcat();
    ~CtAccConcat();

    // --- accumulation adn concatenation parameters

    void setParameters(const Parameters &pars);
    void getParameters(Parameters& pars) const;
    
    void setActive(bool activeFlag);
    void getActive(bool &activeFlag);

    void setPixelThresholdValue(const int &pixelThresholdValue);
    void getPixelThresholdValue(int &pixelThresholdValue) const;

    void setSaturatedImageCounterBufferSize(const int &saturatedImageCounterBufferSize);
    void getSaturatedImageCounterBufferSize(int &saturatedImageCounterBufferSize) const;

    void setSavingFlag(const bool &savingFlag);
    void getSavingFlag(bool &savingFlag) const;

    void setSavePrefix(const std::string &savePrefix);
    void getSavePrefix(std::string &savePrefix) const;

    // --- variable and data result of Concatenation or Accumulation

    void readSaturatedImageCounter(Data&,long frameNumber = -1) const;
    void readSaturatedSumCounter(int from,std::list<long long> &result) const;

    // --- Mask image to calculate sum counter
    void setMask(Data&);

    // --- Callback to monitor detector saturation

    void registerThresholdCallback(ThresholdCallback &cb);
    void unregisterThresholdCallback(ThresholdCallback &cb);
  private:
    Parameters m_pars;
  };
}
#endif
