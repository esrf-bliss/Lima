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
	void setBin(const Bin& bin);
	void setRoi(const Roi& roi);

	void resetBin();
	void resetRoi();
	void reset();

	const Bin& getBin() const { return m_bin; }
	const Roi& getRoi() const { return m_roi; }
	const Size& getSize();

    private:
	Size	m_max_size, m_size;
	Bin	m_bin;
	Roi	m_roi, m_max_roi;
};

class CtHwBinRoi {
    public:
	CtHwBinRoi(HwInterface *hw, CtSwBinRoi *sw_bin_roi, Size& size);
	~CtHwBinRoi();

	bool hasBinCapability() { return m_has_bin; }
	bool hasRoiCapability() { return m_has_roi; }

	void setMaxSize(const Size& size);
	void setBin(Bin& bin, bool round);
	void setRoi(Roi& roi, bool round);

	void resetBin();
	void resetRoi();
	void reset();

	const Bin& getBin() const { return m_bin; }
	const Roi& getRoi() const { return m_roi; }
	const Size& getSize() const { return m_size; }

	void apply();

    private:
	void _updateSize();

	HwBinCtrlObj	*m_hw_bin;
	HwRoiCtrlObj	*m_hw_roi;
	CtSwBinRoi	*m_sw_bin_roi;
	bool	m_has_bin, m_has_roi;
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
	void setMaxImage(const Size &size, ImageType type);

	void getImageType(ImageType& type) const;
	void getHwImageDim(FrameDim& dim) const;
	void getImageDim(FrameDim& dim) const;

	// --- soft
	void getSoft(CtSwBinRoi *& soft) const;
	void getHard(CtHwBinRoi *& hard) const;

	// --- wizard
	void setMode(ImageOpMode mode);
	void getMode(ImageOpMode& mode) const;

	void setRoi(Roi &roi);
	void setBin(Bin &bin);

	void resetRoi();
	void resetBin();
	
	// --- effective
	void getRoi(Roi& roi) const;
	void getBin(Bin& bin) const;

	void reset();

    private:
	void _setHSRoi(const Roi &roi);
	void _setHSBin(const Bin &bin);

	HwDetInfoCtrlObj *m_hw_det;
	// HwFlipCtrlObj	*m_hw_flip;
	CtSwBinRoi	*m_sw;
	CtHwBinRoi	*m_hw;

	Size		m_max_size;
	ImageType	m_img_type;
	ImageOpMode	m_mode;
};

} // namespace lima

#endif // CTIMAGE_H
