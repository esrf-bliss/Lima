#include "MaxipixReconstruction.h"

using namespace lima::Maxipix;

static const int MAXIPIX_LINE_SIZE = 256 * 2;
static const int MAXIPIX_NB_LINE = 256;
static const int MAXIPIX_NB_COLUMN = 256;

//----------------------------------------------------------------------------
//			  5x1 copy function
//----------------------------------------------------------------------------
#define COPY_5x1 \
  /* set aSrcPt to the last line of the 5th chip */	 \
  unsigned char *aSrcPt = ((unsigned char*)src.data()) + \
    MAXIPIX_LINE_SIZE * 4 * MAXIPIX_NB_LINE; \
  /*set aDstPt to the last line of the 5th chip including the gap */	\
  unsigned char *aDstPt = ((unsigned char*)dst.data()) + \
    (MAXIPIX_LINE_SIZE * 4 * MAXIPIX_NB_LINE) + \
    (xSpace * 2 * 4 * MAXIPIX_NB_LINE); \
 \
  int aNbLine = MAXIPIX_NB_LINE; \
  do \
    { \
      int MAXIPIX_LINE_SIZE_COPY = MAXIPIX_LINE_SIZE; \
      						      \
      int aNbChipCopyWithGap = 4; \
      do			  \
    	{ \
	  memcpy(aDstPt,aSrcPt,MAXIPIX_LINE_SIZE_COPY); \
	  GAP_COPY_FUNCTION \
	  aDstPt -= MAXIPIX_LINE_SIZE + xSpace * 2; \
	  aSrcPt -= MAXIPIX_LINE_SIZE; \
	} \
      while(--aNbChipCopyWithGap);		    \
      memcpy(aDstPt,aSrcPt,MAXIPIX_LINE_SIZE_COPY); \
      aDstPt -= MAXIPIX_LINE_SIZE; \
      aSrcPt -= MAXIPIX_LINE_SIZE; \
    } \
  while(--aNbLine);

static inline void _raw_5x1(Data &src,Data &dst,
			     int xSpace)
{
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION \
  unsigned short *aPixel = ((unsigned short*)aDstPt) - 1; \
  for(int i = xSpace;i;--i,--aPixel) \
    *aPixel = 0;
COPY_5x1
}

static inline void _zero_5x1(Data &src,Data &dst,
			     int xSpace)
{
#ifdef GAP_COPY_FUNCTION
#undef GAP_COPY_FUNCTION
#endif
#define GAP_COPY_FUNCTION \
  MAXIPIX_LINE_SIZE_COPY = MAXIPIX_LINE_SIZE - 2; \
  unsigned short *aPixel = ((unsigned short*)aDstPt); \
  for(int i = xSpace + 2;i;--i,--aPixel) \
    *aPixel = 0;
COPY_5x1
}
static inline void _dispatch_5x1(Data &src,Data &dst,
				 int xSpace)
{
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
				 int xSpace)
{
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
//----------------------------------------------------------------------------
//			     2x2 function
//----------------------------------------------------------------------------
static inline void copy_2x2(Data &src,Buffer *dst,int xSpace,int ySpace)
{
  unsigned short *aSrcPt = (unsigned short*)src.data();
  unsigned short *aDstPt = ((unsigned short*)dst->data) + 
    (((MAXIPIX_LINE_SIZE + xSpace) * (MAXIPIX_NB_LINE + (ySpace >> 1))) << 1) -
    MAXIPIX_LINE_SIZE;

  int aJump2LeftChip = MAXIPIX_LINE_SIZE + xSpace - 1;
  
  int aColumnIterNumber = MAXIPIX_NB_COLUMN;
  do
    {
      // copy left chip
      int chipNb = 2;
      do
	{
	  for(int i = MAXIPIX_NB_LINE;i;--i,aDstPt -= (MAXIPIX_LINE_SIZE + xSpace),++aSrcPt)
	    *aDstPt = *aSrcPt;
 
	  aDstPt -= (MAXIPIX_LINE_SIZE + xSpace) * ySpace;
	}
      while(--chipNb);

  
      aDstPt += aJump2LeftChip;
      //copy right chips
      chipNb = 2;
      do
	{
	  for(int i = MAXIPIX_NB_LINE;i;--i,aDstPt += (MAXIPIX_LINE_SIZE + xSpace),++aSrcPt)
	    *aDstPt = *aSrcPt;

	  aDstPt += (MAXIPIX_LINE_SIZE + xSpace) * ySpace;
	}
      while(--chipNb);

      --aJump2LeftChip;
      aDstPt -= aJump2LeftChip;
    }
  while(--aColumnIterNumber);

}

static inline void _raw_2x2(Data &src,Buffer *dst,int xSpace,int ySpace)
{
  unsigned short *aDstPt = ((unsigned short*)dst->data) + MAXIPIX_NB_COLUMN;
  for(int i = MAXIPIX_NB_LINE;i;--i,aDstPt += MAXIPIX_LINE_SIZE)
    for(int k = 0;k < xSpace;++k)
      aDstPt[k] = 0;
  
  aDstPt += MAXIPIX_LINE_SIZE - MAXIPIX_NB_COLUMN;
  int aGapSize = (MAXIPIX_LINE_SIZE + (xSpace << 1)) * ySpace;
  memset(aDstPt,0,aGapSize);
  aDstPt += aGapSize;

  aDstPt += MAXIPIX_NB_COLUMN;
  for(int i = MAXIPIX_NB_LINE;i;--i,aDstPt += MAXIPIX_LINE_SIZE)
    for(int k = 0;k < xSpace;++k)
      aDstPt[k] = 0;

  copy_2x2(src,dst,xSpace,ySpace);
}

static inline void _zero_2x2(Data &src,Buffer *dst,int xSpace,int ySpace)
{
  copy_2x2(src,dst,xSpace,ySpace);
  
  unsigned short *aDstPt = ((unsigned short*)dst->data) + MAXIPIX_NB_COLUMN - 1;
  for(int i = MAXIPIX_NB_LINE - 1;i;--i,aDstPt += MAXIPIX_LINE_SIZE)
    for(int k = 0;k < xSpace + 2;++k)
      aDstPt[k] = 0;
  
  aDstPt += MAXIPIX_LINE_SIZE - (MAXIPIX_NB_COLUMN - 1);
  int aGapSize = (MAXIPIX_LINE_SIZE + (xSpace << 1)) * (ySpace + 2);
  memset(aDstPt,0,aGapSize);
  aDstPt += aGapSize;

  aDstPt += MAXIPIX_NB_COLUMN - 1;
  for(int i = MAXIPIX_NB_LINE - 1;i;--i,aDstPt += MAXIPIX_LINE_SIZE)
    for(int k = 0;k < xSpace + 2;++k)
      aDstPt[k] = 0;
}

static inline void _dispatch_2x2(Data &src,Buffer *dst,int xSpace,int ySpace)
{
  copy_2x2(src,dst,xSpace,ySpace);
  
  unsigned short *aDstPt = ((unsigned short*)dst->data) + MAXIPIX_NB_COLUMN - 1;
  int aNbPixel2Dispatch = (xSpace >> 1) + 1;

  for(int lineId = MAXIPIX_LINE_SIZE - 1;lineId;--lineId)
    {
      unsigned short aPixelValue = *aDstPt / aNbPixel2Dispatch;
      for(int i = aNbPixel2Dispatch;i;--i,++aDstPt)
	*aDstPt = aPixelValue;

      aPixelValue = aDstPt[aNbPixel2Dispatch - 1] / aNbPixel2Dispatch;
      for(int i = aNbPixel2Dispatch;i;--i,++aDstPt)
	*aDstPt = aPixelValue;
  
      aDstPt += MAXIPIX_LINE_SIZE - 2;
    }
  

  aDstPt -= MAXIPIX_NB_COLUMN - 1;
  int aNbPixel2DispatchInY = (ySpace >> 1) + 1;
  int aLineSize = MAXIPIX_LINE_SIZE + xSpace;
  
  int aPart = 2;
  /* aPart == 2 => upper chip
     aPart == 1 => bottom chip
  */
  do
    {
      //Last line or first line of left chip (depend on aPart)
      for(int columnIter = MAXIPIX_NB_COLUMN - 1;columnIter;--columnIter,++aDstPt)
	{
	  unsigned short aPixelValue = *aDstPt / aNbPixel2DispatchInY;
	  for(int y = 0;y < aNbPixel2DispatchInY;++y)
	    aDstPt[aLineSize * y] = aPixelValue;
	}
  
      //Bottom or top right corner of left chip
      unsigned short aPixelValue = *aDstPt / (aNbPixel2DispatchInY * aNbPixel2Dispatch);
      for(int columnIter = aNbPixel2Dispatch;columnIter;--columnIter,++aDstPt)
	{
	  for(int y = 0;y < aNbPixel2DispatchInY;++y)
	    aDstPt[aLineSize * y] = aPixelValue;
	}
      //Bottom or top left corner of right chip
      aPixelValue = aDstPt[aNbPixel2Dispatch - 1] / (aNbPixel2DispatchInY * aNbPixel2Dispatch);
      for(int columnIter = aNbPixel2Dispatch;columnIter;--columnIter,++aDstPt)
	{
	  for(int y = 0;y < aNbPixel2DispatchInY;++y)
	    aDstPt[aLineSize * y] = aPixelValue;
	}
      //Last or first line of right chip
      for(int columnIter = MAXIPIX_NB_COLUMN - 1;columnIter;--columnIter,++aDstPt)
	{
	  aPixelValue = *aDstPt / aNbPixel2DispatchInY;
	  for(int y = 0;y < aNbPixel2DispatchInY;++y)
	    aDstPt[aLineSize * y] = aPixelValue;
	}
      
      //Set Variable for second part iteration (bottom chip)
      aLineSize = -aLineSize;
      aDstPt += aLineSize * (ySpace >> 1);
    }
  while(--aPart);

  aDstPt = ((unsigned short*)dst->data) + aLineSize * (MAXIPIX_NB_LINE + ySpace + 1) +
    MAXIPIX_NB_COLUMN - 1;
  
  for(int lineId = MAXIPIX_LINE_SIZE - 1;lineId;--lineId)
    {
      unsigned short aPixelValue = *aDstPt / aNbPixel2Dispatch;
      for(int i = aNbPixel2Dispatch;i;--i,++aDstPt)
	*aDstPt = aPixelValue;

      aPixelValue = aDstPt[aNbPixel2Dispatch - 1] / aNbPixel2Dispatch;
      for(int i = aNbPixel2Dispatch;i;--i,++aDstPt)
	*aDstPt = aPixelValue;
  
      aDstPt += MAXIPIX_LINE_SIZE - 2;
    }
}

static inline void _mean_2x2(Data &src,Buffer *dst,int xSpace,int ySpace)
{
  copy_2x2(src,dst,xSpace,ySpace);
}

Data MaxipixReconstruction::process(Data &aData)
{
  Data aReturnData;
  aReturnData = aData;

  if(mModel == M_5x1)
    {
      if(!_processingInPlaceFlag)
	{
	  Buffer *aNewBuffer = new Buffer((MAXIPIX_LINE_SIZE * 5 + (mXSpace << 1) * 4) * 
					  MAXIPIX_NB_LINE);
	  aReturnData.setBuffer(aNewBuffer);
	}
      switch(mType)
	{
	case RAW:
	  _raw_5x1(aData,aReturnData,mXSpace);break;
	case ZERO: 
	  _zero_5x1(aData,aReturnData,mXSpace);break;
	case DISPATCH:
	  _dispatch_5x1(aData,aReturnData,mXSpace);break;
	case MEAN:
	  _mean_5x1(aData,aReturnData,mXSpace);break;
	default:		// ERROR
	  break;
	}
    }
  else			// Model 2x2
    {
      int aBufferSize = ((MAXIPIX_LINE_SIZE + mXSpace) * 2) * 
		       ((MAXIPIX_NB_LINE   + mYSpace) * 2);
      Buffer *aNewBuffer = new Buffer(aBufferSize);
      switch(mType)
	{
	case RAW:
	  _raw_2x2(aData,aNewBuffer,mXSpace,mYSpace);break;
	case ZERO: 
	  _zero_2x2(aData,aNewBuffer,mXSpace,mYSpace);break;
	case DISPATCH:
	  _dispatch_2x2(aData,aNewBuffer,mXSpace,mYSpace);break;
	case MEAN:
	  _mean_2x2(aData,aNewBuffer,mXSpace,mYSpace);break;
	default:		// ERROR
	  break;
	}

      if(_processingInPlaceFlag)
	{
	  unsigned char *aSrcPt = (unsigned char*)aNewBuffer->data;
	  unsigned char *aDstPt = (unsigned char*)aData.data();
	  memcpy(aDstPt,aSrcPt,aBufferSize);
	  aNewBuffer->unref();
	}
      else
	aReturnData.setBuffer(aNewBuffer);
    }

  return aReturnData;
}
