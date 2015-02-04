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
#include <algorithm>

#include "LimaCompatibility.h"
#include "ThreadUtils.h"
#include "CtControl.h"
#include "CtConfig.h"
#include "HwSavingCtrlObj.h"

struct Data;
class TaskEventCallback;
class SinkTaskBase;

namespace lima {
  /** @brief Saving management
   *
   * With this class you manage the image saving in different format
   */
  class LIMACORE_API CtSaving 
  {
    DEB_CLASS_NAMESPC(DebModControl,"Saving","Control");

    friend class CtControl;
  public:
    CtSaving(CtControl&);
    ~CtSaving();

    enum ManagedMode
      {
	Software,		///< Saving will be managed by Lima Core (Control)
	Hardware,		///< Saving will be managed by Hardware or Camera SDK
	Camera			///< Saving will be managed by the Camera plugin
      };

    enum FileFormat 
      {
	HARDWARE_SPECIFIC = -1,	///< extended hardware format (ADSC,MarCCD...) @see setHardwareFormat
	RAW,			///< Raw format (no header)
	EDF,			///< EDF format (Esrf Data Format)
	CBFFormat,		///< CBF format
	NXS,			///< Soleil Nexus format
	FITS,			///< Flexible Image Transport Layer (NOST)
	EDFGZ,			///< EDF format with gzip compression
	TIFFFormat,		///< TIFF format
	HDF5,			///< HDF5 format
      };

    enum SavingMode 
      {
	Manual,			///< No automatic saving, you should call CtSaving::writeFrame
	AutoFrame,		///< Save a frame just after it acquisition
	AutoHeader,		///< Save the frame if header and the data of the frame is available 
      };
	
    enum OverwritePolicy 
      {
	Abort,			///< Abort acquisition if file already exist
	Overwrite,		///< Overwrite old files
	Append,			///< Append new data at the end of already existing files
	MultiSet,		///< Like append but doesn't use file counter
      };	

    struct LIMACORE_API Parameters 
    {
      DEB_CLASS_NAMESPC(DebModControl,"Saving::Parameters","Control");
    public:
      std::string directory;	///< base path where the files will be saved
      std::string prefix;	///< prefix of the filename
      std::string suffix;	///< suffix of the filename
      ImageType   imageType;
      long nextNumber;		///< next file number
      FileFormat fileFormat;	///< the saving format (EDF,CBF...)
      SavingMode savingMode;	///< saving mode (automatic,manual...)
      OverwritePolicy overwritePolicy; ///< how you the saving react it find existing filename
      std::string indexFormat;	///< ie: %.4d if you want 4 digits
      long framesPerFile;	///< the number of images save in one files
      long nbframes;
      
      Parameters();
      void checkValid() const;
    };
    
    typedef std::pair<std::string, std::string> HeaderValue;
    typedef std::map<std::string,std::string> HeaderMap;
    typedef std::map<long,Data> FrameMap;

    // --- file parameters

    void setParameters(const Parameters &pars, int stream_idx=0);
    void getParameters(Parameters& pars, int stream_idx=0) const;

    void setDirectory(const std::string &directory, int stream_idx=0);
    void getDirectory(std::string& directory, int stream_idx=0) const;

    void setPrefix(const std::string &prefix, int stream_idx=0);
    void getPrefix(std::string& prefix, int stream_idx=0) const;

    void setSuffix(const std::string &suffix, int stream_idx=0);
    void getSuffix(std::string& suffix, int stream_idx=0) const;
    
    void setNextNumber(long number, int stream_idx=0);
    void getNextNumber(long& number, int stream_idx=0) const;

    void setFormat(FileFormat format, int stream_idx=0);
    void getFormat(FileFormat& format, int stream_idx=0) const;

    void getHardwareFormatList(std::list<std::string> &format_list) const;
    void setHardwareFormat(const std::string &format);
    void getHardwareFormat(std::string &format) const;
    // --- saving modes

    void setSavingMode(SavingMode mode);
    void getSavingMode(SavingMode& mode) const;

    bool hasAutoSaveMode()
    { return m_stream[0]->hasAutoSaveMode(); }

    void setOverwritePolicy(OverwritePolicy policy, int stream_idx=0);
    void getOverwritePolicy(OverwritePolicy& policy, int stream_idx=0) const;

    void setFramesPerFile(unsigned long frames_per_file, int stream_idx=0);
    void getFramesPerFile(unsigned long& frames_per_file, 
			 int stream_idx=0) const;
    
    void setManagedMode(ManagedMode mode);
    void getManagedMode(ManagedMode &mode) const;
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

    void frameReady(Data&);
    void resetLastFrameNb();

    void setEndCallback(TaskEventCallback *);

    // --- internal common header
    void resetInternalCommonHeader();
    void addToInternalCommonHeader(const HeaderValue& value);
    template<class T>
      void addToInternalCommonHeader(const std::string &key,
				     const T&);
    // --- statistic

    void getWriteTimeStatistic(std::list<double>&, int stream_idx=0) const;
    void setStatisticHistorySize(int aSize, int stream_idx=0);

    // --- misc

    void clear();
    void close();
    //                  frame_nr == -1 => last frame
    void writeFrame(int frame_nr = -1, int nb_frames = 1,bool synchronous = true); 


    void setStreamActive(int stream_idx, bool  active);
    void getStreamActive(int stream_idx, bool& active) const;

    class Stream;

    class LIMACORE_API SaveContainer
    {
      DEB_CLASS_NAMESPC(DebModControl,"Saving Container","Control");
    public:
      SaveContainer(Stream& stream);
      virtual ~SaveContainer();
      
      void open(const CtSaving::Parameters&);
      void close();
      void writeFile(Data&,CtSaving::HeaderMap &);
      void setStatisticSize(int aSize);
      void getStatistic(std::list<double>&) const;
      void getParameters(CtSaving::Parameters&) const;
      void clear();
      void prepare(CtControl&);
      /** @brief should return true if container has compression or
       *  havy task to do before saving
       *  if return is true, getCompressionTask should return a Task
       * @see getCompressionTask
       */
      virtual bool needParallelCompression() const {return false;}
      /** @brief get a new compression task at each call.
       * this methode is not call if needParallelCompression return false
       *  @see needParallelCompression
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
      virtual void _prepare(CtControl&) {};

      int			m_written_frames;
      Stream			&m_stream;
    private:
      std::list<double>		m_statistic_list;
      int			m_statistic_size;
      mutable Cond		m_cond;
      bool			m_file_opened;
      long			m_nb_frames_to_write;
    };
    friend class SaveContainer;

    enum ParameterType {
      Acq, Cache, Auto, 
    };

    enum TaskType {
      Save, Compression,
    };

    class LIMACORE_API Stream 
    {
      DEB_CLASS(DebModControl, "CtSaving::Stream");

    public:
      Stream(CtSaving& aCtSaving, int idx);
      ~Stream();

      int getIndex() const
      { return m_idx; }

      const Parameters& getParameters(ParameterType type) const;
      Parameters& getParameters(ParameterType type);
      void setParameters(const Parameters& pars);
      void updateParameters();

      void prepare(CtControl& ct);
      void close();
      void createSaveContainer();
      void checkWriteAccess();
      void checkDirectoryAccess(const std::string&);

      bool needCompression()
      { return m_save_cnt->needParallelCompression(); }

      void setSavingError(CtControl::ErrorCode error)
      { m_saving._setSavingError(error); }

      SinkTaskBase *getTask(TaskType type, const HeaderMap& header);

      void compressionFinished(Data& data);
      void saveFinished(Data& data);
      int getNextNumber() const;

      bool isActive() const
      { return m_active; }
      void setActive(bool active);

      void writeFile(Data& data, HeaderMap& header);

      bool hasAutoSaveMode()
      { const Parameters& pars = getParameters(Cache);
	return pars.savingMode != Manual; 
      }

      void getStatistic(std::list<double>& stat_list) const
      { m_save_cnt->getStatistic(stat_list); }
      void setStatisticSize(int size) 
      { m_save_cnt->setStatisticSize(size); }

      void clear()
      { m_save_cnt->clear(); }

    private:
      class _SaveCBK;
      class _SaveTask;
      class _CompressionCBK;

      CtSaving&			m_saving;
      int			m_idx;

      SaveContainer 	       *m_save_cnt;
      _SaveCBK	 	       *m_saving_cbk;
      Parameters		m_pars;
      Parameters		m_reference_pars;
      Parameters		m_acquisition_pars;
      bool			m_pars_dirty_flag;
      bool			m_active;
      _CompressionCBK 	       *m_compression_cbk;
    };
    friend class Stream;

  private:
    class	_ManualBackgroundSaveTask;
    friend class _ManualBackgroundSaveTask;
    class	_NewFrameSaveCBK;
    friend class _NewFrameSaveCBK;
    typedef std::vector<SinkTaskBase *> TaskList;
    typedef std::map<long, long>	FrameCbkCountMap;
    typedef std::map<long, HeaderMap>	FrameHeaderMap;

    void _validateFrameHeader(long frame_nr,
			      AutoMutex&);

    CtControl& 			m_ctrl;

    int				m_nb_stream;
    Stream		      **m_stream;

    HeaderMap			m_common_header;
    HeaderMap			m_internal_common_header;
    FrameHeaderMap		m_frame_headers;
    FrameMap			m_frame_datas;

    mutable Cond		m_cond;
    bool			m_ready_flag;
    long			m_last_frameid_saved;
    bool			m_need_compression;
    FrameCbkCountMap		m_nb_compression_cbk;
    int				m_nb_save_cbk;
    TaskEventCallback	       *m_end_cbk;
    bool			m_has_hwsaving;
    HwSavingCtrlObj*		m_hwsaving;
    _NewFrameSaveCBK*		m_new_frame_save_cbk;
    ManagedMode			m_managed_mode;	///< two option either harware (manage by SDK,hardware) or software (Lima core)
    std::string			m_specific_hardware_format;
    bool			m_saving_stop;

      Stream& getStream(int stream_idx)
	{ bool stream_ok = (stream_idx >= 0) && (stream_idx < m_nb_stream);
	  return stream_ok ? *m_stream[stream_idx] : getStreamExc(stream_idx); }

      const Stream& getStream(int stream_idx) const
	{ bool stream_ok = (stream_idx >= 0) && (stream_idx < m_nb_stream);
	  return stream_ok ? *m_stream[stream_idx] : getStreamExc(stream_idx); }

      Stream& getStreamExc(int stream_idx) const;

      SavingMode getAcqSavingMode() const
      { return getStream(0).getParameters(Acq).savingMode; }

      ManagedMode getManagedMode() const
      { return m_managed_mode; }

      // --- from control
      void getSaveCounters(int& first_to_save, int& last_to_save)
      { AutoMutex lock(m_cond.mutex());
	first_to_save = last_to_save = -1;
	FrameMap::const_iterator it, end = m_frame_datas.end();
	for (it = m_frame_datas.begin(); it != end; ++it) {
	  if (it->first > last_to_save)
	    last_to_save = it->first;
	  if ((first_to_save == -1) || (it->first < first_to_save))
	    first_to_save = it->first;
	}
      }

      // --- internal call
      void _prepare(CtControl&);
      void _stop(CtControl&);
      void _close();
      void _getCommonHeader(HeaderMap&);
      void _takeHeader(FrameHeaderMap::iterator&, HeaderMap& header,
		       bool keep_in_map);
      void _getTaskList(TaskType type, long frame_nr, const HeaderMap& header, 
			TaskList& task_list);
      void _postTaskList(Data&, const TaskList&);
      void _compressionFinished(Data&, Stream&);
      void _saveFinished(Data&, Stream&);
      void _setSavingError(CtControl::ErrorCode);
      void _updateParameters();
      void _synchronousSaving(Data&,HeaderMap&);
      bool _controlIsFault();
      bool _newFrameWrite(int);
      bool _checkHwFileFormat(const std::string&) const;
      void _ReadImage(Data&,int framenb);

#ifdef WITH_CONFIG
      class _ConfigHandler;
      CtConfig::ModuleTypeCallback* _getConfigHandler();
#endif //WITH_CONFIG
  };

  inline const char* convert_2_string(CtSaving::FileFormat fileFormat)
    {
      const char *aFileFormatHumanPt;
      switch(fileFormat)
	{
	case CtSaving::EDF:
	  aFileFormatHumanPt = "EDF";break;
	case CtSaving::CBFFormat:
	  aFileFormatHumanPt = "CBF";break;
	case CtSaving::NXS:
	  aFileFormatHumanPt = "NXS";break;
	case CtSaving::FITS:
	  aFileFormatHumanPt = "FITS";break;
	case CtSaving::EDFGZ:
	  aFileFormatHumanPt = "EDF gzip";break;
	case CtSaving::TIFFFormat:
	  aFileFormatHumanPt = "TIFF";break;
	case CtSaving::HDF5:
	  aFileFormatHumanPt = "HDF5";break;
	default:
	  aFileFormatHumanPt = "RAW";break;
	}
      return aFileFormatHumanPt;
    }
  inline void convert_from_string(const std::string& val,
				  CtSaving::FileFormat& fileFormat)
    {
      std::string buffer = val;
      std::transform(buffer.begin(),buffer.end(),
		     buffer.begin(),::tolower);

      if(buffer == "edf") 		fileFormat = CtSaving::EDF;
      else if(buffer == "cbf") 		fileFormat = CtSaving::CBFFormat;
      else if(buffer == "nxs") 		fileFormat = CtSaving::NXS;
      else if(buffer == "fits")		fileFormat = CtSaving::FITS;
      else if(buffer == "edf gzip") 	fileFormat = CtSaving::EDFGZ;
      else if(buffer == "raw")		fileFormat = CtSaving::RAW;
      else if(buffer == "tiff")		fileFormat = CtSaving::TIFFFormat;
      else if(buffer == "hdf5")		fileFormat = CtSaving::HDF5;
      else
	{
	  std::ostringstream msg;
	  msg << "FileFormat can't be:" << DEB_VAR1(val);
	  throw LIMA_EXC(Control,InvalidValue,msg.str());
	}

    }
  inline const char* convert_2_string(CtSaving::SavingMode savingMode)
    {
      const char *aSavingModeHumanPt;
      switch(savingMode)
	{
	case CtSaving::AutoFrame:
	  aSavingModeHumanPt = "Auto frame";break;
	case CtSaving::AutoHeader:
	  aSavingModeHumanPt = "Auto header";break;
	default: //	Manual
	  aSavingModeHumanPt = "Manual";break;
	}
      return aSavingModeHumanPt;
    }
  inline void convert_from_string(const std::string& val,
				  CtSaving::SavingMode& savingMode)
    {
      std::string buffer = val;
      std::transform(buffer.begin(),buffer.end(),
		     buffer.begin(),::tolower);

      if(buffer == "auto frame") 	savingMode = CtSaving::AutoFrame;
      else if(buffer == "auto header") 	savingMode = CtSaving::AutoHeader;
      else if(buffer == "manual") 	savingMode = CtSaving::Manual;
      else
	{
	  std::ostringstream msg;
	  msg << "SavingMode can't be:" << DEB_VAR1(val);
	  throw LIMA_EXC(Control,InvalidValue,msg.str());
	}

    }
  inline const char* convert_2_string(CtSaving::OverwritePolicy overwritePolicy)
    {
      const char *anOverwritePolicyHumanPt;
      switch(overwritePolicy)
	{
	case CtSaving::Overwrite:
	  anOverwritePolicyHumanPt = "Overwrite";break;
	case CtSaving::Append:
	  anOverwritePolicyHumanPt = "Append";break;
	case CtSaving::MultiSet:
	  anOverwritePolicyHumanPt = "MultiSet";break;
	default:		// Abort
	  anOverwritePolicyHumanPt = "Abort";break;
	}
      return anOverwritePolicyHumanPt;
    }
  inline void convert_from_string(const std::string& val,
				  CtSaving::OverwritePolicy& overwritePolicy)
    {
      std::string buffer = val;
      std::transform(buffer.begin(),buffer.end(),
		     buffer.begin(),::tolower);

      if(buffer == "overwrite") 	overwritePolicy = CtSaving::Overwrite;
      else if(buffer == "append") 	overwritePolicy = CtSaving::Append;
      else if(buffer == "abort") 	overwritePolicy = CtSaving::Abort;
      else if(buffer == "multiset")	overwritePolicy = CtSaving::MultiSet;
      else
	{
	  std::ostringstream msg;
	  msg << "OverwritePolicy can't be:" << DEB_VAR1(val);
	  throw LIMA_EXC(Control,InvalidValue,msg.str());
	}
    }
  inline const char* convert_2_string(CtSaving::ManagedMode manageMode)
    {
      const char* aManagedModeHumanPt;
      switch(manageMode)
	{
	case CtSaving::Hardware:
	  aManagedModeHumanPt = "Hardware";break;
	default:
	  aManagedModeHumanPt = "Software";break;
	}
      return aManagedModeHumanPt;
    }
  inline void convert_from_string(const std::string& val,
				  CtSaving::ManagedMode& manageMode)
    {
      std::string buffer = val;
      std::transform(buffer.begin(),buffer.end(),
		     buffer.begin(),::tolower);

      if(buffer == "hardware") 		manageMode = CtSaving::Hardware;
      else if(buffer == "software") 	manageMode = CtSaving::Software;
      else
	{
	  std::ostringstream msg;
	  msg << "ManagedMode can't be:" << DEB_VAR1(val);
	  throw LIMA_EXC(Control,InvalidValue,msg.str());
	}

    }
   inline std::ostream& operator<<(std::ostream &os,const CtSaving::Parameters &params)
    {
      const char *aFileFormatHumanPt = convert_2_string(params.fileFormat);
      const char *aSavingModeHumanPt = convert_2_string(params.savingMode);
      const char *anOverwritePolicyHumanPt = convert_2_string(params.overwritePolicy);

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

  inline bool operator ==(const CtSaving::Parameters& a,
			  const CtSaving::Parameters& b)
    {
      return ((a.directory       == b.directory)       &&
	      (a.prefix          == b.prefix)          &&
	      (a.suffix          == b.suffix)          &&
	      (a.imageType       == b.imageType)       &&
	      (a.nextNumber      == b.nextNumber)      &&
	      (a.fileFormat      == b.fileFormat)      &&
	      (a.savingMode      == b.savingMode)      &&
	      (a.overwritePolicy == b.overwritePolicy) &&
	      (a.indexFormat     == b.indexFormat)     &&
	      (a.framesPerFile   == b.framesPerFile)   &&
	      (a.nbframes        == b.nbframes));
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

  template<class T>
    void CtSaving::addToInternalCommonHeader(const std::string &key,
					     const T& obj)
    {
      AutoMutex aLock(m_cond.mutex());
      std::ostringstream str;
      str << obj;	
      const std::string& value = str.str();
      HeaderValue anEntry(key,value);
      m_internal_common_header.insert(anEntry);
    }

} // namespace lima

#endif // CTSAVING_H
