class CtSwBinRoiFlip {
%TypeHeaderCode
#include <CtImage.h>
#include <sstream>
using namespace lima;
%End
    public:
	CtSwBinRoiFlip(Size& size);
	~CtSwBinRoiFlip();

	void setMaxSize(Size& size);
	void setBin(const Bin& bin);
	void setRoi(const Roi& roi);
	void setFlip(const Flip& flip);

	void resetBin();
	void resetRoi();
	void resetFlip();
	void reset();

	const Bin& getBin() const;
	const Roi& getRoi() const;
	const Size& getSize() const;
	const Flip& getFlip() const;

	const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	sipRes = str.str().c_str();
%End
};

class CtHwBinRoiFlip 
{
%TypeHeaderCode
#include <CtImage.h>
using namespace lima;
%End
    public:
	CtHwBinRoiFlip(HwInterface *hw, CtSwBinRoiFlip *sw_bin_roi, Size& size);
	~CtHwBinRoiFlip();

	bool hasBinCapability() const;
	bool hasRoiCapability() const;
	bool hasFlipCapability() const;

	void setMaxSize(const Size& size);
	void setBin(Bin& bin /In,Out/, bool round);
	void setRoi(Roi& roi /In,Out/, bool round);
	void setFlip(Flip& flip /In,Out/,bool mandatory);

	void resetBin();
	void resetRoi();
	void resetFlip();
	void reset();

	const Bin& getBin() const;
	const Roi& getSetRoi() const;
	const Roi& getRealRoi() const;
	const Size& getSize() const;
	const Flip& getFlip() const;

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

	CtImage(HwInterface *hw,CtControl&);
	~CtImage();

	void getMaxImageSize(Size& size /Out/) const;
	void setMaxImage(const Size &size, ImageType type);

	void getImageType(ImageType& imageType /Out/) const;
	void getHwImageDim(FrameDim& dim /Out/) const;
	void getImageDim(FrameDim& dim /Out/) const;
	// --- soft
	void getSoft(CtSwBinRoiFlip *& soft /Out/) const;
	void getHard(CtHwBinRoiFlip *& hard /Out/) const;

	// --- wizard
	void setMode(ImageOpMode mode);
	void getMode(ImageOpMode& mode /Out/) const;

	void setRoi(Roi &roi);
	void setBin(Bin &bin);

	void resetRoi();
	void resetBin();
	void resetFlip();

	// --- effective
	void getRoi(Roi& roi /Out/) const;
	void getBin(Bin& bin /Out/) const;
	void getFlip(Flip &flip /Out/) const;

	void reset();
};
