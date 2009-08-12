class SwBinRoi {
%TypeHeaderCode
#include <CtImage.h>
using namespace lima;
%End
    public:
	SwBinRoi(Size& size);

	void setMaxSize(Size& size);
	void setBin(Bin& bin);
	void setRoi(Roi& roi);

	void resetBin();
	void resetRoi();
	void reset();

	const Bin& getBin() const;
	const Roi& getRoi() const;
	const Size& getSize() const;
};

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
	void setMaxImage(Size size, ImageType type);

	void getImageType(ImageType& imageType /Out/) const;
	void getHwImageDim(FrameDim& dim /Out/) const;
	void getImageDim(FrameDim& dim) const;
	
	// --- soft
/*	void setSoftRoi(const Roi &roi);
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

	void reset();
*/
};
