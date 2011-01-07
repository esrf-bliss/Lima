/***************************************************************//**
 * @file HwBufferSave.h
 * @brief This file contains the HwBufferSave class used to save frames
 *
 * @author A.Kirov, A.Homs
 * @date 03/06/2009
 *******************************************************************/

#ifndef BUFFERSAVE_H
#define BUFFERSAVE_H

#include <stdio.h>
#include <string>
#include <fstream>

#include "LimaCompatibility.h"
#include "HwFrameInfo.h"

namespace lima {


/***************************************************************//**
 * @class HwBufferSave
 *
 * The main method is writeFrame(const HwFrameInfoType& finfo).
 * The other methods configure the saving parameters.
 *******************************************************************/
class LIMACORE_API HwBufferSave {
  public :
	enum FileFormat {
		Raw, EDF,
	};

	HwBufferSave( FileFormat format = Raw, 
		    const std::string& prefix = "img", 
		    int idx = 0, const std::string& suffix = "", 
		    bool overwrite = false , int tot_file_frames = 1);
	~HwBufferSave( );

	void writeFrame( const HwFrameInfoType& finfo );

	void setPrefix(const std::string& prefix);
	void getPrefix(std::string& prefix) const;

	void setFormat(FileFormat  format);
	void getFormat(FileFormat& format) const;

	void setIndex(int  idx);
	void getIndex(int& idx) const;

	void setTotFileFrames(int  tot_file_frames);
	void getTotFileFrames(int& tot_file_frames) const;

	void getOpenFileName(std::string& file_name) const;
	bool isFileOpen() const;

  private:
	std::string getDefSuffix() const;
	void openFile();
	void closeFile();

	void writeEdfHeader( const HwFrameInfoType& finfo );

	FileFormat m_format;
	std::string m_prefix;
	int m_idx;
	std::string m_suffix;
	std::string m_file_name;
	bool m_overwrite;
	int m_written_frames;
	int m_tot_file_frames;
	HwFrameInfoType m_last_frame;
	std::ofstream *m_fout;
};


}

#endif /* BUFFERSAVE_H */
