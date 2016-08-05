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
if (sipIsErr == NULL)
   return PyDict_Check(sipPy);

    *sipCppPtr = new HeaderMap();
    PyObject *key, *value;
    SIP_SSIZE_T pos = 0;

    while (PyDict_Next(sipPy, &pos, &key, &value)) 
    {
	if(PyString_Check(key) && PyString_Check(value))
	   (*sipCppPtr)->insert(HeaderValue(PyString_AS_STRING(key),PyString_AS_STRING(value)));
    }

    return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
    PyObject* aReturnDict = PyDict_New();
    for(HeaderMap::iterator i = sipCpp->begin();i != sipCpp->end();++i)
	{
	  PyObject *value = PyString_FromString(i->second.c_str());
	  PyDict_SetItemString(aReturnDict,i->first.c_str(),value);
	  Py_DECREF(value);
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
	 aCheckFlag = (PyString_Check(first) && PyString_Check(second));
	 Py_DECREF(first);Py_DECREF(second);
	}
     return aCheckFlag;
   }
    PyObject *first = PySequence_GetItem(sipPy,0);
    PyObject *second = PySequence_GetItem(sipPy,1);
    *sipCppPtr = new HeaderValue(PyString_AS_STRING(first),PyString_AS_STRING(second));
    Py_DECREF(first);Py_DECREF(second);

    return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
PyObject *key = PyString_FromString(sipCpp->first.c_str());
PyObject *value = PyString_FromString(sipCpp->second.c_str());
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
      std::string indexFormat;
      long framesPerFile;
      long nbframes;

      Parameters();
      void checkValid() const;

      const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	const std::string& tmpString = str.str();
	sipRes = tmpString.c_str();
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

    // --- saving modes

    void setSavingMode(SavingMode mode);
    void getSavingMode(SavingMode& mode /Out/) const;

    void setOverwritePolicy(OverwritePolicy policy, int stream_idx=0);
    void getOverwritePolicy(OverwritePolicy& policy /Out/, 
                            int stream_idx=0) const;

    void setFramesPerFile(unsigned long frames_per_file, int stream_idx=0);
    void getFramesPerFile(unsigned long& frames_per_file /Out/, 
                         int stream_idx=0) const;

    void setManagedMode(ManagedMode mode);
    void getManagedMode(ManagedMode &mode /Out/) const;
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
    void resetLastFrameNb();

    void setEndCallback(TaskEventCallback *);
    // --- statistic

    void getWriteTimeStatistic(std::list<double>& /Out/, 
                               int stream_idx=0) const;
    void setStatisticHistorySize(int aSize, int stream_idx=0);

    // --- misc

    void clear();
    void close();
    void writeFrame(int frame_nr = -1, int nb_frames = 1,
		    bool synchronous = true);

    void setStreamActive(int stream_idx, bool  active);
    void getStreamActive(int stream_idx, bool& active /Out/) const;
 };
