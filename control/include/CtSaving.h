#ifndef CTSAVING_H
#define CTSAVING_H

#include <map>
#include <list>
#include <string>
#include <fstream>
#include <ios>

#include "Compatibility.h"
#include "ThreadUtils.h"
#include "CtControl.h"

struct Data;
class TaskEventCallback;

namespace lima {

  class DLL_EXPORT CtSaving 
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
	CBFFormat,
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

    struct DLL_EXPORT Parameters 
    {
      std::string directory;
      std::string prefix;
      std::string suffix;
      long nextNumber;
      FileFormat fileFormat;
      SavingMode savingMode;
      OverwritePolicy overwritePolicy;
      std::string indexFormat;
      long framesPerFile;
      Parameters();
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

    void setFormat(FileFormat format);
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

    class SaveContainer
    {
      DEB_CLASS_NAMESPC(DebModControl,"Saving Container","Control");
    public:
      SaveContainer(CtSaving&);
      virtual ~SaveContainer();

      void open(const CtSaving::Parameters&);
      void close();
      void writeFile(Data&,CtSaving::HeaderMap &);
      void setStatisticSize(int aSize);
      void getStatistic(std::list<double>&) const;
      void clear();

    protected:
      virtual bool _open(const std::string &filename,
		  std::ios_base::openmode flags) = 0;
      virtual void _close() = 0;
      virtual void _writeFile(Data &data,
			      CtSaving::HeaderMap &aHeader,
			      FileFormat) = 0;

      int			m_written_frames;
    private:
      CtSaving			&m_saving;
      std::list<double>		m_statistic_list;
      int			m_statistic_size;
      mutable Cond		m_cond;
      bool			m_file_opened;

    };
    friend class SaveContainer;

  private:
    class _SaveTask;
    class _SaveCBK;
    friend class _SaveCBK;

    CtControl			&m_ctrl;
    SaveContainer		*m_save_cnt;
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
    void _setSavingError(CtControl::ErrorCode);
    inline void _create_save_cnt(FileFormat);
    inline void _check_if_multi_frame_per_file_allowed(FileFormat,int) const;
  };
  inline std::ostream& operator<<(std::ostream &os,const CtSaving::Parameters &params)
  {
    const char *aFileFormatHumanPt;
    switch(params.fileFormat)
      {
      case CtSaving::EDF:
	aFileFormatHumanPt = "EDF";break;
      case CtSaving::CBFFormat:
	aFileFormatHumanPt = "CBF";break;
      default:
	aFileFormatHumanPt = "RAW";break;
      }
    os << "<"
       << "directory=" << params.directory << ", "
       << "prefix=" << params.prefix << ", "
       << "suffix=" << params.suffix << ", "
       << "nextNumber=" << params.nextNumber << ", "
       << "fileFormat=" << params.fileFormat << "," << aFileFormatHumanPt << ", "
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
