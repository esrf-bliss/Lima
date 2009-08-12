#include "SoftOpId.h"
using namespace lima;
#include "BackgroundSubstraction.h"

//-------------------- BACKGROUND SUBSTRACTION --------------------
				   
/** @brief small wrapper around BackgroundSubstraction Task
 */
SoftOpBackgroundSubstraction::SoftOpBackgroundSubstraction(SoftOpInstance &operation) :
  m_opt(NULL)
{
  if(!operation.m_key.m_id != BACKGROUNDSUBSTRACTION)
    {
				// throw error
    }
  else
    {
      m_opt = (Tasks::BackgroundSubstraction*)operation.m_opt;
      m_opt->ref();
    }
}

SoftOpBackgroundSubstraction::~SoftOpBackgroundSubstraction()
{
  m_opt->unref();
}

void SoftOpBackgroundSubstraction::setBackgroundImage(const char *filename)
{
  
}
