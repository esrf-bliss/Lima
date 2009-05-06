#ifndef CTACQUISITION_H
#define CTACQUISITION_H

#include "Constants.h"

namespace lima {	

class CtAcquisition {

    public:
	struct Parameters {
		int     m_acq_nframes;
		double  m_acq_time;
		double  m_latency_time;
	};

	CtAcquisition();
	~CtAcquisition();

	void setParameters(const Parameters& pars);
	void getParameters(Parameters& pars) const;

	void setAcqNbFrames(int nframes);
	void getAcqNbFrames(int& nframes) const;

	void setAcqExposureTime(double acq_time);
	void getAcqExposureTime(double& acq_time) const;

	void setLatencyTime(double latency_time);
	void getLatencyTime(double& latency_time) const;

	void setTriggerMode(TrigMode mode);
	void getTriggerMode(TrigMode& mode) const;

 	// ...

	void getReadoutTime(double& readout_time) const;
	void getFrameRate(double& frame_rate) const;

    private:
	Parameters m_pars;
};

} // namespace lima

#endif // CTACQUISITION_H
