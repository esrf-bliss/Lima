#ifndef CTIMAGE_H
#define CTIMAGE_H

#include "Constants.h"
#include "HwInterface.h"
#include "HwCap.h"

namespace lima {

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
	void getImageSize(Size& size) const;
	void getImageType(ImageType& type) const;
	
	// --- soft
	void setSoftROI(ROI roi);
	void getSoftROI(ROI& roi) const;

	void setSoftBin(BIN bin);	
	void getSoftBin(BIN& bin) const;

	// --- hard
	void setHardROI(ROI roi);
	void getHardROI(ROI roi);

	void setHardBin(BIN bin);
	void getHardBin(BIN& bin) const;

	// --- wizard
	void setWizardMode(WizardMode mode);
	void getWizardMode(WizardMode& mode) const;

	void setWizardROI(ROI roi);
	void setWizardBin(BIN bin);

	// --- effective
	void getROI(ROI& roi) const;
	void setBin(BIN& bin) const;

    private:
	ROI	m_soft_roi, m_hard_roi, m_eff_roi;
	BIN	m_soft_bin, m_hard_bin, n_eff_bin;
	WizardMode	m_wizard_mode;

}

} // namespace lima

#endif // CTIMAGE_H
