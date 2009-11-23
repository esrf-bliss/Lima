class CtSwBinRoi {
%TypeHeaderCode
#include <CtImage.h>
using namespace lima;
%End
    public:
	CtSwBinRoi(Size& size);

	void setMaxSize(Size& size);
	void setBin(const Bin& bin);
	void setRoi(const Roi& roi);

	void resetBin();
	void resetRoi();
	void reset();

	const Bin& getBin() const;
	const Roi& getRoi() const;
	const Size& getSize() const;
};

class CtHwBinRoi 
{
%TypeHeaderCode
#include <CtImage.h>
using namespace lima;
%End
    public:
	CtHwBinRoi(HwInterface *hw, CtSwBinRoi *sw_bin_roi, Size& size);
	~CtHwBinRoi();

	bool hasBinCapability();
	bool hasRoiCapability();

	void setMaxSize(const Size& size);
	void setBin(Bin& bin /In,Out/, bool round);
	void setRoi(Roi& roi /In,Out/, bool round);

	void resetBin();
	void resetRoi();
	void reset();

	const Bin& getBin() const;
	const Roi& getSetRoi() const;
	const Roi& getRealRoi() const;
	const Size& getSize() const;

	void apply();
};

class CtImage {
%TypeHeaderCode
#include <CtImage.h>
using namespace lima;
%End
    public:

	enum ImageOpMode {
		HardOnly,
		SoftOnly,
		HardAndSoft,
	};
		
	// Size, Type, Bin, XY from common

	CtImage(HwInterface *hw);
	~CtImage();

	void getMaxImageSize(Size& size /Out/) const;
	void setMaxImage(const Size &size, ImageType type);

	void getImageType(ImageType& imageType /Out/) const;
	void getHwImageDim(FrameDim& dim /Out/) const;
	void getImageDim(FrameDim& dim) const;
	// --- soft
	void getSoft(CtSwBinRoi *& soft /Out/) const;
	void getHard(CtHwBinRoi *& hard /Out/) const;

	// --- wizard
	void setMode(ImageOpMode mode);
	void getMode(ImageOpMode& mode /Out/) const;

	void setRoi(Roi &roi);
	void setBin(Bin &bin);

	void resetRoi();
	void resetBin();
	
	// --- effective
	void getRoi(Roi& roi /Out/) const;
	void getBin(Bin& bin /Out/) const;

	void reset();
};
