#ifndef CTSAVING_H
#define CTSAVING_H

#include <map>
#include <list>
#include <string>
#include <fstream>

#include "ThreadUtils.h"
#include "CtControl.h"

class Data;
class TaskEventCallback;

namespace lima {

  class CtSaving 
  {
    DEB_CLASS_NAMESPC(DebModControl,"Saving","Control");

    friend class CtControl;
  public:

    CtSaving(CtControl&);
    ~CtSaving();
  
    enum FileFormat 
      {
	RAW,
	EDF,
      };

    enum SavingMode 
      {
	Manual,
	AutoFrame,
	AutoHeader,
      };
	
    enum OverwritePolicy 
      {
	Abort,
	Overwrite,
	Append,
      };	

    struct Parameters 
    {
      std::string directory;
      std::string prefix;
      std::string suffix;
      long nextNumber;
      FileFormat fileFormat;
      SavingMode savingMode;
      OverwritePolicy overwritePolicy;
      long framesPerFile;
    };

    typedef std::pair<std::string, std::string> HeaderValue;
    typedef std::map<std::string,std::string> HeaderMap;
    typedef std::map<long,Data> FrameMap;

    // --- file parameters

    void setParameters(const Parameters &pars);
    void getParameters(Parameters& pars) const;

    void setDirectory(const std::string &directory);
    void getDirectory(std::string& directory) const;

    void setPrefix(const std::string &prefix);
    void getPrefix(std::string& prefix) const;

    void setSuffix(const std::string &suffix);
    void getSuffix(std::string& suffix) const;

    void setNextNumber(long number);
    void getNextNumber(long& number) const;

    void setFormat(const FileFormat &format);
    void getFormat(FileFormat& format) const;

    // --- saving modes

    void setSavingMode(SavingMode mode);
    void getSavingMode(SavingMode& mode) const;

    bool hasAutoSaveMode() { return m_pars.savingMode != Manual; };

    void setOverwritePolicy(OverwritePolicy policy);
    void getOverwritePolicy(OverwritePolicy& policy) const;

    void setFramesPerFile(unsigned long frames_per_file);
    void getFramePerFile(unsigned long& frames_per_file) const;

    // --- common headers

    void resetCommonHeader();
    void setCommonHeader(const HeaderMap &header);
    void updateCommonHeader(const HeaderMap &header);
    void getCommonHeader(HeaderMap& header) const;
    void addToCommonHeader(const HeaderValue &value);

    // --- frame headers

    void updateFrameHeader(long frame_nr, const HeaderMap &header);
    void addToFrameHeader(long frame_nr,const HeaderValue &value);
    void validateFrameHeader(long frame_nr);
    void getFrameHeader(long frame_nr, HeaderMap& header) const;
    void takeFrameHeader(long frame_nr, HeaderMap& header);

    void removeFrameHeader(long frame_nr);
    void removeAllFrameHeaders();

    void frameReady(Data &);
    void resetLastFrameNb();

    void setEndCallback(TaskEventCallback *);

    // --- statistic

    void getWriteTimeStatistic(std::list<double>&) const;
    void setStatisticHistorySize(int aSize);

    // --- misc

    void clear();

  private:
    class _SaveTask;
    class _SaveContainer;
    class _SaveCBK;
    friend class _SaveCBK;

    CtControl			&m_ctrl;
    _SaveContainer		*m_save_cnt;
    _SaveCBK			*m_saving_cbk;
    Parameters			m_pars;
    HeaderMap			m_common_header;
    std::map<long, HeaderMap>	m_frame_headers;
    FrameMap			m_frame_datas;

    mutable Cond		m_cond;
    bool			m_ready_flag;
    long			m_last_frameid_saved;
    
    TaskEventCallback		*m_end_cbk;

    void _get_common_header(HeaderMap&);
    void _takeHeader(std::map<long,HeaderMap>::iterator&, HeaderMap& header);
    void _post_save_task(Data&,_SaveTask*);
    void _save_finished(Data&);
  };
  inline std::ostream& operator<<(std::ostream &os,const CtSaving::Parameters &params)
  {
    os << "<"
       << "directory=" << params.directory << ", "
       << "prefix=" << params.prefix << ", "
       << "suffix=" << params.suffix << ", "
       << "nextNumber=" << params.nextNumber << ", "
       << "fileFormat=" << params.fileFormat << ", "
       << "savingMode=" << params.savingMode << ", "
       << "overwritePolicy=" << params.overwritePolicy << ", "
       << "framesPerFile=" << params.framesPerFile
       << ">";
    return os;
  }
  inline std::ostream& operator<<(std::ostream &os,const CtSaving::HeaderMap &header)
  {
    os << "< ";
    for(CtSaving::HeaderMap::const_iterator i = header.begin();
	i != header.end();++i)
      os << "(" << i->first << "," << i->second << ") ";
    os << ">";
    return os;
  }
  inline std::ostream& operator<<(std::ostream &os,const CtSaving::HeaderValue &value)
  {
    os << "< (" << value.first << "," << value.second << ") >";
    return os;
  }
} // namespace lima

#endif // CTSAVING_H
