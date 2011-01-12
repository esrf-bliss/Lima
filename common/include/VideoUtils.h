#ifndef VIDEOUTILS_H
#define VIDEOUTILS_H

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
      mode(Y8),
      buffer(NULL)
    {}

    int		frameNumber;
    int 	height;
    int 	width;
    VideoMode 	mode;
    char*	buffer;
  };

  void data2Image(Data &aData,VideoImage &anImage);
  void image2Data(VideoImage &anImage,Data &aData);
}
#endif
