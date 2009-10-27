#include "MaxipixReconstruction.h"

using namespace lima::Maxipix;

static const int MAXIPIX_LINE_SIZE = 256 * 2;
static const int MAXIPIX_NB_LINE = 256;

#define COPY_5x1 \
  /* set aSrcPt to the last line of the 5th chip */	 \
  unsigned char *aSrcPt = ((unsigned char*)src.data()) + \
    MAXIPIX_LINE_SIZE * 4 * MAXIPIX_NB_LINE; \
  /*set aDstPt to the last line of the 5th chip including the gap */	\
  unsigned char *aDstPt = ((unsigned char*)dst.data()) + \
    (MAXIPIX_LINE_SIZE * 4 * MAXIPIX_NB_LINE) + \
    (xSpace * 2 * 4 * MAXIPIX_NB_LINE); \
 \
  /*number of iteration == number of line * number of chip */		\
  /* in case of inplace remove the last memcopy (first line of the first chip) */ \
  int nbIter = (MAXIPIX_NB_LINE * 5) - int(inplace); \
  do \
    { \
      memcpy(aDstPt,aSrcPt,MAXIPIX_LINE_SIZE_COPY); \
      GAP_COPY_FUNCTION\
      aDstPt -= MAXIPIX_LINE_SIZE + xSpace * 2; \
      aSrcPt -= MAXIPIX_LINE_SIZE; \
    } \
  while(--nbIter); 


static inline void _zero_5x1(Data &src,Data &dst,
			     int xSpace,bool inplace)
{
  int MAXIPIX_LINE_SIZE_COPY = MAXIPIX_LINE_SIZE;
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION \
  unsigned short *aPixel = ((unsigned short*)aDstPt) - 1; \
  for(int i = xSpace;i;--i,--aPixel) \
    *aPixel = 0;
COPY_5x1
}

static inline void _dispatch_5x1(Data &src,Data &dst,
				 int xSpace,bool inplace)
{
  int MAXIPIX_LINE_SIZE_COPY = MAXIPIX_LINE_SIZE;
  int nbPixelDispatch = (xSpace >> 1) + 1; // (xSpace / 2) + 1
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION \
  MAXIPIX_LINE_SIZE_COPY = MAXIPIX_LINE_SIZE - 2; \
  unsigned short *aPixel = ((unsigned short*)aDstPt); \
  unsigned short aPixelValue = *aPixel / nbPixelDispatch; \
  for(int i = nbPixelDispatch;i;--i,--aPixel) \
    *aPixel = aPixelValue; \
   \
  unsigned short *aSrcPixel = ((unsigned short*)aSrcPt) - 1; \
  aPixelValue = *aSrcPixel / nbPixelDispatch; \
  for(int i = nbPixelDispatch;i;--i,--aPixel) \
    *aPixel = aPixelValue; 
COPY_5x1
}

static inline void _mean_5x1(Data &src,Data &dst,
				 int xSpace,bool inplace)
{
  int MAXIPIX_LINE_SIZE_COPY = MAXIPIX_LINE_SIZE;
  int nbPixelMean = (xSpace >> 1) + 1; // (xSpace / 2) + 1
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION \
  MAXIPIX_LINE_SIZE_COPY = MAXIPIX_LINE_SIZE - 2; \
  unsigned short *aPixel = ((unsigned short*)aDstPt); \
  unsigned short aFirstPixelValue = *aPixel / nbPixelMean; \
  *aPixel = aFirstPixelValue;--aPixel; \
   \
  unsigned short *aSrcPixel = ((unsigned short*)aSrcPt) - 1; \
  unsigned short aSecondPixelValue = *aSrcPixel / nbPixelMean; \
  float aStepValue = (aSecondPixelValue - aFirstPixelValue) / (xSpace + 1); \
  float aPixelValue = aFirstPixelValue + aSecondPixelValue; \
  for(int i = xSpace + 1;i;--i,aPixelValue += aStepValue,--aPixel) \
    *aPixel = (unsigned short)aPixelValue;
COPY_5x1
}

Data MaxipixReconstruction::process(Data &aData)
{
  Data aReturnData;
  aReturnData = aData;

  if(_processingInPlaceFlag)
    {
      if(mModel == M_5x1)
	{
	  switch(mType)
	    {
	    case ZERO: 
	      _zero_5x1(aData,aData,mXSpace,_processingInPlaceFlag);break;
	    case DISPATCH:
	      _dispatch_5x1(aData,aData,mXSpace,_processingInPlaceFlag);break;
	    case MEAN:
	      _mean_5x1(aData,aData,mXSpace,_processingInPlaceFlag);break;
	    default:		// ERROR
	      break;
	    }
	}
      else			// Model 4x2
	{
	}
    }
  else
    {
      if(mModel == M_5x1)
	{
	  Buffer *aNewBuffer = new Buffer((MAXIPIX_LINE_SIZE * 5 + (mXSpace << 1) * 4) * 
					  MAXIPIX_NB_LINE);
	  aReturnData.setBuffer(aNewBuffer);
	  switch(mType)
	    {
	    case ZERO: 
	      _zero_5x1(aData,aReturnData,mXSpace,_processingInPlaceFlag);break;
	    case DISPATCH:
	      _dispatch_5x1(aData,aReturnData,mXSpace,_processingInPlaceFlag);break;
	    case MEAN:
	      _mean_5x1(aData,aReturnData,mXSpace,_processingInPlaceFlag);break;
	    default:		// ERROR
	      break;
	    }
	}
      else			// Model 4x2
	{
	}
    }
  return aReturnData;
}
