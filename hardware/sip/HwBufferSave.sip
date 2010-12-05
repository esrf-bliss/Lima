class HwBufferSave 
{
%TypeHeaderCode
#include <HwBufferSave.h>
using namespace lima;
%End
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
	void getPrefix(std::string& prefix /Out/) const;

	void setFormat(FileFormat  format);
	void getFormat(FileFormat& format /Out/) const;

	void setIndex(int  idx);
	void getIndex(int& idx /Out/) const;

	void setTotFileFrames(int  tot_file_frames);
	void getTotFileFrames(int& tot_file_frames /Out/) const;

	void getOpenFileName(std::string& file_name /Out/) const;
	bool isFileOpen() const;
};
