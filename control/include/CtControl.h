#ifndef CTCONTROL_H
#define CTCONTROL_H

#include "HwInterface.h"

namespace lima {

class CtControl {
    public:

	struct ImageStatus {
		long	LastImageAcquired;
		long	LastBaseImageReady;
		long	LastImageReady;
		long	LastImageSaved;
		long	LastCounterReady;
	};

	CtControl(HwInterface *hw);
	~CtControl();

	void prepareAcq();
	void startAcq();
	void stopAcq();

	void getAcqStatus(); // from HW
	void getImageStatus(ImageStatus& status) const;

	void reset();

    private:
	HwInterface	m_hw;
	ImageStatus	m_img_status;
};

} // namespace lima

#endif // CTCONTROL_H
