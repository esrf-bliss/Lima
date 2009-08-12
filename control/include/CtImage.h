#ifndef CTIMAGE_H
#define CTIMAGE_H

#include "Constants.h"
#include "HwInterface.h"
#include "HwDetInfoCtrlObj.h"
#include "HwBinCtrlObj.h"

namespace lima {

class CtSwBinRoi {
    public:
	CtSwBinRoi(Size& size);

	void setMaxSize(Size& size);
	void setBin(Bin& bin);
	void setRoi(Roi& roi);

	void resetBin() { m_bin.reset(); }
	void resetRoi() { m_roi.reset(); }
	void reset()	{ m_bin.reset(); m_roi.reset(); }

	const Bin& getBin() const { return m_bin; }
	const Roi& getRoi() const { return m_roi; }
	const Size& getSize();

    private:
	Size	m_max_size, m_size;
	Bin	m_bin;
	Roi	m_roi, m_max_roi;
};

class CtImage {

    public:

	enum ImageOpMode {
		HardOnly,
		SoftOnly,
		HardAndSoft,
	};

	CtImage(HwInterface *hw);
	~CtImage();

	void getMaxImageSize(Size& size) const;
	void setMaxImage(Size size, ImageType type);

	void getImageType(ImageType& type) const;
	void getHwImageDim(FrameDim& dim) const;
	void getImageDim(FrameDim& dim) const;


	// --- soft
	void getSoft(CtSwBinRoi *& soft) const;
	//bool getHard(HwBinRoi *& hard) const;

/*	void setHardRoi(Roi roi);
	void resetHardRoi();
	void getHardRoi(Roi roi);

	void setHardBin(Bin bin);
	void resetHardBin();
	void getHardBin(Bin& bin) const;
*/
	// --- wizard
	void setMode(ImageOpMode mode);
	void getMode(ImageOpMode& mode) const;

	void setRoi(Roi roi);
	void setBin(Bin bin);

	void resetRoi();
	void resetBin();
	
	// --- effective
	void getRoi(Roi& roi) const;
	void getBin(Bin& bin) const;

	void reset();

    private:
	void _setHSRoi(Roi roi);
	void _setHSBin(Bin bin);

	HwDetInfoCtrlObj *m_hw_det;
	HwBinCtrlObj	*m_hw_bin;
	// HwRoiCtrlObj	*m_hw_roi;
	// HwFlipCtrlObj	*m_hw_flip;
	bool	m_has_hw_bin, m_has_hw_roi, m_has_hw_flip;
	Size	m_max_size, m_hw_size, m_sw_size;
	ImageType	m_img_type;
	CtSwBinRoi	*m_sw;
	ImageOpMode	m_mode;

	// BinRoi	m_hw_binroi, m_sw_binroi;
};

} // namespace lima

#endif // CTIMAGE_H
