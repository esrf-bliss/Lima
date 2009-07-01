class CtAcquisition {
%TypeHeaderCode
#include <CtAcquisition.h>
using namespace lima;
%End
    public:
        enum ShutMode {Manual,AutoPerFrame,AutoPerSequence};
        enum TrigMode {Internal,ExtStart,MultExtStart,Gate,ExtStartStop};
        enum AcqMode {Single,Accumulation,Concatenation};
	struct Parameters {
		CtAcquisition::AcqMode	acqMode;
		int	acqNbFrames;
		double	acqExpoTime;
		int	accNbFrames;
		double	accExpoTime;
		int	concatNbFrames;
		double	latencyTime;
		CtAcquisition::TrigMode triggerMode;
		CtAcquisition::ShutMode shutMode;
		double	shutOpenTime;
		double	shutCloseTime;
	};

	CtAcquisition(HwInterface *hw);
	~CtAcquisition();

	// --- global

	void setParameters(const Parameters& pars);
	void getParameters(Parameters& pars /Out/) const;

	// --- acq modes

	void setAcqMode(AcqMode mode);
	void getAcqMode(AcqMode& mode /Out/) const;

	void setAcqNbFrames(int nframes);
	void getAcqNbFrames(int& nframes /Out/) const;

	void setAcqExposureTime(double acq_time);
	void getAcqExposureTime(double& acq_time /Out/) const;

	void setAccNbFrames(int nframes);
	void getAccNbFrames(int& nframes /Out/) const;

	void setAccExposureTime(double acc_time);
	void getAccExposureTime(double& acc_time /Out/) const;

	void setConcatNbFrames(int nframes);
	void getConcatNbFrames(int& nframes /Out/) const;

	// --- common

	void setLatencyTime(double latency_time);
	void getLatencyTime(double& latency_time /Out/) const;

	void setTriggerMode(TrigMode mode);
	void getTriggerMode(TrigMode& mode /Out/) const;


	// --- shutter

	void setShutterMode(ShutMode mode);
	void getShutterMode(ShutMode& mode /Out/) const;

	void setShutterOpenTime(double open_time);
	void getShutterOpenTime(double& open_time /Out/) const;

	void setShutterCloseTime(double close_time);
	void getShutterCloseTime(double& close_time /Out/) const;
 	// --- read-only

	void getReadoutTime(double& readout /Out/) const;
 
	void getFrameRate(double& framerate /Out/) const;
};
