#ifndef SIMUHWINTERFACE_H
#define SIMUHWINTERFACE_H

#include "HwInterface.h"
#include "Simulator.h"

namespace lima
{

class SimuHwInterface;

class SimuDetInfoCtrlObj : public HwDetInfoCtrlObj
{
 public:
	SimuDetInfoCtrlObj(Simulator& simu);
	virtual ~SimuDetInfoCtrlObj();

	virtual void getMaxImageSize(Size& max_image_size);
	virtual void getDetectorImageSize(Size& det_image_size);

	virtual void getDefImageType(ImageType& def_image_type);
	virtual void getCurrImageType(ImageType& curr_image_type);
	virtual void setCurrImageType(ImageType  curr_image_type);

	virtual void getPixelSize(double& pixel_size);
	virtual void getDetectorType(std::string& det_type);
	virtual void getDetectorModel(std::string& det_model);

 protected:
	virtual void setMaxImageSizeCallbackActive(bool cb_active);

 private:
	Simulator& m_simu;
	bool m_iscb_act;
};


class SimuHwInterface : public HwInterface
{
 public:
	SimuHwInterface(Simulator& simu);
	virtual ~SimuHwInterface();

	virtual const CapList& getCapList() const;

	virtual void reset(ResetLevel reset_level);
	virtual void prepareAcq();
	virtual void startAcq();
	virtual void stopAcq();
	virtual void getStatus(StatusType& status);
	virtual int getNbAcquiredFrames();

 private:
	Simulator& m_simu;
	CapList m_cap_list;
	SimuDetInfoCtrlObj m_det_info;
};

}

#endif // SIMUHWINTERFACE_H
