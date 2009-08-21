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
    Py_ssize_t pos = 0;

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
#include <CtSaving.h>
using namespace lima;
%End
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
      CtSaving::FileFormat fileFormat;
      CtSaving::SavingMode savingMode;
      CtSaving::OverwritePolicy overwritePolicy;
      long framesPerFile;
    };


    // --- file parameters

    void setParameters(const Parameters &pars);
    void getParameters(Parameters& pars /Out/) const;

    void setDirectory(const std::string &directory);
    void getDirectory(std::string &directory /Out/) const;

    void setPrefix(const std::string &prefix);
    void getPrefix(std::string &prefix /Out/) const;

    void setSuffix(const std::string &suffix);
    void getSuffix(std::string &suffix /Out/) const;

    void setNextNumber(long number);
    void getNextNumber(long& number) const;

    void setFormat(const FileFormat &format);
    void getFormat(FileFormat &format /Out/) const;

    // --- saving modes

    void setSavingMode(SavingMode mode);
    void getSavingMode(SavingMode& mode /Out/) const;

    void setOverwritePolicy(OverwritePolicy policy);
    void getOverwritePolicy(OverwritePolicy& policy /Out/) const;

    void setFramesPerFile(unsigned long frames_per_file);
    void getFramePerFile(unsigned long& frames_per_file /Out/) const;

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

    //void getWriteTimeStatistic(std::list<double>& /Out/) const;
    void setStatisticHistorySize(int aSize);

    // --- misc

    void clear();
 };
