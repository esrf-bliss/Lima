#ifndef __SOFTOPID_H
#define __SOFTOPID_H

#include "BackgroundSubstraction.h"

namespace lima
{
  enum SoftOpId
    {
      UNDEF,
      BACKGROUNDSUBSTRACTION,
      BINNING,
      BPM,
      FLATFIELDCORRECTION,
      FLIP,
      MASK,
      ROICOUNTER,
      SOFTROI
    };

  struct SoftOpKey
  {
    SoftOpId	m_id;
    const char *m_name;
  };
  
  struct SoftOpInstance
  {
    SoftOpKey m_key;
    void*     m_opt;
  };

  void getAvailableOp(const SoftOpKey*&);
  
  class SoftOpBackgroundSubstraction
  {
  public:
    SoftOpBackgroundSubstraction(SoftOpInstance&);
    ~SoftOpBackgroundSubstraction();
    
    void setBackgroundImage(const char* filename);
  private:
    Tasks::BackgroundSubstraction *m_opt;
  };

}
#endif
