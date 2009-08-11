#ifndef CTACQUISITION_H
#define CTACQUISITION_H

#include "Constants.h"
#include "HwInterface.h"
#include "HwCap.h"
#include "CtControl.h"

namespace lima {	

class CtAcquisition {
	friend class CtControl;

    public:

	struct Parameters {
		Parameters();
		void reset();

		AcqMode	acqMode;
		int	acqNbFrames;
		double	acqExpoTime;
		double	accMaxExpoTime;
		int	concatNbFrames;
		double	latencyTime;
		TrigMode triggerMode;
	};

	CtAcquisition(HwInterface *hw);
	~CtAcquisition();

	// --- global

	void setPars(Parameters pars);
	void getPars(Parameters& pars) const;

	void reset();
	void apply(CtControl::ApplyPolicy policy);
	void sync();

	// --- acq modes

	void setAcqMode(AcqMode mode);
	void getAcqMode(AcqMode& mode) const;

	void setAcqNbFrames(int nframes);
	void getAcqNbFrames(int& nframes) const;

	void setAcqExpoTime(double acq_time);
	void getAcqExpoTime(double& acq_time) const;

	void setAccMaxExpoTime(double max_time);

	void getAccNbFrames(int& nframes);
	void getAccExpoTime(double& acc_time);

	void setConcatNbFrames(int nframes);
	void getConcatNbFrames(int& nframes) const; 

	// --- common

	void setLatencyTime(double latency_time);
	void getLatencyTime(double& latency_time) const;

	void setTriggerMode(TrigMode mode);
	void getTriggerMode(TrigMode& mode) const;

    private:

	struct ChangedPars {
		ChangedPars();
		void set(bool);
		void check(Parameters p1, Parameters p2);

		bool	acqExpoTime;
		bool	acqNbFrames;
		bool	latencyTime;
		bool	triggerMode;
		bool	accMaxExpoTime;
	};

	void _updateAccPars();
	void _setDefaultPars(Parameters* pars);
	void _apply();
	void _hwRead();

	HwSyncCtrlObj	*m_hw_sync;
	HwSyncCtrlObj::ValidRangesType	m_valid_ranges;
	Parameters	m_inpars, m_hwpars;
	ChangedPars	m_changes;
	double		m_readout_time;
	double		m_frame_rate;
	int		m_acc_nframes;
	double		m_acc_exptime;
	bool		m_applied_once;
};

} // namespace lima

#endif // CTACQUISITION_H
