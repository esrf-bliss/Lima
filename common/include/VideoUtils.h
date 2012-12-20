#ifndef VIDEOUTILS_H
#define VIDEOUTILS_H

#include <cstdlib>
#include <iostream>

#include "Constants.h"

#include "Data.h"

namespace lima
{
  struct VideoImage
  {
    VideoImage() :
      frameNumber(-1),
      height(-1),
      width(-1),
      inused(0),
      mode(Y8),
      buffer(NULL)
    {}
    ~VideoImage()
    {
      if(buffer)
	free(buffer);
    }
    long long   frameNumber;
    int 	height;
    int 	width;
    int		inused;
    VideoMode 	mode;
    char*	buffer;

    inline void alloc(int size)
    {
      if(!buffer || double(size) > this->size())
	buffer = (char*)realloc(buffer,size);
    }
    inline void setParams(int fNumber,int w,int h,VideoMode m)
    {
      double oldSize = size();
      frameNumber = fNumber;
      width = w;
      height = h;
      mode = m;
      double newSize = height * width * depth();
     if(!buffer || newSize > oldSize)
	{
	  int size = int(newSize + 0.5);
	  buffer = (char*)realloc(buffer,size);
	}
    }
    inline double size() const {return buffer ? height * width * depth() : 0;}
    static inline double mode_depth(VideoMode m)
    {
      switch(m)
	{
	case YUV411:
	  return 1.5;
	case BAYER_RG8:
	case Y8:
	  return 1.;
	case RGB555:
	case RGB565:
	case BAYER_RG16:
	case I420:
	case Y16:
	case YUV422:
	  return 2.;
	case RGB32:
	case BGR32:
	case Y32:
	  return 4.;
	//case RGB24:
	case BGR24:
	case YUV444:
	  return 3.;
	  
	default:
	  return -1;	/* ERROR */
	}
    }
    
    inline double depth() const
    {
      return mode_depth(mode);
    } 

  };

  void data2Image(Data &aData,VideoImage &anImage);
  void image2YUV(const unsigned char *srcPt,int width,int height,VideoMode mode,
		 unsigned char *dst);

  inline std::ostream& operator<<(std::ostream &os,
				  const VideoImage &anImage)
  {
    const char *stringMode;
    switch(anImage.mode)
      {
      case Y8: stringMode = "Y8";break;
      case Y16: stringMode = "Y16";break;
      case Y32: stringMode = "Y32";break;
      case Y64: stringMode = "Y64";break;
      case RGB555: stringMode = "RGB555";break;
      case RGB565: stringMode = "RGB565";break;
	  //case RGB24: stringMode = "RGB24";break;
      case RGB32: stringMode = "RGB32";break;
      case BGR24: stringMode = "BGR24";break;
      case BGR32: stringMode = "BGR32";break;
      case BAYER_RG8: stringMode = "BAYER_RG8";break;
      case BAYER_RG16: stringMode = "BAYER_RG16";break;
      case I420: stringMode = "I420";break;
      case YUV411: stringMode = "YUV411";break;
      case YUV422: stringMode = "YUV422";break;
      case YUV444: stringMode = "YUV444";break;
      default:
	stringMode = "Unknowed";
	break;
      }

    os << "<"
       << "frameNumber=" << anImage.frameNumber << ", "
       << "height=" << anImage.height << ", "
       << "width=" << anImage.width << ", "
       << "inused=" << anImage.inused << ", "
       << "mode=" << stringMode << ", "
       << "buffer=" << (void*)anImage.buffer
       << ">";
    return os;
  }
}
#endif
