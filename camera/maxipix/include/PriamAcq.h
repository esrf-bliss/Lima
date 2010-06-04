#ifndef _PRIAMACQ_H
#define _PRIAMACQ_H

#include <string>
#include <vector>

#include "PriamSerial.h"
#include "MaxipixDet.h"
#include "Constants.h"

namespace lima {
namespace Maxipix {

#define SETBIT(c, p, v) { if (v) c |= (1<<p); else c &= ~(1<<p); }

class PriamAcq {

    DEB_CLASS_NAMESPC(DebModCamera, "PriamAcq", "Maxipix");

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

    static const int maxPorts= 5;

    PriamAcq(PriamSerial& priam_serial);
    ~PriamAcq();

    // --- configuration

    void setup(MaxipixDet::Version ver, MaxipixDet::Polarity pol, \
		float freq, std::string fsr0);
    void setChipType(MaxipixDet::Version, MaxipixDet::Polarity);

    void getBoardVersion(short& pcb, short& firmware);
    void getChipID(short port, long& id);

    void setFastFOSpeed(bool fast);
    void getFastFOSpeed(bool& fast);
    
    void setOscillator(float freq);
    void getOscillator(float& freq);

    void setChipFsr(short port, std::string fsr);
    void setChipCfg(short port, std::string cfg);
    
    void enableSerial(short port);

    // --- timing

    void setTimeUnit(TimeUnit unit);
    void getTimeUnit(TimeUnit& unit);

    void setExposureTime(double askexpo, double& setexpo);
    void getExposureTime(double& expo);

    void setMaxExposureTime();
    void getExposureTimeRange(double& min_expo, double& max_expo);

    void setIntervalTime(double asktime, double& settime);
    void getIntervalTime(double& itime);

    void getIntervalTimeRange(double& minit, double& maxit);

    void setShutterTime(double asktime, double& settime);
    void getShutterTime(double& stime);

    // --- external IO signals

    void setShutterLevel(SignalLevel level);
    void getShutterLevel(SignalLevel& level);

    void setShutterMode(ShutterMode mode);
    void getShutterMode(ShutterMode& mode);

    void setReadyLevel(SignalLevel level);
    void getReadyLevel(SignalLevel& level);

    void setReadyMode(ReadyMode mode);
    void getReadyMode(ReadyMode& mode);

    void setGateLevel(SignalLevel level);
    void getGateLevel(SignalLevel& level);

    void setGateMode(GateMode mode);
    void getGateMode(GateMode& mode);

    void setTriggerLevel(SignalLevel level);
    void getTriggerLevel(SignalLevel& level);

    void setTriggerMode(TrigMode mode);
    void getTriggerMode(TrigMode& mode);

    // --- acquisition

    void setNbFrames(int nb);
    void getNbFrames(int& nb);

    void setSerialReadout(short port);
    void setParalellReadout(std::vector<int> ports);
    void getReadoutMode(ReadoutMode& mode, std::vector<int>& ports);

    void setImageMode(ImageMode image);
    void getImageMode(ImageMode& image);

    void setFFCorrection(short flat);
    void getFFCorrection(short& flat);

    void startAcq();
    void stopAcq();

    void getStatus(DetStatus& status);

    // --- reset

    void resetFifo(short port);
    void resetAllFifo();
    void resetChip(short port);
    void resetAllChip();

  private:
    void _readBoardID();
    void _timeAdjust();
    void _timeToReg(double, double&, std::string&, std::string&);
    void _regToTime(std::string, std::string, double&);
    void _updateSignalReg(std::string&);
    void _writeSignalReg();
    void _writeRomReg();
    void _writeMsrReg(char);
    
    inline void _checkPortNr(short port);


    PriamSerial& m_priam_serial;
    short	m_setup;
    MaxipixDet::Version  m_version;
    std::vector<long> m_chip_id;
    std::string m_board_id;
    short	m_pcb; 
    short	m_firmware;
    float	m_osc_frequency;
    std::string	m_chip_fsr0;
    bool	m_fo_fast;

    TimeUnit	m_time_unit;
    double	m_time_us;
    double	m_min_it;
    double	m_expo_time;
    double	m_int_time;

    SignalLevel	m_shut_level;
    ShutterMode	m_shut_mode;
    SignalLevel	m_ready_level;
    ReadyMode	m_ready_mode;
    SignalLevel	m_gate_level;
    GateMode	m_gate_mode;
    SignalLevel	m_trig_level;
    TrigMode	m_trig_mode;

    ReadoutMode m_read_mode;
    ImageMode   m_img_mode;
    short	m_flatfield;	
    int		m_nb_frame;

    std::vector<int>  m_port_used;
  };

  inline void PriamAcq::_checkPortNr(short port)
  {
    if ((port<0)||(port>=maxPorts))
        throw LIMA_HW_EXC(InvalidValue, "Invalid priam port number");
  };

};
};

#endif // _PRIAMACQ_H
