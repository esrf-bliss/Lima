#include <cmath>
#include "PriamAcq.h"

using namespace std;
using namespace lima;
using namespace lima::Maxipix;

PriamAcq::PriamAcq(PriamSerial& priam_serial)
	:m_priam_serial(priam_serial),
	 m_setup(0), m_version(MaxipixDet::DUMMY), 
	 m_chip_fsr0(""), m_fo_fast(false),
	 m_expo_time(-1.), m_int_time(-1.),
 	 m_shut_level(HIGH_RISE), m_shut_mode(FRAME),
	 m_ready_level(HIGH_RISE), m_ready_mode(EXPOSURE_READOUT),
	 m_gate_level(HIGH_RISE), m_gate_mode(INACTIVE),
	 m_trig_level(HIGH_RISE), m_trig_mode(IntTrig),
	 m_read_mode(PARALLEL), m_img_mode(NORMAL),
	 m_flatfield(0), m_nb_frame(-1)
{
    DEB_CONSTRUCTOR();

    m_chip_id.resize(maxPorts,0);
    m_port_used.push_back(0);

    setTimeUnit(UNIT_MS);
    enableSerial(0);
    _readBoardID();
}

PriamAcq::~PriamAcq()
{
    DEB_DESTRUCTOR();
}

void PriamAcq::_readBoardID()
{
    DEB_MEMBER_FUNCT();
    m_priam_serial.readRegister(PriamSerial::PR_BID, m_board_id);
    m_pcb= (short)((m_board_id.at(0) >> 4) & 0x0f);
    m_firmware= (short)(m_board_id.at(0) & 0x0f);
}

void PriamAcq::getBoardVersion(short& pcb, short& firmware) const
{
    DEB_MEMBER_FUNCT();
    pcb= m_pcb;
    firmware= m_firmware;
    DEB_RETURN() << DEB_VAR2(pcb, firmware);
}

void PriamAcq::setup(MaxipixDet::Version version,
		     MaxipixDet::Polarity polarity, 
		     float osc,const string &fsr0)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR4(version, polarity, osc, fsr0);

    enableSerial(0);
    setChipType(version, polarity);
    m_chip_fsr0= fsr0;
    setOscillator(osc);
}

void PriamAcq::setChipType(MaxipixDet::Version version,
			   MaxipixDet::Polarity polarity)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR2(version, polarity);

    string sval;
    char val;

    switch (version) {
	case MaxipixDet::MPX2: val= 0x40; break;
  	case MaxipixDet::MXR2: val= 0x20; break;
	case MaxipixDet::TPX1: val= 0x60; break;
	default: val= 0x00;
    }
    m_version= version;
    if (polarity==MaxipixDet::POSITIVE) {
	val= 0x80 | val;
    }

    sval.assign(1, val);
    m_priam_serial.writeRegister(PriamSerial::PR_MCR1, sval);
    m_setup |= 0x01;
}

void PriamAcq::setFastFOSpeed(bool fast)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(fast);

    string sval;
    char cval;

    m_priam_serial.readRegister(PriamSerial::PR_MCR1, sval);
    cval= sval.at(0) & 0xfe;
    if (fast) cval |= 0x01;
    sval.assign(1, cval);
    m_priam_serial.writeRegister(PriamSerial::PR_MCR1, sval);
    m_fo_fast= fast;
}

void PriamAcq::getFastFOSpeed(bool& fast) const
{
    DEB_MEMBER_FUNCT();

    string sval;
    m_priam_serial.readRegister(PriamSerial::PR_MCR1, sval);
    fast= (sval.at(0) & 0x01);
    DEB_RETURN() << DEB_VAR1(fast);
}
    
void PriamAcq::setOscillator(float frequency)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(frequency);

    string sval;
    char val[3];
    float xtal, fpgaDiv, outputDiv;
    short fval;

    if (frequency < 15.625)
	frequency= 15.625;
    if (frequency > 350.0)
	frequency= 350.0;

    xtal= 25.0;	// -- crystal reference oscillator 25MHz
    fpgaDiv= 2; // -- internal FPGA divider fixed to 2
    if (frequency <= 43.75) {
	val[1]= 6;
	outputDiv= 8.0;
    }
    else if (frequency <= 87.5) {
	val[1]= 4;
	outputDiv= 4.0;
    }
    else if (frequency <= 175.0) {
	val[1]= 2;
	outputDiv= 2.0;
    }
    else {
	val[1]= 0;
	outputDiv= 1.0;
    }
    fval= (short) (frequency * outputDiv * fpgaDiv / xtal);
    m_osc_frequency= xtal * fval / outputDiv / fpgaDiv;

    val[0]= (char)fval;
    val[2]= 0;
    sval.append(val);
    m_priam_serial.writeRegister(PriamSerial::PR_OSC, sval);

    // -- adjust tas/tap after frequency change
    usleep(500000);
    _timeAdjust();

    // -- for TPX, set counting frequency divider
    if (m_version == MaxipixDet::TPX1) {
	sval= string(1, (char)0x01);
	m_priam_serial.writeRegister(PriamSerial::PR_TIP, sval);
    }

    // -- set minimum interval (in us)
    if (m_firmware & 0x08) {
	m_min_it= 1.0 / m_osc_frequency * 920000.; // P_S firmware
    } else {
	m_min_it= 1.0 / m_osc_frequency * 30000.;
    }

    m_setup |= 0x02;
}

void PriamAcq::getOscillator(float& frequency) const
{
    DEB_MEMBER_FUNCT();

    frequency= m_osc_frequency;
    DEB_RETURN() << DEB_VAR1(frequency);
}

void PriamAcq::_timeAdjust()
{
    DEB_MEMBER_FUNCT();

    string sval;
    short tas, tap;

    if (!m_chip_fsr0.size())
	THROW_HW_ERROR(Error) << "Priam time adjust needs fsr@0 set.";
    setChipFsr(0, m_chip_fsr0);

    m_priam_serial.readRegister(PriamSerial::PR_TAS, sval);
    tas= (short)sval.at(0);
    
    tap= (tas >> 4)&0x0f;
    tas= tap - 2;
    if (tas < 0) tas= 0;

    if (m_firmware >= 2) {
	if (m_osc_frequency < 100.0)
	    tap -= 1;
	else if ((m_osc_frequency >= 100.0) && (m_osc_frequency < 120.0))
	    tap -= 2;
	else
	    tap -= 3;
    }
    if (tap < 0) tap= 0;

    sval= string(1, (char)tap);
    m_priam_serial.writeRegister(PriamSerial::PR_TAP, sval);
    sval= string(1, (char)tas);
    m_priam_serial.writeRegister(PriamSerial::PR_TAS, sval);
}

void PriamAcq::enableSerial(short port)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(port);

    string sval;
    short val;

    _checkPortNr(port);

    val= 0x20 | (1<<port);
    sval= string(1, (char)val);
    m_priam_serial.writeRegister(PriamSerial::PR_MCR2, sval);
}
    
void PriamAcq::setChipFsr(short port,const string &fsr)
{
    DEB_MEMBER_FUNCT();

    std::ostringstream fsrString;
    fsrString << std::hex;
    fsrString << fsr;

    DEB_PARAM() << DEB_VAR2(port,fsrString);

    string sdummy, sid;

    enableSerial(port);
    sdummy.append(32, '\xff');
    m_priam_serial.writeFsr(sdummy, sid);
    m_priam_serial.writeFsr(fsr, sid);
    m_chip_id[port]= 0;
    for (int i=0; i<3; i++)
	m_chip_id[port] |= ((sid.at(i)&0xff)<<i);
    if (port==0)
	m_chip_fsr0= fsr;
}

void PriamAcq::getChipID(short port, long& id) const
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(port);

    _checkPortNr(port);
    id= m_chip_id[port];
    DEB_RETURN() << DEB_VAR1(id);
}

void PriamAcq::setChipCfg(short port,const string &cfg)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR2(port, cfg.size());

    string out;
    enableSerial(port);
    m_priam_serial.writeMatrix(cfg);
    m_priam_serial.readMatrix(out);
}

void PriamAcq::setTimeUnit(TimeUnit unit)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(unit);

    m_time_unit= unit;
    m_time_us= pow(10., (int)m_time_unit);
}

void PriamAcq::getTimeUnit(TimeUnit& unit) const
{
    DEB_MEMBER_FUNCT();

    unit= m_time_unit;
    DEB_RETURN() << DEB_VAR1(unit);
}

void PriamAcq::_timeToReg(double in, double& out, string& reg1, string& reg2)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(in);

    double askt, sett;
    int iu, it;

    askt= in * m_time_us;
    for (iu=0; iu<UNIT_NB; iu++) {
	sett= askt / pow(10., iu);
	if (sett < 1024) break;
    }
    if (sett > 1024)
	THROW_HW_ERROR(InvalidValue) << "Time overflow";

    it= (int)sett;
    reg1.assign(1, (char)(it&0xff));
    reg2.assign(1, (char)(((it>>8)&0x03)|(iu<<5)));
    out= ((double)it * pow(10.,iu)) / m_time_us;
    DEB_RETURN() << DEB_VAR3(out, reg1, reg2);
}

void PriamAcq::_regToTime(string reg1, string reg2, double& out) const
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR2(reg1, reg2);

    int iu, it;

    iu= (int)((reg2.at(0) >> 5) & 0x07);
    it= (int)(((reg2.at(0)&0x03)<<8)|(reg1.at(0)&0xff));
    out= ((double)it * pow(10., iu)) / m_time_us;
    DEB_RETURN() << DEB_VAR1(out);
}

void PriamAcq::setMaxExposureTime()
{
    DEB_MEMBER_FUNCT();

    string val;
    val.assign(1, (char)0xff);
    m_priam_serial.writeRegister(PriamSerial::PR_ET1, val);
    val.assign(1, (char)((UNIT_S<<5)|0x03));
    m_priam_serial.writeRegister(PriamSerial::PR_ET2, val);
}

void PriamAcq::setExposureTime(double askexpo, double& setexpo)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(askexpo);

    string et1, et2;

    _timeToReg(askexpo, setexpo, et1, et2);
    m_priam_serial.writeRegister(PriamSerial::PR_ET1, et1);
    m_priam_serial.writeRegister(PriamSerial::PR_ET2, et2);
    m_expo_time= setexpo;
    DEB_RETURN() << DEB_VAR1(setexpo);
}

void PriamAcq::getExposureTime(double& expo) const
{
    DEB_MEMBER_FUNCT();

    string et1, et2;
    m_priam_serial.readRegister(PriamSerial::PR_ET1, et1);
    m_priam_serial.readRegister(PriamSerial::PR_ET2, et2);
    _regToTime(et1, et2, expo);
    DEB_RETURN() << DEB_VAR1(expo);
}

void PriamAcq::getExposureTimeRange(double& min_expo, double& max_expo) const
{
    DEB_MEMBER_FUNCT();

    min_expo= 300 / m_time_us;
    max_expo= (double)0x3ff * 1000000. / m_time_us;
    DEB_RETURN() << DEB_VAR2(min_expo, max_expo);
}

void PriamAcq::setIntervalTime(double asktime, double& settime)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(asktime);

    string it1, it2;
    double minit= m_min_it / m_time_us;

    if (asktime < minit) {
	_timeToReg(minit, settime, it1, it2);
    } else {
        _timeToReg(asktime, settime, it1, it2);
    }

    m_priam_serial.writeRegister(PriamSerial::PR_IT1, it1);
    m_priam_serial.writeRegister(PriamSerial::PR_IT2, it2);
    m_int_time= settime;
    DEB_RETURN() << DEB_VAR1(settime);
}

void PriamAcq::getIntervalTime(double& itime) const
{
    DEB_MEMBER_FUNCT();

    string it1, it2;

    m_priam_serial.readRegister(PriamSerial::PR_IT1, it1);
    m_priam_serial.readRegister(PriamSerial::PR_IT2, it2);
    _regToTime(it1, it2, itime);
    DEB_RETURN() << DEB_VAR1(itime);
}

void PriamAcq::getIntervalTimeRange(double& minit, double& maxit) const
{
    DEB_MEMBER_FUNCT();

    minit= m_min_it / m_time_us;
    maxit= (double)0x3ff * 1000000. / m_time_us;
    DEB_RETURN() << DEB_VAR2(minit, maxit);
}

void PriamAcq::setShutterTime(double asktime, double& settime)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(asktime);

    string st1, st2;

    _timeToReg(asktime, settime, st1, st2);
    _updateSignalReg(st2);
    m_priam_serial.writeRegister(PriamSerial::PR_ST1, st1);
    m_priam_serial.writeRegister(PriamSerial::PR_ST2, st2);
    DEB_RETURN() << DEB_VAR1(settime);
}

void PriamAcq::getShutterTime(double& stime) const
{
    DEB_MEMBER_FUNCT();

    string st1, st2;

    m_priam_serial.readRegister(PriamSerial::PR_ST1, st1);
    m_priam_serial.readRegister(PriamSerial::PR_ST2, st2);
    _regToTime(st1, st2, stime);
    DEB_RETURN() << DEB_VAR1(stime);
}

void PriamAcq::_updateSignalReg(string& st2)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(DEB_HEX(st2.at(0)));

    char cval;
    cval= st2.at(0);
    SETBIT(cval, 2, m_ready_level==HIGH_RISE);
    SETBIT(cval, 3, m_shut_level==HIGH_RISE);
    SETBIT(cval, 4, m_shut_mode==SEQUENCE);
    st2.assign(1, cval);
    DEB_RETURN() << DEB_VAR1(DEB_HEX(st2.at(0)));
}

void PriamAcq::_writeSignalReg()
{
    DEB_MEMBER_FUNCT();

    string st2;

    m_priam_serial.readRegister(PriamSerial::PR_ST2, st2);
    _updateSignalReg(st2);
    m_priam_serial.writeRegister(PriamSerial::PR_ST2, st2);
}

void PriamAcq::setShutterLevel(SignalLevel level)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(level);
    m_shut_level= level;
    _writeSignalReg();
}

void PriamAcq::getShutterLevel(SignalLevel& level) const
{
    DEB_MEMBER_FUNCT();
    level= m_shut_level;
    DEB_RETURN() << DEB_VAR1(level);
}

void PriamAcq::setShutterMode(ShutterMode mode)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(mode);
    m_shut_mode= mode;
    _writeSignalReg();
}

void PriamAcq::getShutterMode(ShutterMode& mode) const
{
    DEB_MEMBER_FUNCT();
    mode= m_shut_mode;
    DEB_RETURN() << DEB_VAR1(mode);
}

void PriamAcq::setReadyLevel(SignalLevel level)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(level);
    m_ready_level= level;
    _writeSignalReg();
}

void PriamAcq::getReadyLevel(SignalLevel& level) const
{
    DEB_MEMBER_FUNCT();
    level= m_ready_level;
    DEB_RETURN() << DEB_VAR1(level);
}

void PriamAcq::setReadyMode(ReadyMode mode)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(mode);
    m_ready_mode= mode;
}

void PriamAcq::getReadyMode(ReadyMode& mode) const
{
    DEB_MEMBER_FUNCT();
    mode= m_ready_mode;
    DEB_RETURN() << DEB_VAR1(mode);
}

void PriamAcq::setGateLevel(SignalLevel level)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(level);
    m_gate_level= level;
}

void PriamAcq::getGateLevel(SignalLevel& level) const
{
    DEB_MEMBER_FUNCT();
    level= m_gate_level;
    DEB_RETURN() << DEB_VAR1(level);
}

void PriamAcq::setGateMode(GateMode mode)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(mode);
    m_gate_mode= mode;
}

void PriamAcq::getGateMode(GateMode& mode) const
{
    DEB_MEMBER_FUNCT();
    mode= m_gate_mode;
    DEB_RETURN() << DEB_VAR1(mode);
}

void PriamAcq::setTriggerLevel(SignalLevel level)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(level);
    m_trig_level= level;
}

void PriamAcq::getTriggerLevel(SignalLevel& level) const
{
    DEB_MEMBER_FUNCT();
    level= m_trig_level;
    DEB_RETURN() << DEB_VAR1(level);
}

void PriamAcq::setTriggerMode(TrigMode mode)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(mode);
    m_trig_mode= mode;
}

void PriamAcq::getTriggerMode(TrigMode& mode) const
{
    DEB_MEMBER_FUNCT();
    mode= m_trig_mode;
    DEB_RETURN() << DEB_VAR1(mode);
}

void PriamAcq::setNbFrames(int nb)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(nb);

    string in1, in2;

    if (nb==0 && m_firmware<3)
	THROW_HW_ERROR(InvalidValue) 
	    << "Infinite sequence only possible for firmware rev.>3 ("
	    << DEB_VAR1(m_firmware) << ")";

    if (nb > 0xffff)
  	THROW_HW_ERROR(InvalidValue) << "Too many frames asked";

    in1.assign(1, (char)(nb&0xff));
    in2.assign(1, (char)((nb>>8)&0xff));
    m_priam_serial.writeRegister(PriamSerial::PR_INB1, in1);
    m_priam_serial.writeRegister(PriamSerial::PR_INB2, in2);
    m_nb_frame= nb;
}

void PriamAcq::getNbFrames(int& nb) const
{
    DEB_MEMBER_FUNCT();

    string in1, in2;

    m_priam_serial.readRegister(PriamSerial::PR_INB1, in1);
    m_priam_serial.readRegister(PriamSerial::PR_INB2, in2);
    nb= (int)((in1.at(0)&0xff)|(in2.at(0)<<8));
    DEB_RETURN() << DEB_VAR1(nb);
}

void PriamAcq::_writeRomReg()
{
   DEB_MEMBER_FUNCT();

   string rom;
   char	cval;

   cval= (m_flatfield & 0x06) << 2;
   if (m_read_mode==SERIAL)
	cval |= 0x80;
   if (m_img_mode==RAW)
	cval |= 0x01;
   rom.assign(1, cval);
   m_priam_serial.writeRegister(PriamSerial::PR_ROM, rom);
}

void PriamAcq::setImageMode(ImageMode img)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(img);
    m_img_mode= img;
    _writeRomReg();
}

void PriamAcq::getImageMode(ImageMode& img) const
{
    DEB_MEMBER_FUNCT();
    img= m_img_mode;
    DEB_RETURN() << DEB_VAR1(img);
}

void PriamAcq::setSerialReadout(short port)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(port);

    _checkPortNr(port);
    m_port_used.clear();
    m_port_used.push_back(port);
    m_read_mode= SERIAL;
    _writeRomReg();
}

void PriamAcq::setParalellReadout(vector<int> ports)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(ports.size());

    if (!ports.size())
	THROW_HW_ERROR(InvalidValue) << "Empty priam ports list";
    for (int i=0; i<(int)ports.size(); i++)
	_checkPortNr(ports[i]);
    
    m_port_used.clear();
    for (int i=0; i<(int)ports.size(); i++)
	m_port_used.push_back(ports[i]);
    
    m_read_mode= PARALLEL;
    _writeRomReg();
}

void PriamAcq::getReadoutMode(ReadoutMode& mode, vector<int>& ports) const
{
    DEB_MEMBER_FUNCT();
    mode= m_read_mode;
    ports= m_port_used;
    DEB_RETURN() << DEB_VAR2(mode, ports.size());
}

void PriamAcq::setFFCorrection(short flat)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(flat);

    if ((flat<0) || (flat>6))
	THROW_HW_ERROR(InvalidValue) << "Wrong FlatField correction factor "
				     << DEB_VAR1(flat);
    m_flatfield= flat;
    _writeRomReg();
}

void PriamAcq::getFFCorrection(short& flat) const
{
    DEB_MEMBER_FUNCT();
    flat= m_flatfield;
    DEB_RETURN() << DEB_VAR1(flat);
}

void PriamAcq::startAcq()
{
    DEB_MEMBER_FUNCT();

    string reg;
    char mcr2, msr;
    int nbchip;
    double txtime, minit;

    if (m_nb_frame == -1)
	THROW_HW_ERROR(Error) << "Number of frames has not been set";
    if (m_setup != 0x03)
	THROW_HW_ERROR(Error) << "Priam setup not complete";
    if (m_expo_time<0.)
	THROW_HW_ERROR(Error) << "Exposure time not set";
    if (m_int_time<0.)
	THROW_HW_ERROR(Error) << "Interval time not set";

    nbchip= (int)m_port_used.size();
    txtime= m_fo_fast ? 560. : 700.;
    txtime /= m_time_us;
    minit = m_min_it / m_time_us;
    if ((m_int_time-minit+m_expo_time)<(txtime*nbchip))
	THROW_HW_ERROR(Error) << "Timing too fast (interval+expo < transfer): "
			      << DEB_VAR5(m_int_time, minit, m_expo_time, 
					  txtime, nbchip);

    mcr2= (m_read_mode==SERIAL) ? 0x20 : 0x00;
    for (int i=0; i<(int)m_port_used.size(); i++)
	mcr2 |= (1<<m_port_used[i]);
    reg.assign(1, mcr2);
    m_priam_serial.writeRegister(PriamSerial::PR_MCR2, reg);

    switch (m_trig_mode) {
	case IntTrig: 		msr= 0x01; break;
        case ExtTrigSingle:	msr= 0x02; break;
	case ExtTrigMult:	msr= 0x04; break;
	case ExtGate:		msr= 0x06; break;
default:
	    THROW_HW_ERROR(Error) << "Invalid " << DEB_VAR1(m_trig_mode);
    }
    if ((msr!=0x01)&&(m_trig_level==LOW_FALL)) msr |= 0x01;
    _writeMsrReg(msr);
}

void PriamAcq::_writeMsrReg(char val)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(val);

    string reg;
    char msr;

    msr= val;
    SETBIT(msr, 3, (m_gate_mode==ACTIVE) && (m_gate_level==LOW_FALL));
    SETBIT(msr, 4, (m_gate_mode==ACTIVE));
    SETBIT(msr, 5, (m_ready_mode==EXPOSURE_READOUT));
    reg.assign(1, msr);
    m_priam_serial.writeRegister(PriamSerial::PR_MSR, reg);
};

void PriamAcq::stopAcq()
{
    DEB_MEMBER_FUNCT();
    _writeMsrReg((char)0x00);
    enableSerial(0);
}

void PriamAcq::getStatus(DetStatus& status) const
{
    DEB_MEMBER_FUNCT();

    string msr;
    int	busy;

    m_priam_serial.readRegister(PriamSerial::PR_MSR, msr);
    busy= (msr.at(0)>>6)&0x03;
    switch (busy) {
	case 0: status= DetIdle; break;
	case 1: status= DetWaitForTrigger; break;
	case 2: status= DetExposure; break;
	case 3: status= DetReadout; break;
	default: status= DetFault;
    }

    DEB_RETURN() << DEB_VAR1(status);
}

void PriamAcq::resetFifo(short port)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(port);

    string sval;
    char val;

    _checkPortNr(port);

    val= 0xc0 | 0x20 | (1<<port);
    sval= string(1, val);
    m_priam_serial.writeRegister(PriamSerial::PR_MCR2, sval);
}

void PriamAcq::resetAllFifo()
{
    DEB_MEMBER_FUNCT();
    for (int i=0; i<(int)m_port_used.size(); i++)
	resetFifo(m_port_used[i]);
}

void PriamAcq::resetChip(short port)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(port);

    string sval;
    char val;

    _checkPortNr(port);

    val= 0x80 | 0x20 | (1<<port);
    sval= string(1, val);
    m_priam_serial.writeRegister(PriamSerial::PR_MCR2, sval);
}
void PriamAcq::resetAllChip()
{
    DEB_MEMBER_FUNCT();
    for (int i=0; i<(int)m_port_used.size(); i++)
	resetChip(m_port_used[i]);
}

