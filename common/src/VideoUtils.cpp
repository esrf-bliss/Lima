#include "Exceptions.h"

#include "VideoUtils.h"


using namespace lima;

void lima::data2Image(Data &aData,VideoImage &anImage)
{
  if(!aData.empty())
    {
      VideoMode mode;
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
      anImage.mode = mode;
      anImage.width = aData.dimensions[0];
      anImage.height = aData.dimensions[1];
      anImage.frameNumber = aData.frameNumber;
    }
}

void lima::image2Data(VideoImage &anImage,Data &aData)
{
}
