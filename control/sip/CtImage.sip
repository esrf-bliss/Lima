%If (!tobedeleted)
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

	void getMaxImageSize(Size& size /Out/) const;
	void getImageSize(Size& size /Out/) const;
	void getImageType(ImageType& imageType /Out/) const;
	
	// --- soft
	void setSoftRoi(const Roi &roi);
	void getSoftRoi(Roi& roi /Out/) const;
	void setSoftBin(const Bin &bin);	
	void getSoftBin(Bin& bin /Out/) const;
	// --- hard
	void setHardRoi(const Roi &roi);
	void getHardRoi(Roi& roi) const;

	void setHardBin(const Bin &bin);
	void getHardBin(Bin& bin) const;

	// --- wizard
	void setWizardMode(WizardMode mode);
	void getWizardMode(WizardMode& mode /Out/) const;


	void setWizardRoi(const Roi &roi);
	void setWizardBin(const Bin &bin);

	// --- effective
	void getRoi(Roi& roi /Out/) const;
	void getBin(Bin& bin /Out/) const;
};
%End
