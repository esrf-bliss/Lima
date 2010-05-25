%MappedType std::list<SoftOpRoiCounter::RoiIdAndResults>
{
%TypeHeaderCode
#include <list>
#include <RoiCounter.h>
using namespace lima;
%End

%ConvertToTypeCode
if(sipIsErr == NULL)
{
 bool aReturnFlag = PyList_Check(sipPy);
 for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
 {
  PyObject *p = PyList_GET_ITEM(sipPy, i);
  aReturnFlag = PySequence_Check(p) && 
                PySequence_Size(p) == 2 &&
    	        PyInt_Check(PySequence_Fast_GET_ITEM(p,0)) &&
                PyList_Check(PySequence_Fast_GET_ITEM(p,1));
  if(aReturnFlag)
    {
      PyObject *aTmpList = PySequence_Fast_GET_ITEM(p,1);
      for(int k = 0;aReturnFlag && k < PyList_Size(aTmpList);++k)
	aReturnFlag = sipCanConvertToType(PyList_GET_ITEM(aTmpList, k),
					  sipType_Tasks_RoiCounterResult, SIP_NOT_NONE);
    }
 }
 return aReturnFlag;
}

std::list<SoftOpRoiCounter::RoiIdAndResults> *roiIdNResultPt = 
	new std::list<SoftOpRoiCounter::RoiIdAndResults>();

for(int i = 0;i < PyList_Size(sipPy);++i)
{
 PyObject *p = PyList_GET_ITEM(sipPy, i);
 int anId = PyInt_AsLong(PySequence_Fast_GET_ITEM(p,0));
 PyObject *aTmpList = PySequence_Fast_GET_ITEM(p,1);
 
 roiIdNResultPt->push_back(SoftOpRoiCounter::RoiIdAndResults(anId,std::list<Tasks::RoiCounterResult>()));
 for(int k = 0;k < PyList_Size(aTmpList);++k)
   {
     int state;
     Tasks::RoiCounterResult *aRoiCounterResult = 
       reinterpret_cast<Tasks::RoiCounterResult*>(sipConvertToType(PySequence_Fast_GET_ITEM(p,1),
								   sipType_Tasks_RoiCounterResult, 0,	
								   SIP_NOT_NONE,
								   &state, sipIsErr));
     // Deal with any errors.
     if (*sipIsErr)
       {
	 sipReleaseType(aRoiCounterResult, sipType_Tasks_RoiCounterResult, state);
	 
	 // Tidy up.
	 delete roiIdNResultPt;

	 // There is no temporary instance.
	 return 0;
       }
     
     roiIdNResultPt->back().second.push_back(*aRoiCounterResult);
     sipReleaseType(aRoiCounterResult, sipType_Tasks_RoiCounterResult, state);
   }
}
*sipCppPtr = roiIdNResultPt;
return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
 PyObject *l;

 // Create the Python list of the correct length.
 if (!(l = PyList_New(sipCpp->size())))
  return NULL;

 int i = 0;
 for (std::list<SoftOpRoiCounter::RoiIdAndResults>::iterator j = sipCpp->begin();
      j != sipCpp->end();++j,++i)
  {
    int m = 0;
    std::list<Tasks::RoiCounterResult> &aRoiCounterResultList = (*j).second;
    PyObject *PyRoiResultList = PyList_New(aRoiCounterResultList.size());
    for(std::list<Tasks::RoiCounterResult>::iterator k = aRoiCounterResultList.begin();
        k != aRoiCounterResultList.end();++k,++m)
    {
     PyObject *wobj;

     // Get the Python wrapper for the QWidget instance, creating a new
     // one if necessary, and handle any ownership transfer.
     if (!(wobj = sipConvertFromType(&(*k), sipType_Tasks_RoiCounterResult, sipTransferObj)))
     {
      // There was an error so garbage collect the Python lists.
      Py_DECREF(PyRoiResultList);
      Py_DECREF(l);
      return NULL;
     }
     PyList_SET_ITEM(PyRoiResultList,m,wobj);
    }

   PyObject *anIndexPy = PyInt_FromLong((*j).first);
   PyList_SET_ITEM(l, i, PyTuple_Pack(2,anIndexPy,PyRoiResultList));
   Py_DECREF(anIndexPy);Py_DECREF(PyRoiResultList);
  }

 // Return the Python list.
 return l;
%End
};

enum SoftOpId
{
 UNDEF,
 BACKGROUNDSUBSTRACTION,
 BINNING,
 BPM,
 FLATFIELDCORRECTION,
 FLIP,
 MASK,
 ROICOUNTERS,
 SOFTROI
};

struct SoftOpKey
{
%TypeHeaderCode
#include <SoftOpId.h>
%End
 SoftOpKey();
 SoftOpKey(SoftOpId id,const char *name);
 
 SoftOpId m_id;
 const char *m_name;
};

class SoftOpBackgroundSubstraction
{
%TypeHeaderCode
#include <SoftOpId.h>
using namespace lima;
%End
 public:
  SoftOpBackgroundSubstraction();
  ~SoftOpBackgroundSubstraction();
    
  void setBackgroundImage(Data &aData);
    
 protected:
  virtual void addTo(TaskMgr&,int stage);
  virtual void prepare();
};

class SoftOpRoiCounter
{
%TypeHeaderCode
#include <SoftOpId.h>
using namespace lima;
%End
 public:
  typedef std::pair<int,std::list<Tasks::RoiCounterResult> > RoiIdAndResults;

  SoftOpRoiCounter();
  ~SoftOpRoiCounter();

  void add(const std::list<Roi> &rois); 
  void set(const std::list<Roi> &rois); 
  void get(std::list<Roi> &rois /Out/) const;
  void del(const std::list<int> &roiIds);
  void clearAllRoi();
    
  void setMask(Data &aMask);

  void setBufferSize(int size);
  void getBufferSize(int &size /Out/) const;
    
  void readCounters(int from,std::list<SoftOpRoiCounter::RoiIdAndResults> &result /Out/) const;
 protected:
  virtual void addTo(TaskMgr&,int stage);
  virtual void prepare();
};

