#include "lima/Exceptions.h"

#include "lima/VideoUtils.h"
using namespace lima;

/** func tool to convert from color to Y only (luma) for greyscale image
 */
inline void _rgb555_2_yuv(const unsigned char *data,unsigned char *luma,
			   int column,int row)
{
  for(int aSize = column * row;aSize;--aSize,data += 2,++luma)
    {
      unsigned char red = (data[0] & 0x7c) >> 2;
      unsigned char green = ((data[0] & 0x03) << 3)  + ((data[1] & 0xe0) >> 5);
      unsigned char blue = data[1] & 0x1f;
      *luma = ((66 * red + 129 * green + 25 * blue) + 128) >> 8;
    }
}

inline void _rgb565_2_yuv(const unsigned char *data,unsigned char *luma,
			   int column,int row)
{
  for(int aSize = column * row;aSize;--aSize,data += 2,++luma)
    {
      unsigned char red = (data[0] & 0xf8) >> 3;
      unsigned char green = ((data[0] & 0x07) << 3)  + ((data[1] & 0xe0) >> 5);
      unsigned char blue = data[1] & 0x1f;
      *luma = ((66 * red + 129 * green + 25 * blue) + 128) >> 8;
    }
}

inline void _rgb_2_yuv(const unsigned char *data,unsigned char *luma,
			int column,int row,int bandes)
{
  for(int aSize = column * row;aSize;--aSize,data += bandes,++luma)
    *luma = ((66 * data[0] + 129 * data[1] + 25 * data[2]) + 128) >> 8;
}

inline void _bgr_2_yuv(const unsigned char *data,unsigned char *luma,
			int column,int row,int bandes)
{
  for(int aSize = column * row;aSize;--aSize,data += bandes,++luma)
    *luma = ((25 * data[0] + 129 * data[1] + 66 * data[2]) + 128) >> 8;
}

template<class xClass>
inline void _bayer_2_yuv(const xClass* bayer0,xClass* luma,
			 int column,int row,int blue,int start_with_green)
{
  int luma_step = column * sizeof(xClass);
  int bayer_step = column;
  xClass *luma0 = (xClass*)luma;
  memset( luma0, 0, luma_step);
  memset( luma0 + (row - 1)*bayer_step, 0, luma_step);
  luma0 += column + 1;
  row -= 2;
  column -= 2;

  for( ; row > 0;--row,bayer0 += bayer_step, luma0 += bayer_step )
    {
      int t0, t1;
      const xClass* bayer = bayer0;
      xClass* dst = luma0;
      const xClass* bayer_end = bayer + column;

      dst[-1] = 0;

      if(column <= 0 )
	continue;

      if( start_with_green )
        {
	  t0 = (bayer[1] + bayer[bayer_step*2+1] + 1) >> 1;
	  t1 = (bayer[bayer_step] + bayer[bayer_step+2] + 1) >> 1;
	  if(blue < -1)
	    *dst = (bayer[bayer_step+1] * 150 + t0 * 29 + t1 * 76) >> 8;
	  else
	    *dst = (bayer[bayer_step+1] * 150 + t1 * 29 + t0 * 76) >> 8;
	  ++bayer;
	  ++dst;
        }

      if( blue > 0 )
        {
	  for( ; bayer <= bayer_end - 2; bayer += 2)
            {
	      t0 = (bayer[0] + bayer[2] + bayer[bayer_step*2] +
		    bayer[bayer_step*2+2] + 2) >> 2;
	      t1 = (bayer[1] + bayer[bayer_step] +
		    bayer[bayer_step+2] + bayer[bayer_step*2+1]+2) >> 2;
	      *dst = (t0 * 76 + t1 * 150 + bayer[bayer_step+1] * 29) >> 8;
	      ++dst;

	      t0 = (bayer[2] + bayer[bayer_step*2+2] + 1) >> 1;
	      t1 = (bayer[bayer_step+1] + bayer[bayer_step+3] + 1) >> 1;
	      *dst = (t0 * 76 + bayer[bayer_step+2] * 150 + t1 * 29) >> 8;
	      ++dst;
            }
        }
      else
        {
	  for( ; bayer <= bayer_end - 2; bayer += 2)
            {
	      t0 = (bayer[0] + bayer[2] + bayer[bayer_step*2] +
		    bayer[bayer_step*2+2] + 2) >> 2;
	      t1 = (bayer[1] + bayer[bayer_step] +
		    bayer[bayer_step+2] + bayer[bayer_step*2+1]+2) >> 2;
	      *dst = (t0 * 29 + t1 * 150 + bayer[bayer_step+1] * 76) >> 8;
	      ++dst;

	      t0 = (bayer[2] + bayer[bayer_step*2+2] + 1) >> 1;
	      t1 = (bayer[bayer_step+1] + bayer[bayer_step+3] + 1) >> 1;
	      *dst = (t0 * 29 + bayer[bayer_step+2] * 150 + t1 * 76) >> 8;
	      ++dst;
            }
        }

      if( bayer < bayer_end )
        {
	  t0 = (bayer[0] + bayer[2] + bayer[bayer_step*2] +
		bayer[bayer_step*2+2] + 2) >> 2;
	  t1 = (bayer[1] + bayer[bayer_step] +
		bayer[bayer_step+2] + bayer[bayer_step*2+1]+2) >> 2;
	  if(blue > 0)
	    *dst = (t0 * 76 + t1 * 150 +  bayer[bayer_step+1] * 29) >> 8;
	  else
	    *dst = (t0 * 29 + t1 * 150 +  bayer[bayer_step+1] * 76) >> 8;
	  ++bayer;
	  ++dst;
        }

      blue = -blue;
      start_with_green = !start_with_green;
    }
}

template<class xClass>
inline void _bayer_rg_2_yuv(const xClass* bayer0,xClass* luma,
			    int column,int row)
{
  _bayer_2_yuv<xClass>(bayer0,luma,column,row,1,0);
}

template<class xClass>
inline void _bayer_bg_2_yuv(const xClass* bayer0,xClass* luma,
			    int column,int row)
{
  _bayer_2_yuv<xClass>(bayer0,luma,column,row,-1,0);
}

inline void _yuv422packed_2_yuv(const unsigned char *data,unsigned char *luma,
			int column,int row)
{
  // format 4 bytes for 2 pixels U and V common for 2pixels: |U0|Y0|V0|Y1|  |U2|Y2|V2|Y3|
  // so Y (luma) is every 2 bytes
  long nbIter = column * row /2;
  --nbIter;
  for(const unsigned char *src = data+1; nbIter; --nbIter, src += 4, luma+=2)
    {
      luma[0] = src[0];
      luma[1] = src[2];
    }
}

void lima::data2Image(Data &aData,VideoImage &anImage)
{
  if(!aData.empty())
    {
      switch(aData.type)
	{
	case Data::UINT8:
	case Data::INT8:
	  anImage.mode = Y8;break;
	case Data::UINT16:
	case Data::INT16:
	  anImage.mode = Y16;break;
	case Data::UINT32:
	case Data::INT32:
	  anImage.mode = Y32;break;
	case Data::UINT64:
	case Data::INT64:
	  anImage.mode = Y64;break;
	case Data::FLOAT:
	case Data::DOUBLE:
	default:
	  throw LIMA_COM_EXC(Error, "Data type is not yet used for VideoImage");
	}
      anImage.alloc(aData.size());
      memcpy(anImage.buffer,aData.data(),aData.size());
      anImage.width = aData.dimensions[0];
      anImage.height = aData.dimensions[1];
      anImage.frameNumber = aData.frameNumber;
    }
}

/*
 * convert the video color image to Y (luma only) greyscale image
 */
void lima::image2YUV(const unsigned char *srcPt,int width,int height,VideoMode mode,
		     unsigned char *dst)
{

  switch(mode)
    {
    case Y8:
    case Y16:
    case Y32:
    case Y64:
      {
	int size = int((width * height * VideoImage::mode_depth(mode)) + .5);
	memcpy(dst,srcPt,size);
	break;
      }
    case I420:
    case YUV411:
    case YUV422:
    case YUV444:
      memcpy(dst,srcPt,width * height);
      break;
    case YUV422PACKED:
      _yuv422packed_2_yuv(srcPt,dst,width,height);
    case RGB555:
      _rgb555_2_yuv(srcPt,dst,width,height);
      break;
    case RGB565:
      _rgb565_2_yuv(srcPt,dst,width,height);
      break;
    case BAYER_RG8:
      _bayer_rg_2_yuv(srcPt,dst,width,height);
      break;
    case BAYER_RG16:
      _bayer_rg_2_yuv((unsigned short*)srcPt,(unsigned short*)dst,width,height);
      break;
    case BAYER_BG8:
      _bayer_bg_2_yuv(srcPt,dst,width,height);
      break;
    case BAYER_BG16:
      _bayer_bg_2_yuv((unsigned short*)srcPt,(unsigned short*)dst,width,height);
      break;
    case RGB32:
      _rgb_2_yuv(srcPt,dst,width,height,4);
      break;
    case BGR32:
      _bgr_2_yuv(srcPt,dst,width,height,4);
      break;
    case RGB24:
      _rgb_2_yuv(srcPt,dst,width,height,3);
      break;
    case BGR24:
      _bgr_2_yuv(srcPt,dst,width,height,3);
      break;
    default:
      throw LIMA_COM_EXC(Error,"Video mode not yet managed!");
    }
}
