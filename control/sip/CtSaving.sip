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
    Parameters getParameters() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipRes = new CtSaving::Parameters();
sipCpp->getParameters(*sipRes);
Py_END_ALLOW_THREADS
%End

    void setDirectory(const std::string &directory);
    std::string getDirectory() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipRes = new std::string();
sipCpp->getDirectory(*sipRes);
Py_END_ALLOW_THREADS
%End

    void setPrefix(const std::string &prefix);
    std::string getPrefix() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipRes = new std::string();
sipCpp->getPrefix(*sipRes);
Py_END_ALLOW_THREADS
%End
    void setSuffix(const std::string &suffix);
    std::string getSuffix() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipRes = new std::string();
sipCpp->getSuffix(*sipRes);
Py_END_ALLOW_THREADS
%End

    void setNextNumber(long number);
    long getNextNumber() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getNextNumber(sipRes);
Py_END_ALLOW_THREADS
%End
    void setFormat(const FileFormat &format);
    FileFormat getFormat() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getFormat(sipRes);
Py_END_ALLOW_THREADS
%End

    // --- saving modes

    void setSavingMode(SavingMode mode);
    SavingMode getSavingMode() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getSavingMode(sipRes);
Py_END_ALLOW_THREADS
%End

    void setOverwritePolicy(OverwritePolicy policy);
    OverwritePolicy getOverwritePolicy() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getOverwritePolicy(sipRes);
Py_END_ALLOW_THREADS
%End

    void setFramesPerFile(unsigned long frames_per_file);
    unsigned long getFramePerFile() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getFramePerFile(sipRes);
Py_END_ALLOW_THREADS
%End
    // --- common headers

    void resetCommonHeader();
    void setCommonHeader(const HeaderMap &header);
    void updateCommonHeader(const HeaderMap &header);
    HeaderMap getCommonHeader() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipRes = new HeaderMap();
sipCpp->getCommonHeader(*sipRes);
Py_END_ALLOW_THREADS
%End

    void addToCommonHeader(const HeaderValue &value);

    // --- frame headers

    void updateFrameHeader(long frame_nr, const HeaderMap &header);
    void addToFrameHeader(long frame_nr,const HeaderValue &value);
    void validateFrameHeader(long frame_nr);
    HeaderMap getFrameHeader(long frame_nr) const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipRes = new HeaderMap();
sipCpp->getFrameHeader(a0,*sipRes);
Py_END_ALLOW_THREADS
%End
    HeaderMap takeFrameHeader(long frame_nr);
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipRes = new HeaderMap();
sipCpp-> takeFrameHeader(a0,*sipRes);
Py_END_ALLOW_THREADS
%End

    void removeFrameHeader(long frame_nr);
    void removeAllFrameHeaders();

    void frameReady(Data &);
 };
