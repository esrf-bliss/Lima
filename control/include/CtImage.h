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
	void setSoftRoi(const Roi &roi);
	void getSoftRoi(Roi& roi) const;

	void setSoftBin(const Bin &bin);	
	void getSoftBin(Bin& bin) const;

	// --- hard
	void setHardRoi(const Roi &roi);
	void getHardRoi(Roi &roi) const;

	void setHardBin(const Bin &bin);
	void getHardBin(Bin& bin) const;

	// --- wizard
	void setWizardMode(WizardMode mode);
	void getWizardMode(WizardMode& mode) const;

	void setWizardRoi(const Roi &roi);
	void setWizardBin(const Bin &bin);

	// --- effective
	void getRoi(Roi& roi) const;
	void getBin(Bin& bin) const;

    private:
	Roi	m_soft_roi, m_hard_roi, m_eff_roi;
	Bin	m_soft_bin, m_hard_bin, n_eff_bin;
	WizardMode	m_wizard_mode;

};

} // namespace lima

#endif // CTIMAGE_H
