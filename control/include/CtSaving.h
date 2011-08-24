//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
#ifndef CTSAVING_H
#define CTSAVING_H

#include <map>
#include <list>
#include <string>
#include <fstream>
#include <ios>

#include "LimaCompatibility.h"
#include "ThreadUtils.h"
#include "CtControl.h"

struct Data;
class TaskEventCallback;
class SinkTaskBase;

namespace lima {

  class LIMACORE_API CtSaving 
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
	NXS
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

    struct LIMACORE_API Parameters 
    {
	  std::string directory;
      std::string prefix;
      std::string suffix;
	  ImageType   imageType;
      long nextNumber;
      FileFormat fileFormat;
      SavingMode savingMode;
      OverwritePolicy overwritePolicy;
      std::string indexFormat;
      long framesPerFile;
	  long nbframes;
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

    void frameReady(Data &,bool=false);
    void resetLastFrameNb();

    void setEndCallback(TaskEventCallback *);

    // --- statistic

    void getWriteTimeStatistic(std::list<double>&) const;
    void setStatisticHistorySize(int aSize);

    // --- misc

    void clear();
    void writeFrame(int frame_nr = -1);	///< frame_nr == -1 => last frame

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
      void getParameters(CtSaving::Parameters&) const;	  	  
      void clear();
      
      /** @brief should return true if container has compression or
       *  havy task to do before saving
       *  if return is true, getCompressionTask should return a Task
       * @see getCompressionTask
       */
      virtual bool needParralelCompression() const {return false;}
      /** @brief get a new compression task at each call.
       * this methode is not call if needParralelCompression return false
       *  @see needParralelCompression
       */
      virtual SinkTaskBase* getCompressionTask(const CtSaving::HeaderMap&) {return NULL;}

    protected:
      virtual bool _open(const std::string &filename,
		  std::ios_base::openmode flags) = 0;
      virtual void _close() = 0;
      virtual void _writeFile(Data &data,
			      CtSaving::HeaderMap &aHeader,
			      FileFormat) = 0;
      virtual void _clear() {};

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
    class _CompressionCBK;

    CtControl& 			m_ctrl;
    SaveContainer* 		m_save_cnt;
    _SaveCBK* 			m_saving_cbk;
    _CompressionCBK* 		m_compression_cbk;
    Parameters			m_pars;
    Parameters			m_acquisition_pars;
    bool			m_pars_dirty_flag;
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
    inline void _create_save_cnt();
    inline void _check_if_multi_frame_per_file_allowed(FileFormat,int) const;
    // --- internal call
    void _prepare();
    void _validate_parameters();
    void _check_write_access();
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
      case CtSaving::NXS:
	aFileFormatHumanPt = "NXS";break;	
      default:
	aFileFormatHumanPt = "RAW";break;
      }

    const char *aSavingModeHumanPt;
    switch(params.savingMode)
      {
      case CtSaving::AutoFrame:
	aSavingModeHumanPt = "Auto frame";break;
      case CtSaving::AutoHeader:
	aSavingModeHumanPt = "Auto header";break;
      default: //	Manual
	aSavingModeHumanPt = "Manual";break;
      }

    const char *anOverwritePolicyHumanPt;
    switch(params.overwritePolicy)
      {
      case CtSaving::Overwrite:
	anOverwritePolicyHumanPt = "Overwrite";break;
      case CtSaving::Append:
	anOverwritePolicyHumanPt = "Append";break;
      default:		// Abort
	  anOverwritePolicyHumanPt = "Abort";break;
      }

    os << "<"
       << "directory=" << params.directory << ", "
       << "prefix=" << params.prefix << ", "
       << "suffix=" << params.suffix << ", "
       << "nextNumber=" << params.nextNumber << ", "
       << "fileFormat=" << params.fileFormat << "," << aFileFormatHumanPt << ", "
       << "savingMode=" << params.savingMode << "," << aSavingModeHumanPt << ", "
       << "overwritePolicy=" << params.overwritePolicy << "," << anOverwritePolicyHumanPt << ", "
       << "framesPerFile=" << params.framesPerFile << ", "
	   << "nbframes=" << params.nbframes
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
