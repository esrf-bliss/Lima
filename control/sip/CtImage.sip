class CtImage {
%TypeHeaderCode
#include <CtImage.h>
using namespace lima;
%End
    public:

	enum WizardMode {
		HardOnly,
		SoftOnly,
		HardAndSoft,
	};
		
	// Size, Type, Bin, XY from common

	CtImage(HwInterface *hw);
	~CtImage();

	Size getMaxImageSize() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipRes = new Size();
sipCpp->getMaxImageSize(*sipRes);
Py_END_ALLOW_THREADS
%End

	Size getImageSize() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipRes = new Size();
sipCpp->getImageSize(*sipRes);
Py_END_ALLOW_THREADS
%End

	ImageType getImageType() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getImageType(sipRes);
Py_END_ALLOW_THREADS
%End
	
	// --- soft
	void setSoftRoi(const Roi &roi);
	Roi getSoftRoi() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipRes = new Roi();
sipCpp->getSoftRoi(*sipRes);
Py_END_ALLOW_THREADS
%End

	void setSoftBin(const Bin &bin);	
	Bin getSoftBin() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipRes = new Bin();
sipCpp->getSoftBin(*sipRes);
Py_END_ALLOW_THREADS
%End


	// --- hard
	void setHardRoi(const Roi &roi);
	Roi getHardRoi() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipRes = new Roi();
sipCpp->getHardRoi(*sipRes);
Py_END_ALLOW_THREADS
%End

	void setHardBin(const Bin &bin);
	Bin getHardBin() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipRes = new Bin();
sipCpp->getHardBin(*sipRes);
Py_END_ALLOW_THREADS
%End

	// --- wizard
	void setWizardMode(WizardMode mode);
	WizardMode getWizardMode() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getWizardMode(sipRes);
Py_END_ALLOW_THREADS
%End


	void setWizardRoi(const Roi &roi);
	void setWizardBin(const Bin &bin);

	// --- effective
	Roi getRoi() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipRes = new Roi();
sipCpp->getRoi(*sipRes);
Py_END_ALLOW_THREADS
%End
	Bin getBin() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipRes = new Bin();
sipCpp->getBin(*sipRes);
Py_END_ALLOW_THREADS
%End
};
