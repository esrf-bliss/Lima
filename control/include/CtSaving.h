#ifndef CTSAVING_H
#define CTSAVING_H

namespace lima {

class CtSaving {

    public:

	CtSaving();
	~CtSaving();

	enum FileFormat {
		RAW,
		EDF,
	};

	enum SavingMode {
		Manual,
		AutoFrame,
		AutoHeader,
	};
	
	enum OverwritePolicy {
		Abort,
		Overwrite,
		Append,
	};	

	struct Parameters {
		string directory;
		string prefix;
		string suffix;
		long nextNumber;
		FileFormat fileFormat;
		SavingMode savingMode;
		OverwritePolicy overwritePolicy;
		unsigned long framesPerFile;
	};

	typedef std::pairs<string, string> HeaderValue;
	typedef std::list<HeaderValue> HeaderList;

	// --- file parameters

	void setParameters(Parameters pars);
	void getParameters(Parameters& pars) const;

	void setDirectory(string directory);
	void getDirectory(string& directory) const;

	void setPrefix(string prefix);
	void getPrefix(string& prefix) const;

	void setSuffix(string suffix);
	void getSuffix(string& suffix) const;

	void setNextNumber(long number);
	void getNextNumber(long& number) const;

	void setFormat(FileFormat format);
	void getFormat(FileFormat& format) const;

	// --- saving modes

	void setSavingMode(SavingMode mode);
	void getSavingMode(SavingMode& mode) const;

	void setOverwritePolicy(OverwritePolicy policy);
	void getOverwritePolicy(OverwritePolicy& policy) const;

	void setFramesPerFile(unsigned long frames_per_file);
	void getFramePerFile(unsigned long& frames_per_file) const;

	// --- common headers

	void getStaticHeader(HeaderList& header) const;

	void resetCommonHeader();
	void setCommonHeader(HeaderList header);
	void getCommonHeader(HeaderList& header) const;
	void addToCommonHeader(HeaderValue value);

	// --- frame headers

	void setFrameHeader(long frame_nr, HeaderList header);
	void getFrameHeader(long frame_nr, HeaderList& header) const;
	void takeFrameHeader(long frame_nr, HeaderList& header);
	void addToFrameHeader(long frame_nr, HeaderValue value);

	void removeFrameHeader(long frame_nr);
	void removeAllFrameHeaders();

	// --- final header

	void takeHeader(long frame_nr, HeaderList& header);
	
    private:

	Parameters m_pars;
	HeaderList m_static_header;
	HeaderList m_common_header;
	map<long, HeaderList> m_frame_headers;
};

} // namespace lima

#undef // CTSAVING_H
