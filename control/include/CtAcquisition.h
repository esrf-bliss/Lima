#ifndef CTACQUISITION_H
#define CTACQUISITION_H

#include "Constants.h"
#include "HwInterface.h"
#include "HwCap.h"

namespace lima {	

class CtAcquisition {

    public:
	struct Parameters {
		AcqMode	acqMode;
		int	acqNbFrames;
		double	acqExpoTime;
		int	accNbFrames;
		double	accExpoTime;
		int	concatNbFrames;
		double	latencyTime;
		TrigMode triggerMode;
		ShutMode shutMode;
		double	shutOpenTime;
		double	shutCloseTime;
	};

	CtAcquisition(HwInterface *hw);
	~CtAcquisition();

	// --- global

	void setParameters(const Parameters& pars);
	void getParameters(Parameters& pars) const;

	// --- acq modes

	void setAcqMode(AcqMode mode);
	void getAcqMode(AcqMode& mode) const;

	void setAcqNbFrames(int nframes);
	void getAcqNbFrames(int& nframes) const;

	void setAcqExposureTime(double acq_time);
	void getAcqExposureTime(double& acq_time) const;

	void setAccNbFrames(int nframes);
	void getAccNbFrames(int& nframes);

	void setAccExposureTime(double acc_time);
	void getAccExposureTime(double& acc_time) const;

	void setConcatNbFrames(int nframes);
	void getConcatNbFrames(int& nframes);

	// --- common

	void setLatencyTime(double latency_time);
	void getLatencyTime(double& latency_time) const;

	void setTriggerMode(TrigMode mode);
	void getTriggerMode(TrigMode& mode) const;

	// --- shutter

	void setShutterMode(ShutMode mode);
	void getShutterMode(ShutMode& mode) const;

	void setShutterOpenTime(double open_time);
	void getShutterOpenTime(double& open_time) const;

	void setShutterCloseTime(double close_time);
	void getShutterCloseTime(double& close_time) const;

 	// --- read-only

	void getReadoutTime(double& readout_time) const;
	void getFrameRate(double& frame_rate) const;

    private:
	HwSyncCtrlObj	*m_hw_sync;
	HwBufferCtrlObj	*m_hw_buffer;
	Parameters	m_pars;
	double		m_readout_time;
	double		m_frame_rate;
};

} // namespace lima

#endif // CTACQUISITION_H
