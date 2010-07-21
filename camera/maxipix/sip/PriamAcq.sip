

namespace Maxipix {

%TypeHeaderCode
#include <string>
#include <vector>

#include "PriamSerial.h"
#include "PriamAcq.h"
#include "MaxipixDet.h"
#include "Constants.h"

using namespace lima;
%End

class PriamAcq {

public:
  typedef std::pair<int,std::string> PortNFsr;

  enum TimeUnit {
    UNIT_US,
    UNIT_10US,
    UNIT_100US,
    UNIT_MS,
    UNIT_10MS,
    UNIT_100MS,
    UNIT_S,
    UNIT_NB
  };

  enum SignalLevel {
    LOW_FALL,
    HIGH_RISE
  };

  enum ShutterMode {
    FRAME,
    SEQUENCE
  };

  enum GateMode {
    INACTIVE,
    ACTIVE
  };

  enum ReadyMode {
    EXPOSURE,
    EXPOSURE_READOUT
  };

  enum ReadoutMode {
    PARALELL,
    SERIAL
  };

  enum ImageMode {
    NORMAL,
    RAW
  };

    PriamAcq(Maxipix::PriamSerial& priam_serial);
    ~PriamAcq();

    // --- configuration

    void setup(Maxipix::MaxipixDet::Version ver, Maxipix::MaxipixDet::Polarity pol, float freq,const std::string &fsr0);
    void setChipType(Maxipix::MaxipixDet::Version v, Maxipix::MaxipixDet::Polarity p);

    void getBoardVersion(short& pcb /Out/, short& firmware /Out/) const;
    void getChipID(short port, long& id /Out/) const;

    void setFastFOSpeed(bool fast);
    void getFastFOSpeed(bool& fast /Out/) const;
    
    void setOscillator(float freq);
    void getOscillator(float& freq /Out/) const;

    void setChipFsr(short port, const std::string &fsr);
    void setChipCfg(short port, const std::string &cfg);

    void applyFsrChips();
    void prepareFsrChips(SIP_PYOBJECT);
%MethodCode
    if(!PySequence_Check(a0))
	throw Exception(Hardware,InvalidValue,"Should give sequence",
	__FILE__,__FUNCTION__, __LINE__);
    std::vector<Maxipix::PriamAcq::PortNFsr> aTmpVector;
    for(Py_ssize_t i = 0;i < PySequence_Size(a0);++i)
	{
	   PyObject *aTuplePt = PySequence_Fast_GET_ITEM(a0,i);
	   if(!PySequence_Check(aTuplePt) ||
	      PySequence_Size(aTuplePt) != 2)
	     throw Exception(Hardware,InvalidValue,"In the sequence you should give (portid,fsrstring)",
			     __FILE__,__FUNCTION__, __LINE__);
	   PyObject *intObject = PySequence_Fast_GET_ITEM(aTuplePt,0);
	   if(!PyInt_Check(intObject))
	     throw Exception(Hardware,InvalidValue,"first object in the sequence is the portid",
			     __FILE__,__FUNCTION__, __LINE__);
	   PyObject *stringObject = PySequence_Fast_GET_ITEM(aTuplePt,1);
	   if(!PyString_Check(stringObject))
	     throw Exception(Hardware,InvalidValue,"second object in the sequence is the fsrstring",
			     __FILE__,__FUNCTION__, __LINE__);
	   
	   aTmpVector.push_back(Maxipix::PriamAcq::PortNFsr(PyInt_AsLong(intObject),
							    std::string(PyString_AS_STRING(stringObject),
									PyString_GET_SIZE(stringObject))));
	}
    
   Py_BEGIN_ALLOW_THREADS;
   sipCpp->prepareFsrChips(aTmpVector);
   Py_END_ALLOW_THREADS;
%End    

    void enableSerial(short port);

    // --- timing

    void setTimeUnit(TimeUnit unit);
    void getTimeUnit(TimeUnit& unit /Out/) const;

    void setExposureTime(double askexpo, double& setexpo);
    void getExposureTime(double& expo /Out/) const;

    void setMaxExposureTime();
    void getExposureTimeRange(double& min_expo /Out/, double& max_expo /Out/) const;

    void setIntervalTime(double asktime, double& settime);
    void getIntervalTime(double& itime /Out/) const;

    void getIntervalTimeRange(double& minit /Out/, double& maxit /Out/) const;

    void setShutterTime(double asktime, double& settime /Out/);
    void getShutterTime(double& stime /Out/) const;

    // --- external IO signals

    void setShutterLevel(SignalLevel level);
    void getShutterLevel(SignalLevel& level /Out/) const;

    void setShutterMode(ShutterMode mode);
    void getShutterMode(ShutterMode& mode /Out/) const;

    void setReadyLevel(SignalLevel level);
    void getReadyLevel(SignalLevel& level /Out/) const;

    void setReadyMode(ReadyMode mode);
    void getReadyMode(ReadyMode& mode /Out/) const;

    void setGateLevel(SignalLevel level);
    void getGateLevel(SignalLevel& level /Out/) const;

    void setGateMode(GateMode mode);
    void getGateMode(GateMode& mode /Out/) const;

    void setTriggerLevel(SignalLevel level);
    void getTriggerLevel(SignalLevel& level /Out/) const;

    void setTriggerMode(TrigMode mode);
    void getTriggerMode(TrigMode& mode /Out/) const;

    // --- acquisition

    void setNbFrames(int nb);
    void getNbFrames(int& nb /Out/) const;

    void setSerialReadout(short port);
    void setParalellReadout(std::vector<int> ports);
    void getReadoutMode(ReadoutMode& mode /Out/, std::vector<int>& ports /Out/) const;

    void setImageMode(ImageMode image);
    void getImageMode(ImageMode& image /Out/) const;

    void setFFCorrection(short flat);
    void getFFCorrection(short& flat /Out/) const;

    void startAcq();
    void stopAcq();

    void getStatus(DetStatus& status /Out/) const;

    // --- reset

    void resetFifo(short port);
    void resetAllFifo();
    void resetChip(short port);
    void resetAllChip();
  };
};

