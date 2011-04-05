#ifndef VIDEOUTILS_H
#define VIDEOUTILS_H

#include <cstdlib>

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
    int		frameNumber;
    int 	height;
    int 	width;
    int		inused;
    VideoMode 	mode;
    char*	buffer;

    inline void alloc(int size)
    {
      if(!buffer || double(size) > this->size())
	{
	  double allocSize = this->size();
	  size = int(allocSize + 0.5);
	  buffer = (char*)realloc(buffer,size);
	}
    }
    inline double size() const {return buffer ? height * width * depth() : 0;}
    inline double depth() const
    {
      switch(mode)
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
	case RGB24:
	case BGR24:
	case YUV444:
	  return 3.;
	  
	default:
	  return -1;	/* ERROR */
	}
    } 

  };

  void data2Image(Data &aData,VideoImage &anImage);
  void image2Data(VideoImage &anImage,Data &aData);
}
#endif
