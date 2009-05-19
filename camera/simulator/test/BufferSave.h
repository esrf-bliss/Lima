#ifndef BUFFERSAVE_H
#define BUFFERSAVE_H
#include <string>
#include <fstream>
#include "FrameBuilder.h"

namespace lima {


#define EDF_HEADER_LEN		1024
#define EDF_HEADER_BUFFER_LEN	(10 * EDF_HEADER_LEN)


typedef struct FrameInfo {
	unsigned long acq_frame_nb;
	void *frame_ptr;
	int width, height, depth;
	double frame_time_stamp;
} FrameInfoType;


enum FileFormat {
	FMT_RAW,
	FMT_EDF,
};


class BufferSave {
  public :
	std::string m_prefix;
	enum FileFormat m_format;

	BufferSave( const std::string &prefix, enum FileFormat format );
	~BufferSave( );

	int writeFrame( FrameInfoType &finfo );

  private:
	int writeEdfHeader( const FrameInfoType &finfo, std::ofstream &file );

};


}

#endif /* BUFFERSAVE_H */
