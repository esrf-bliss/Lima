#ifndef CTSAVING_H
#define CTSAVING_H

#include <map>
#include <list>
#include <string>
#include <fstream>

#include "ThreadUtils.h"

class Data;

namespace lima {

  class CtSaving {
    friend class CtControl;
  public:

    CtSaving(CtControl&);
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

    void _get_common_header(HeaderMap&);
    void _takeHeader(std::map<long,HeaderMap>::iterator&, HeaderMap& header);
    void _frame_ready(Data &);
    void _post_save_task(Data&,_SaveTask*);
    void _save_finnished(Data&);
 };

} // namespace lima

#endif // CTSAVING_H
