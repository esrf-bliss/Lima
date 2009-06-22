#include "SoftOpId.h"
using namespace lima;

static SoftOpKey SoftOpTable[] = {
  {BACKGROUNDSUBSTRACTION,"Background substraction"},
  {BINNING,"Binning"},
  {BPM,"Bpm"},
  {FLATFIELDCORRECTION,"Flat field correction"},
  {FLIP,"Flip"},
  {MASK,"Mask"},
  {ROICOUNTER,"Roi counter"},
  {SOFTROI,"Software roi"},
  {NULL}
};

//-------------------- BACKGROUND SUBSTRACTION --------------------
				   
/** @brief small wrapper around BackgroundSubstraction Task
 */
SoftOpBackgroundSubstraction::SoftOpBackgroundSubstraction(SoftOpInstance &operation) :
  m_opt(NULL)
{
  if(!operation.m_key.m_id != BackgroundSubstraction)
    {
				// throw error
    }
  else
    {
      m_opt = (BackgroundSubstraction*)operation.m_opt;
      m_opt.ref();
    }
}

SoftOpBackgroundSubstraction::~SoftOpBackgroundSubstraction()
{
  m_opt.unref();
}

SoftOpBackgroundSubstraction::setBackgroundImage(const char *filename)
{
  
}
