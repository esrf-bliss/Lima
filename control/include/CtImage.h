#ifndef CTIMAGE_H
#define CTIMAGE_H

#include "Constants.h"
#include "HwInterface.h"
#include "HwDetInfoCtrlObj.h"
#include "HwBinCtrlObj.h"

namespace lima {

class BinRoi {
    public:
	BinRoi() : m_max_size(), m_size(), m_bin(), m_roi() {};

	void setMaxSize(Size& size);
	void setBin(Bin& bin);
	void setRoi(Roi& roi);

	void resetBin() { m_bin.reset(); }
	void resetRoi() { m_roi.reset(); }
	void reset()	{ m_bin.reset(); m_roi.reset(); }

	const Bin& getBin() const { return m_bin; }
	const Roi& getRoi() const { return m_roi; }
	const Size& getSize() const { return m_size; }

    private:
	void 	_getBinSize(Size& size) { size= m_max_size / m_size ; }
	void 	_updateRoi();
	void	_updateSize();

	Size	m_max_size, m_size;
	Bin	m_bin;
	Roi	m_roi;
};

class CtImage {

    public:

	enum WizardMode {
		HardOnly,
		SoftOnly,
		HardAndSoft,
	};

	// Size, Type, Bin, XY from common

	CtImage(HwInterface *hw);
	~CtImage();

	void getMaxImageSize(Size& size) const;
	void setMaxImage(Size size, ImageType type);

	void getImageType(ImageType& type) const;
	void getHwImageDim(FrameDim& dim) const;
	void getImageDim(FrameDim& dim) const;


	// --- soft
/*	void setSoftRoi(Roi roi);
	void resetSoftRoi(Roi roi);
	void getSoftRoi(Roi& roi) const;

	void setSoftBin(Bin bin);
	void resetSoftBin();
	void getSoftBin(Bin& bin) const;

	// --- hard
	void setHardRoi(Roi roi);
	void resetHardRoi();
	void getHardRoi(Roi roi);

	void setHardBin(Bin bin);
	void resetHardBin();
	void getHardBin(Bin& bin) const;
*/
	// --- wizard
/*	void setWizardMode(WizardMode mode);
	void getWizardMode(WizardMode& mode) const;

	void setWizardRoi(Roi roi);
	void setWizardBin(Bin bin);

	void resetWizardRoi();
	void resetWizardBin();
*/
	// --- effective
/*	void getRoi(Roi& roi) const;
	void setBin(Bin& bin) const;

	void reset();
*/
    private:

	HwDetInfoCtrlObj *m_hw_det;
	HwBinCtrlObj	*m_hw_bin;
	// HwRoiCtrlObj	*m_hw_roi;
	// HwFlipCtrlObj	*m_hw_flip;
	bool	m_has_hw_bin, m_has_hw_roi, m_has_hw_flip;
	Size	m_max_size, m_hw_size, m_sw_size;
	ImageType	m_img_type;

	// BinRoi	m_hw_binroi, m_sw_binroi;
	// WizardMode	m_wizard_mode;
};

} // namespace lima

#endif // CTIMAGE_H
