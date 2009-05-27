#ifndef BUFFERSAVE_H
#define BUFFERSAVE_H

#include <stdio.h>
#include <string>
#include <fstream>
#include "FrameBuilder.h"
#include "HwFrameCallback.h"

namespace lima {


class BufferSave {
  public :
	enum FileFormat {
		Raw, EDF,
	};

	typedef std::string String;

	BufferSave( );
	BufferSave( FileFormat format, const String& prefix, 
		    int idx = 0, const String& suffix = "", 
		    bool overwrite = false , int tot_file_frames = 1);
	~BufferSave( );

	void writeFrame( const FrameInfoType& finfo );

	void setPrefix(const String& prefix);
	void getPrefix(String& prefix) const;

	void setFormat(FileFormat  format);
	void getFormat(FileFormat& format) const;

	void setIndex(int  idx);
	void getIndex(int& idx) const;

	void setTotFileFrames(int  tot_file_frames);
	void getTotFileFrames(int& tot_file_frames) const;

	void getOpenFileName(String& file_name) const;
	bool isFileOpen() const;

  private:
	String getDefSuffix() const;
	void openFile();
	void closeFile();

	void writeEdfHeader( const FrameInfoType& finfo );

	FileFormat m_format;
	String m_prefix;
	int m_idx;
	String m_suffix;
	String m_file_name;
	bool m_overwrite;
	int m_written_frames;
	int m_tot_file_frames;
	FrameInfoType m_last_frame;
	std::ofstream *m_fout;
};


}

#endif /* BUFFERSAVE_H */
