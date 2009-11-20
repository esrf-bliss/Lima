%MappedType std::vector<HwCap>
{
%TypeHeaderCode
#include <vector>
#include <HwCap.h>
%End
%ConvertToTypeCode
if(sipIsErr == NULL)
 return PySequence_Check(sipPy);

*sipCppPtr = NULL; // not used in that way.
return 0;
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
	};

	enum ResetLevel {
		SoftReset, HardReset,
	};

	HwInterface();
	virtual ~HwInterface();

	virtual const std::vector<HwCap>& getCapList() const = 0;

	SIP_PYOBJECT getHwCtrlObj(HwCap::Type cap_type);
%MethodCode
	const HwInterface::CapList& cap_list = sipCpp->getCapList();

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
