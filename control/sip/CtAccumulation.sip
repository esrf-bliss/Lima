
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

    int m_max;

   protected:
    virtual void aboveMax(Data&,long long value) = 0;
  };

  CtAccumulation(CtControl&);
  ~CtAccumulation();

  // --- accumulation adn concatenation parameters

  void setParameters(const Parameters &pars);
  void getParameters(Parameters& pars /Out/) const;
  
  void setActive(bool activeFlag);
  void getActive(bool &activeFlag /Out/);

  void setPixelThresholdValue(long long pixelThresholdValue);
  void getPixelThresholdValue(long long &pixelThresholdValue /Out/) const;

  void getBufferSize(int &aBufferSize /Out/) const;

  void setSavingFlag(bool savingFlag);
  void getSavingFlag(bool &savingFlag /Out/) const;

  void setSavePrefix(const std::string &savePrefix);
  void getSavePrefix(std::string &savePrefix /Out/) const;

  // --- variable and data result of Concatenation or Accumulation

  void readSaturatedImageCounter(Data& /Out/,long frameNumber = -1);
  void readSaturatedSumCounter(saturatedCounterResult &result /Out/,int from = -1);

  // --- Mask image to calculate sum counter
  void setMask(Data&);


  // --- Callback to monitor detector saturation

  void registerThresholdCallback(ThresholdCallback &cb);
  void unregisterThresholdCallback(ThresholdCallback &cb);
};
