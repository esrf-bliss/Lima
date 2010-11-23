class CtAcquisition {
%TypeHeaderCode
#include <CtAcquisition.h>
#include <Constants.h>
using namespace lima;
%End
    public:
     enum AccTimeMode {Live,Real};

	struct Parameters {
		Parameters();
		void reset();

		AcqMode	         		acqMode;
                CtAcquisition::AccTimeMode      accTimeMode;
		int	         		acqNbFrames;
		double	         		acqExpoTime;
		double	         		accMaxExpoTime;
		int	         		concatNbFrames;
		double	         		latencyTime;
		TrigMode         		triggerMode;

		const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	sipRes = str.str().c_str();
%End
	};

	CtAcquisition(HwInterface *hw);
	~CtAcquisition();

	// --- global

	void setPars(const Parameters &pars);
	void getPars(Parameters& pars /Out/) const;

	void reset();
	void apply(CtControl::ApplyPolicy policy);
	void sync();

	// --- acq modes

	void setAcqMode(AcqMode mode);
	void getAcqMode(AcqMode& mode /Out/) const;

        void setAccTimeMode(CtAcquisition::AccTimeMode mode);
	void getAccTimeMode(CtAcquisition::AccTimeMode &mode /Out/) const;

	void setAcqNbFrames(int nframes);
	void getAcqNbFrames(int& nframes /Out/) const;
	void getAccLiveTime(double& acc_live_time /Out/) const;
        void getAccDeadTime(double& acc_dead_time /Out/) const;

	void setAcqExpoTime(double acq_time);
	void getAcqExpoTime(double& acq_time /Out/) const;

	void setAccMaxExpoTime(double max_time);
	void getAccMaxExpoTime(double& acc_time /Out/) const;

	void getAccNbFrames(int& nframes /Out/) const;
	void getAccExpoTime(double& acc_time /Out/) const;

	void setConcatNbFrames(int nframes);
	void getConcatNbFrames(int& nframes /Out/) const;

	// --- common

	void setLatencyTime(double latency_time);
	void getLatencyTime(double& latency_time /Out/) const;

	void setTriggerMode(TrigMode mode);
	void getTriggerMode(TrigMode& mode /Out/) const;
};

