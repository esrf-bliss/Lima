

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

    void setup(Maxipix::MaxipixDet::Version ver, Maxipix::MaxipixDet::Polarity pol, float freq, std::string fsr0);
    void setChipType(Maxipix::MaxipixDet::Version v, Maxipix::MaxipixDet::Polarity p);

    void getBoardVersion(short& pcb /Out/, short& firmware /Out/);
    void getChipID(short port, long& id /Out/);

    void setFastFOSpeed(bool fast);
    void getFastFOSpeed(bool& fast /Out/);
    
    void setOscillator(float freq);
    void getOscillator(float& freq /Out/);

    void setChipFsr(short port, std::string fsr);
    void setChipCfg(short port, std::string cfg);
    
    void enableSerial(short port);

    // --- timing

    void setTimeUnit(TimeUnit unit);
    void getTimeUnit(TimeUnit& unit /Out/);

    void setExposureTime(double askexpo, double& setexpo);
    void getExposureTime(double& expo /Out/);

    void setMaxExposureTime();
    void getExposureTimeRange(double& min_expo /Out/, double& max_expo /Out/);

    void setIntervalTime(double asktime, double& settime);
    void getIntervalTime(double& itime /Out/);

    void getIntervalTimeRange(double& minit /Out/, double& maxit /Out/);

    void setShutterTime(double asktime, double& settime /Out/);
    void getShutterTime(double& stime /Out/);

    // --- external IO signals

    void setShutterLevel(SignalLevel level);
    void getShutterLevel(SignalLevel& level /Out/);

    void setShutterMode(ShutterMode mode);
    void getShutterMode(ShutterMode& mode /Out/);

    void setReadyLevel(SignalLevel level);
    void getReadyLevel(SignalLevel& level /Out/);

    void setReadyMode(ReadyMode mode);
    void getReadyMode(ReadyMode& mode /Out/);

    void setGateLevel(SignalLevel level);
    void getGateLevel(SignalLevel& level /Out/);

    void setGateMode(GateMode mode);
    void getGateMode(GateMode& mode /Out/);

    void setTriggerLevel(SignalLevel level);
    void getTriggerLevel(SignalLevel& level /Out/);

    void setTriggerMode(TrigMode mode);
    void getTriggerMode(TrigMode& mode /Out/);

    // --- acquisition

    void setNbFrames(int nb);
    void getNbFrames(int& nb /Out/);

    void setSerialReadout(short port);
    void setParalellReadout(std::vector<int> ports);
    void getReadoutMode(ReadoutMode& mode /Out/, std::vector<int>& ports /Out/);

    void setImageMode(ImageMode image);
    void getImageMode(ImageMode& image /Out/);

    void setFFCorrection(short flat);
    void getFFCorrection(short& flat /Out/);

    void startAcq();
    void stopAcq();

    void getStatus(DetStatus& status /Out/);

    // --- reset

    void resetFifo(short port);
    void resetAllFifo();
    void resetChip(short port);
    void resetAllChip();
  };
};

