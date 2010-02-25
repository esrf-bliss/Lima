%MappedType std::vector<HwCap>
{
%TypeHeaderCode
#include <vector>
#include <HwCap.h>
%End
%ConvertToTypeCode
if(sipIsErr == NULL)
 return PySequence_Check(sipPy);

*sipCppPtr = new std::vector<HwCap>(PyList_Size(sipPy));
for(int i = 0;i < PyList_Size(sipPy);++i)
{
  PyObject *itemPt = PyList_GetItem(sipPy,i);
  int state;
#if (SIP_VERSION > 0x040800)
  HwCap *hwCapPt = (HwCap*)sipConvertToType(itemPt,sipType_HwCap, sipTransferObj,SIP_NOT_NONE, &state,sipIsErr);
#else
  HwCap *hwCapPt = (HwCap*)sipConvertToInstance(itemPt,sipClass_HwCap, sipTransferObj,SIP_NOT_NONE, &state,sipIsErr);
#endif
  if(!*sipIsErr)
  {
    if(hwCapPt)
      (*sipCppPtr)->operator[](i) = HwCap(*hwCapPt);
  }
#if (SIP_VERSION >= 0x040800)
  sipReleaseType(hwCapPt,sipType_HwCap,state);
#else
  sipReleaseInstance(hwCapPt,sipClass_HwCap,state);
#endif
}
return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode

PyObject *returnList =  PyList_New(sipCpp->size());
int aPos = 0;
for(std::vector<HwCap>::iterator i = sipCpp->begin();
    i != sipCpp->end();++i,++aPos)
{
  PyObject *aHwCap = sipConvertFromInstance(&(*i),sipClass_HwCap,NULL); 
  PyList_SET_ITEM(returnList,aPos,aHwCap);
  Py_DECREF(aHwCap);
}
return returnList;
%End
};

class HwInterface
{
%TypeHeaderCode
#include <HwCap.h>
#include <HwInterface.h>
#include <LimaConvertor.h>
using namespace lima;
%End  
 public:
        struct StatusType {
		AcqStatus acq;
		DetStatus det;
		DetStatus det_mask;
	const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	sipRes = str.str().c_str();
%End
	};

	enum ResetLevel {
		SoftReset, HardReset,
	};

	HwInterface();
	virtual ~HwInterface();

	virtual void getCapList(std::vector<HwCap> &cap_list /Out/) const = 0;

	SIP_PYOBJECT getHwCtrlObj(HwCap::Type cap_type);
%MethodCode
	HwInterface::CapList cap_list;
	sipCpp->getCapList(cap_list);

	typedef HwInterface::CapList::const_iterator It;
	for (It i = cap_list.begin(); i != cap_list.end(); ++i)
		if (i->getType() == a0) {
			SIP_CONVERT_CTRLOBJECT_TO_SIPOBJECT(i);
			break;
		}
%End
	virtual void reset(ResetLevel reset_level) = 0;
	virtual void prepareAcq() = 0;
	virtual void startAcq() = 0;
	virtual void stopAcq() = 0;
	virtual void getStatus(StatusType& status /Out/) = 0;
	virtual int getNbAcquiredFrames();
	virtual int getNbHwAcquiredFrames() = 0;
};
