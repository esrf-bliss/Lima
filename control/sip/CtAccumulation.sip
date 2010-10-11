
class CtAccumulation
{
%TypeHeaderCode
#include <CtAccumulation.h>
using namespace lima;
%End
public:
  typedef std::list<std::list<long long> > saturatedCounterResult;

  struct Parameters
  {
    Parameters();
    void reset();
    
    bool		active;	///< if true do the calculation
    long long		pixelThresholdValue; ///< value which determine the threshold of the calculation

    bool	  	savingFlag; ///< saving flag if true save saturatedImageCounter
    std::string 	savePrefix; ///< prefix filename of saturatedImageCounter (default is saturated_image_counter)
  };
  
  class ThresholdCallback
  {
   public:
    ThresholdCallback();
    virtual ~ThresholdCallback();

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

  void setPixelThresholdValue(int pixelThresholdValue);
  void getPixelThresholdValue(int &pixelThresholdValue) const;

  void getBufferSize(int &aBufferSize) const;

  void setSavingFlag(bool savingFlag);
  void getSavingFlag(bool &savingFlag) const;

  void setSavePrefix(const std::string &savePrefix);
  void getSavePrefix(std::string &savePrefix) const;

  // --- variable and data result of Concatenation or Accumulation

//  void readSaturatedImageCounter(Data&,long frameNumber = -1);
//  void readSaturatedSumCounter(int from,saturatedCounterResult &result);

  // --- Mask image to calculate sum counter
  void setMask(Data&);


  // --- Callback to monitor detector saturation

  void registerThresholdCallback(ThresholdCallback &cb);
  void unregisterThresholdCallback(ThresholdCallback &cb);
};
