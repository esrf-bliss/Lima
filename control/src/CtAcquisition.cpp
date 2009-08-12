
#include "CtAcquisition.h"
#include "math.h"

using namespace lima;


CtAcquisition::CtAcquisition(HwInterface *hw)
{
	if (!hw->getHwCtrlObj(m_hw_sync))
		throw LIMA_CTL_EXC(Error, "Cannot get hardware sync object");

	m_hw_sync->getValidRanges(m_valid_ranges);
	m_applied_once= false;
}

CtAcquisition::~CtAcquisition()
{
}

void CtAcquisition::setPars(const Parameters &pars)
{
	setAcqMode(pars.acqMode);
	setAcqNbFrames(pars.acqNbFrames);
	setAcqExpoTime(pars.acqExpoTime);
	setAccMaxExpoTime(pars.accMaxExpoTime);
	setConcatNbFrames(pars.concatNbFrames);
	setLatencyTime(pars.latencyTime);
	setTriggerMode(pars.triggerMode);
}

void CtAcquisition::getPars(Parameters& pars) const
{
	pars= m_inpars;
}

void CtAcquisition::apply(CtControl::ApplyPolicy policy)
{
	CtControl::ApplyPolicy use_policy;

	use_policy= m_applied_once ? policy : CtControl::All;
	
	switch (policy) {
		case CtControl::All:
			// --- apply all parameters
			m_changes.set(1);
			_apply();
			break;
		case CtControl::Changes:
			// --- apply changed parameters (use internal soft cache)
			m_changes.check(m_inpars, m_hwpars);
			_apply();
			break;
		case CtControl::HwSyncChanges:
			// --- read hw and apply changed parameters
			_hwRead();
			m_changes.check(m_inpars, m_hwpars);
			_apply();
			break;
		default:
			m_changes.set(1);
			_apply();
	}
	
}

void CtAcquisition::sync()
{
	_hwRead();
	m_inpars= m_hwpars;
}

void CtAcquisition::_hwRead()
{
	int read_nframes;
	m_hw_sync->getTrigMode(m_hwpars.triggerMode);
	m_hw_sync->getExpTime(m_hwpars.acqExpoTime);
	m_hw_sync->getLatTime(m_hwpars.latencyTime);
	m_hw_sync->getNbFrames(m_hwpars.acqNbFrames);

	switch (m_hwpars.acqMode) {
		case Single:
			m_hwpars.acqNbFrames= read_nframes;
			m_hwpars.concatNbFrames= 0;
			m_acc_exptime= m_hwpars.accMaxExpoTime;
			m_acc_nframes= 0;
			break;
		case Concatenation:
			// concatNbFrames unchanged (or read in buffer ??)
			m_hwpars.acqNbFrames= read_nframes;
			m_acc_exptime= m_hwpars.accMaxExpoTime;
			m_acc_nframes= 0;
			break;
		case Accumulation:
			m_hwpars.concatNbFrames= 0;
			_updateAccPars();
			m_hwpars.acqNbFrames= int(read_nframes / m_acc_nframes);
			break;
	}
}

void CtAcquisition::_apply()
{
	if (m_changes.triggerMode) m_hw_sync->setTrigMode(m_inpars.triggerMode);
	if (m_changes.latencyTime) m_hw_sync->setLatTime(m_inpars.latencyTime);

	switch (m_inpars.acqMode) {
		case Single:
			if (m_changes.acqExpoTime) m_hw_sync->setExpTime(m_inpars.acqExpoTime);
			if (m_changes.acqNbFrames) m_hw_sync->setNbFrames(m_inpars.acqNbFrames);
			break;
		case Concatenation:
			if (m_changes.acqExpoTime) m_hw_sync->setExpTime(m_inpars.acqExpoTime);
			if (m_changes.acqNbFrames) m_hw_sync->setNbFrames(m_inpars.acqNbFrames);
			break;
		case Accumulation:
			if (m_changes.acqExpoTime || m_changes.accMaxExpoTime) {
				_updateAccPars();
				m_hw_sync->setExpTime(m_acc_exptime);
				m_hw_sync->setNbFrames(m_inpars.acqNbFrames * m_acc_nframes);
			} else if (m_changes.acqNbFrames) {
				m_hw_sync->setNbFrames(m_inpars.acqNbFrames * m_acc_nframes);
			}
			break;
	}
	m_hwpars= m_inpars;
	m_changes.set(0);
}

void CtAcquisition::setTriggerMode(TrigMode mode)
{
	m_inpars.triggerMode= mode;
}

void CtAcquisition::getTriggerMode(TrigMode& mode) const
{
	mode= m_inpars.triggerMode;
}


void CtAcquisition::setAcqMode(AcqMode mode)
{
	m_inpars.acqMode= mode;
	switch (m_inpars.acqMode) {
		case Single:
			if (m_inpars.acqNbFrames<1) m_inpars.acqNbFrames= 1;
			break;
		case Accumulation:
			if (m_inpars.accMaxExpoTime<=0) {
				m_inpars.accMaxExpoTime= m_inpars.acqExpoTime;
			}
		case Concatenation:
			if (m_inpars.concatNbFrames<1)
				m_inpars.concatNbFrames= 1;
			break;
	}

}

void CtAcquisition::getAcqMode(AcqMode& mode) const
{
	mode= m_inpars.acqMode;
}

void CtAcquisition::setAcqNbFrames(int nframes)
{
	m_inpars.acqNbFrames= nframes;
}

void CtAcquisition::getAcqNbFrames(int& nframes) const 
{
	nframes= m_inpars.acqNbFrames;
}

void CtAcquisition::setAcqExpoTime(double acq_time)
{
	if (acq_time < m_valid_ranges.min_exp_time)
		throw LIMA_CTL_EXC(InvalidValue, "Exposure time too short");
	if (acq_time > m_valid_ranges.max_exp_time)
		throw LIMA_CTL_EXC(InvalidValue, "Exposure time too long");
	m_inpars.acqExpoTime= acq_time;
}

void CtAcquisition::getAcqExpoTime(double& acq_time) const
{
	acq_time= m_inpars.acqExpoTime;
}

void CtAcquisition::getAccNbFrames(int& nframes) const
{
	_updateAccPars();
	nframes= m_acc_nframes;
}

void CtAcquisition::setAccMaxExpoTime(double acc_time)
{
	m_inpars.accMaxExpoTime= acc_time;
}

void CtAcquisition::getAccExpoTime(double& acc_time) const
{
	_updateAccPars();
	acc_time= m_acc_exptime;
}

void CtAcquisition::_updateAccPars() const
{
	int acc_div;
	acc_div= int(m_inpars.acqExpoTime / m_inpars.accMaxExpoTime);
	if (fmod(m_inpars.acqExpoTime,m_inpars.accMaxExpoTime)) {
		m_acc_nframes= acc_div + 1;
		m_acc_exptime= m_inpars.acqExpoTime / m_acc_nframes;
	} else {
		m_acc_nframes= acc_div;
		m_acc_exptime= m_inpars.accMaxExpoTime;
	}
}

void CtAcquisition::setConcatNbFrames(int nframes)
{
	m_inpars.concatNbFrames= nframes;
}

void CtAcquisition::getConcatNbFrames(int& nframes) const
{
	nframes= m_inpars.concatNbFrames;
}

void CtAcquisition::setLatencyTime(double lat_time) 
{
	if (lat_time < m_valid_ranges.min_lat_time)
		throw LIMA_CTL_EXC(InvalidValue, "Latency time too short");
	if (lat_time > m_valid_ranges.max_lat_time)
		throw LIMA_CTL_EXC(InvalidValue, "Latency time too long");
	m_inpars.latencyTime= lat_time;
}

void CtAcquisition::getLatencyTime(double& time) const
{
	time= m_inpars.latencyTime;
}


// ------------------
// struct Parameters
// ------------------
CtAcquisition::Parameters::Parameters()
{
	reset();
}

void CtAcquisition::Parameters::reset()
{
	acqMode= Single;
	acqNbFrames= 1;
	acqExpoTime= 1.;
	accMaxExpoTime= 1.;
	concatNbFrames= 0;
	latencyTime= 0.;
	triggerMode= IntTrig;
}

// ------------------
// struct ChangedPars
// ------------------
CtAcquisition::ChangedPars::ChangedPars()
{
	set(0);
}

void CtAcquisition::ChangedPars::set(bool val)
{
	acqExpoTime= val;
	acqNbFrames= val;
	latencyTime= val;
	triggerMode= val;
	accMaxExpoTime= val;
}

void CtAcquisition::ChangedPars::check(Parameters p1, Parameters p2)
{
	acqExpoTime= (p1.acqExpoTime != p2.acqExpoTime);
	acqNbFrames= (p1.acqNbFrames != p2.acqNbFrames);
	latencyTime= (p1.latencyTime != p2.latencyTime);
	triggerMode= (p1.triggerMode != p2.triggerMode);
	accMaxExpoTime= (p1.accMaxExpoTime != p2.accMaxExpoTime);
}
