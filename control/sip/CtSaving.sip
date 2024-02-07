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



%MappedType HeaderMap
{
%TypeHeaderCode
#include <map>
#include <string>
typedef std::map<std::string,std::string> HeaderMap;
typedef std::pair<std::string, std::string> HeaderValue;
%End

%ConvertToTypeCode
   PyObject *key, *value;
   SIP_SSIZE_T pos;
   
   if (sipIsErr == NULL)
   {
       bool aReturnFlag = PyDict_Check(sipPy);
       pos = 0;
       while(aReturnFlag && PyDict_Next(sipPy, &pos, &key, &value))
           aReturnFlag = sipCheckConvertFromPyStr(key) && sipCheckConvertFromPyStr(value);

       return aReturnFlag;
   }

    *sipCppPtr = new HeaderMap();
    pos = 0;
    while (PyDict_Next(sipPy, &pos, &key, &value)) 
        (*sipCppPtr)->insert(HeaderValue(sipConvertFromPyStr(key),sipConvertFromPyStr(value)));

    return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
    PyObject* aReturnDict = PyDict_New();
    for(HeaderMap::iterator i = sipCpp->begin();i != sipCpp->end();++i)
	{
	  PyObject *key = sipConvertToPyStr(i->first);
	  PyObject *value = sipConvertToPyStr(i->second);
	  PyDict_SetItem(aReturnDict,key,value);
	  Py_DECREF(key); Py_DECREF(value);
	}
return aReturnDict;
%End

};

%MappedType HeaderValue
{
%TypeHeaderCode
#include <string>
#include <map>
typedef std::pair<std::string, std::string> HeaderValue;
%End

%ConvertToTypeCode
if (sipIsErr == NULL)
   {
     bool aCheckFlag = PySequence_Check(sipPy) && PySequence_Length(sipPy) == 2;
     if(aCheckFlag)
	{
	 PyObject *first = PySequence_GetItem(sipPy,0);
	 PyObject *second = PySequence_GetItem(sipPy,1);
	 aCheckFlag = (sipCheckConvertFromPyStr(first) && sipCheckConvertFromPyStr(second));
	 Py_DECREF(first);Py_DECREF(second);
	}
     return aCheckFlag;
   }
    PyObject *first = PySequence_GetItem(sipPy,0);
    PyObject *second = PySequence_GetItem(sipPy,1);
    *sipCppPtr = new HeaderValue(sipConvertFromPyStr(first),sipConvertFromPyStr(second));
    Py_DECREF(first);Py_DECREF(second);

    return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
PyObject *key = sipConvertToPyStr(sipCpp->first);
PyObject *value = sipConvertToPyStr(sipCpp->second);
PyObject* aReturnTuple = PyTuple_Pack(2,key,value);
Py_DECREF(key);Py_DECREF(value);

return aReturnTuple;
%End

};

class CtSaving 
{
%TypeHeaderCode
#include "lima/CtSaving.h"
#include <sstream>
using namespace lima;
%End
  public:

    CtSaving(CtControl&);
    ~CtSaving();

    enum ManagedMode
      {
	Software,
	Hardware
      };
  
    enum FileFormat {
	HARDWARE_SPECIFIC = -1,
	RAW,
	EDF,
	CBFFormat,
	NXS,
	FITS,
	EDFGZ,
	TIFFFormat,
	HDF5,
	EDFConcat,
	EDFLZ4,
	CBFMiniHeader,
	HDF5GZ,
	HDF5BS,
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
      MultiSet,
    };	

    struct Parameters {
      std::string directory;
      std::string prefix;
      std::string suffix;
      ImageType   imageType;
      long nextNumber;
      CtSaving::FileFormat fileFormat;
      CtSaving::SavingMode savingMode;
      CtSaving::OverwritePolicy overwritePolicy;
      bool useHwComp;
      std::string indexFormat;
      long framesPerFile;
      long nbframes;

      Parameters();
      void checkValid() const;

      SIP_PYOBJECT __repr__() const;
%MethodCode
      LIMA_REPR_CODE
%End
    };


    // --- file parameters

    void setParameters(const Parameters &pars, int stream_idx=0);
    void getParameters(Parameters& pars /Out/, int stream_idx=0) const;

    void setDirectory(const std::string &directory, int stream_idx=0);
    void getDirectory(std::string &directory /Out/, int stream_idx=0) const;

    void setPrefix(const std::string &prefix, int stream_idx=0);
    void getPrefix(std::string &prefix /Out/, int stream_idx=0) const;

    void setSuffix(const std::string &suffix, int stream_idx=0);
    void getSuffix(std::string &suffix /Out/, int stream_idx=0) const;

    void setNextNumber(long number, int stream_idx=0);
    void getNextNumber(long& number, int stream_idx=0) const;

    void setFormat(FileFormat format, int stream_idx=0);
    void getFormat(FileFormat &format /Out/, int stream_idx=0) const;

    void setFormatAsString(const std::string &format, int stream_idx=0);
    void getFormatAsString(std::string& format /Out/, int stream_idx=0) const;

    //void getFormatList(std::list<FileFormat> &format_list /Out/) const;
    void getFormatListAsString(std::list<std::string> &format_list /Out/) const;

    void setFormatSuffix(int stream_idx=0);

    // --- saving modes

    void setSavingMode(SavingMode mode);
    void getSavingMode(SavingMode& mode /Out/) const;

    void setOverwritePolicy(OverwritePolicy policy, int stream_idx=0);
    void getOverwritePolicy(OverwritePolicy& policy /Out/, 
                            int stream_idx=0) const;

    void setUseHwComp(bool  policy, int stream_idx=0);
    void getUseHwComp(bool& policy /Out/, int stream_idx=0) const;

    void setFramesPerFile(unsigned long frames_per_file, int stream_idx=0);
    void getFramesPerFile(unsigned long& frames_per_file /Out/, 
                         int stream_idx=0) const;

    void setManagedMode(ManagedMode mode);
    void getManagedMode(ManagedMode &mode /Out/) const;

    // --- ZBuffers
    
    void setZBufferParameters(const BufferHelper::Parameters& pars,
                              int stream_idx = 0);
    void getZBufferParameters(BufferHelper::Parameters& pars /Out/,
                              int stream_idx = 0);

    // --- common headers

    void resetCommonHeader();
    void setCommonHeader(const HeaderMap &header);
    void updateCommonHeader(const HeaderMap &header);
    void getCommonHeader(HeaderMap &header /Out/) const;

    void addToCommonHeader(const HeaderValue &value);

    // --- frame headers

    void updateFrameHeader(long frame_nr, const HeaderMap &header);
    void addToFrameHeader(long frame_nr,const HeaderValue &value);
    void validateFrameHeader(long frame_nr);
    void getFrameHeader(long frame_nr,HeaderMap &header /Out/) const;
    void takeFrameHeader(long frame_nr,HeaderMap &header /Out/);

    void removeFrameHeader(long frame_nr);
    void removeAllFrameHeaders();

    void frameReady(Data &);

    void setEndCallback(TaskEventCallback *);
    // --- statistic

    void getStatistic(std::list<double>& /Out/,
		      std::list<double>& /Out/, 
		      std::list<double>& /Out/, 
		      std::list<double>& /Out/, 
		      int stream_idx=0) const;

    void getStatisticCounters(double& /Out/,
			      double& /Out/,
			      double& /Out/,
			      double& /Out/,
			      int stream_idx=0) const;

    void setStatisticHistorySize(int aSize, int stream_idx=0);
    int getStatisticHistorySize(int stream_idx=0) const;

    void setEnableLogStat(bool enable, int stream_idx=0);
    void getEnableLogStat(bool &enable /Out/, int stream_idx=0) const;

    // --- misc

    void clear();
    void close();
    void writeFrame(int frame_nr = -1, int nb_frames = 1,
		    bool synchronous = true);

    void setStreamActive(int stream_idx, bool  active);
    void getStreamActive(int stream_idx, bool& active /Out/) const;

    void getMaxConcurrentWritingTask(int& nb_thread /Out/,int stream_idx = 0) const;
    void setMaxConcurrentWritingTask(int,int stream_idx = 0);
    
  private:
    CtSaving(const CtSaving&);
 };
