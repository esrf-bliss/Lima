//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
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
      PARALLEL,
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
		float freq, const std::string &fsr0);
    void setChipType(MaxipixDet::Version, MaxipixDet::Polarity);

    void getBoardVersion(short& pcb, short& firmware) const;
    void getChipID(short port, long& id) const;

    void setFastFOSpeed(bool fast);
    void getFastFOSpeed(bool& fast) const;
    
    void setOscillator(float freq);
    void getOscillator(float& freq) const;

    void setChipFsr(short port,const std::string &fsr);
    void setChipCfg(short port,const std::string &cfg);
    
    void enableSerial(short port);

    // --- timing

    void setTimeUnit(TimeUnit unit);
    void getTimeUnit(TimeUnit& unit) const;

    void setExposureTime(double askexpo, double& setexpo);
    void getExposureTime(double& expo) const;

    void setMaxExposureTime();
    void getExposureTimeRange(double& min_expo, double& max_expo) const;

    void setIntervalTime(double asktime, double& settime);
    void getIntervalTime(double& itime) const;

    void getIntervalTimeRange(double& minit, double& maxit) const;

    void setShutterTime(double asktime, double& settime);
    void getShutterTime(double& stime) const;

    // --- external IO signals

    void setShutterLevel(SignalLevel level);
    void getShutterLevel(SignalLevel& level) const;

    void setShutterMode(ShutterMode mode);
    void getShutterMode(ShutterMode& mode) const;

    void setReadyLevel(SignalLevel level);
    void getReadyLevel(SignalLevel& level) const;

    void setReadyMode(ReadyMode mode);
    void getReadyMode(ReadyMode& mode) const;

    void setGateLevel(SignalLevel level);
    void getGateLevel(SignalLevel& level) const;

    void setGateMode(GateMode mode);
    void getGateMode(GateMode& mode) const;

    void setTriggerLevel(SignalLevel level);
    void getTriggerLevel(SignalLevel& level) const;

    void setTriggerMode(TrigMode mode);
    void getTriggerMode(TrigMode& mode) const;

    // --- acquisition

    void setNbFrames(int nb);
    void getNbFrames(int& nb) const;

    void setSerialReadout(short port);
    void setParalellReadout(std::vector<int> ports);
    void getReadoutMode(ReadoutMode& mode, std::vector<int>& ports) const;

    void setImageMode(ImageMode image);
    void getImageMode(ImageMode& image) const;

    void setFFCorrection(short flat);
    void getFFCorrection(short& flat) const;

    void startAcq();
    void stopAcq();

    void getStatus(DetStatus& status) const;

    // --- reset

    void resetFifo(short port);
    void resetAllFifo();
    void resetChip(short port);
    void resetAllChip();

  private:
    void _readBoardID();
    void _timeAdjust();
    void _timeToReg(double,double&,std::string&,std::string&);
    void _regToTime(const std::string,const std::string, double&) const;
    void _updateSignalReg(std::string&);
    void _writeSignalReg();
    void _writeRomReg();
    void _writeMsrReg(char);
    
    inline void _checkPortNr(short port) const;


    PriamSerial& 		m_priam_serial;
    short			m_setup;
    MaxipixDet::Version  	m_version;
    std::vector<long> 		m_chip_id;
    std::string 		m_board_id;
    short			m_pcb; 
    short			m_firmware;
    float			m_osc_frequency;
    std::string			m_chip_fsr0;
    bool			m_fo_fast;

    TimeUnit			m_time_unit;
    double			m_time_us;
    double			m_min_it;
    double			m_expo_time;
    double			m_int_time;

    SignalLevel			m_shut_level;
    ShutterMode			m_shut_mode;
    SignalLevel			m_ready_level;
    ReadyMode			m_ready_mode;
    SignalLevel			m_gate_level;
    GateMode			m_gate_mode;
    SignalLevel			m_trig_level;
    TrigMode			m_trig_mode;

    ReadoutMode 		m_read_mode;
    ImageMode   		m_img_mode;
    short			m_flatfield;	
    int				m_nb_frame;

    std::vector<int>  		m_port_used;
};

inline void PriamAcq::_checkPortNr(short port) const
{
    DEB_MEMBER_FUNCT();
    if ((port<0)||(port>=maxPorts))
	THROW_HW_ERROR(InvalidValue) << "Invalid priam " << DEB_VAR1(port);
};

}; // namespace Maxipix
}; // namespace lima

#endif // _PRIAMACQ_H
