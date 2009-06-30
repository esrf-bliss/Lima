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
	Parameters getParameters() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipRes = new CtAcquisition::Parameters();
sipCpp->getParameters(*sipRes);
Py_END_ALLOW_THREADS
%End

	// --- acq modes

	void setAcqMode(AcqMode mode);
	AcqMode getAcqMode() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getAcqMode(sipRes);
Py_END_ALLOW_THREADS
%End

	void setAcqNbFrames(int nframes);
	int getAcqNbFrames() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getAcqNbFrames(sipRes);
Py_END_ALLOW_THREADS
%End


	void setAcqExposureTime(double acq_time);
	double getAcqExposureTime() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getAcqExposureTime(sipRes);
Py_END_ALLOW_THREADS
%End


	void setAccNbFrames(int nframes);
	int getAccNbFrames();
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getAccNbFrames(sipRes);
Py_END_ALLOW_THREADS
%End


	void setAccExposureTime(double acc_time);
	double getAccExposureTime() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getAccExposureTime(sipRes);
Py_END_ALLOW_THREADS
%End

	void setConcatNbFrames(int nframes);
	int getConcatNbFrames();
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getConcatNbFrames(sipRes);
Py_END_ALLOW_THREADS
%End

	// --- common

	void setLatencyTime(double latency_time);
	double getLatencyTime() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getLatencyTime(sipRes);
Py_END_ALLOW_THREADS
%End

	void setTriggerMode(TrigMode mode);
	TrigMode getTriggerMode() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getTriggerMode(sipRes);
Py_END_ALLOW_THREADS
%End


	// --- shutter

	void setShutterMode(ShutMode mode);
	ShutMode getShutterMode() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getShutterMode(sipRes);
Py_END_ALLOW_THREADS
%End


	void setShutterOpenTime(double open_time);
	double getShutterOpenTime() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getShutterOpenTime(sipRes);
Py_END_ALLOW_THREADS
%End

	void setShutterCloseTime(double close_time);
	double getShutterCloseTime() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getShutterCloseTime(sipRes);
Py_END_ALLOW_THREADS
%End
 	// --- read-only

	double getReadoutTime() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getReadoutTime(sipRes);
Py_END_ALLOW_THREADS
%End
 
	double getFrameRate() const;
%MethodCode
Py_BEGIN_ALLOW_THREADS
sipCpp->getFrameRate(sipRes);
Py_END_ALLOW_THREADS
%End
};
