class HwBinCtrlObj
{
%TypeHeaderCode
#include <HwBinCtrlObj.h>
using namespace lima;
%End
 public:
	HwBinCtrlObj();
	virtual ~HwBinCtrlObj();

	virtual void setBin(const Bin& bin) = 0;
	virtual void getBin(Bin& bin /Out/) = 0;
	virtual void checkBin(Bin& bin /In,Out/) = 0;
};
