class HwFlipCtrlObj
{
%TypeHeaderCode
#include <HwFlipCtrlObj.h>
using namespace lima;
%End
 public:
	HwFlipCtrlObj();
	virtual ~HwFlipCtrlObj();

	virtual void setFlip(const Flip& flip) = 0;
	virtual void getFlip(Flip& flip /Out/) = 0;
	virtual void checkFlip(Flip& flip /In,Out/) = 0;
};
