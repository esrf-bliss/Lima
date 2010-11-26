
#include "CtAcquisition.h"
#include "math.h"

using namespace lima;
/*----------------------------------------------------------------------
			 validRangesCallback
----------------------------------------------------------------------*/
class CtAcquisition::_ValidRangesCallback : public HwSyncCtrlObj::ValidRangesCallback
{
  DEB_CLASS(DebModControl,"_ValidRangesCallback");
public:
  _ValidRangesCallback(CtAcquisition &acq) :
    HwSyncCtrlObj::ValidRangesCallback(),
    m_acq(acq)
  {}
  virtual void validRangesChanged(const HwSyncCtrlObj::ValidRangesType &ranges)
  {
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(ranges);

    m_acq.m_valid_ranges = ranges;
  }
private:
  CtAcquisition& m_acq;
};

CtAcquisition::CtAcquisition(HwInterface *hw) :
  m_acc_nframes(-1),
  m_acc_exptime(-1.),
  m_acc_live_time(-1.),
  m_acc_dead_time(-1.),
  m_valid_ranges_cb(NULL)
{
  DEB_CONSTRUCTOR();

  if (!hw->getHwCtrlObj(m_hw_sync))
    throw LIMA_CTL_EXC(Error, "Cannot get hardware sync object");

  m_valid_ranges_cb = new _ValidRangesCallback(*this);
  m_hw_sync->getValidRanges(m_valid_ranges);
  m_hw_sync->registerValidRangesCallback(m_valid_ranges_cb);
  DEB_TRACE() << DEB_VAR1(m_valid_ranges);

  m_applied_once= false;
}

CtAcquisition::~CtAcquisition()
{
  DEB_DESTRUCTOR();
  m_hw_sync->unregisterValidRangesCallback(m_valid_ranges_cb);
  delete m_valid_ranges_cb;
}

void CtAcquisition::setPars(const Parameters &pars)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(pars);

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
  DEB_MEMBER_FUNCT();

  pars = m_inpars;
  
  DEB_RETURN() << DEB_VAR1(pars);
}

void CtAcquisition::reset()
{
  DEB_MEMBER_FUNCT();
  
  m_inpars.reset();
}

void CtAcquisition::apply(CtControl::ApplyPolicy policy)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(policy);

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
  DEB_MEMBER_FUNCT();

  _hwRead();
  m_inpars= m_hwpars;
  
  DEB_TRACE() << DEB_VAR1(m_inpars);
}

void CtAcquisition::_hwRead()
{
  DEB_MEMBER_FUNCT();

  m_hw_sync->getTrigMode(m_hwpars.triggerMode);
  m_hw_sync->getExpTime(m_hwpars.acqExpoTime);
  m_hw_sync->getLatTime(m_hwpars.latencyTime);
  m_hw_sync->getNbFrames(m_hwpars.acqNbFrames);

  switch (m_hwpars.acqMode) {
  case Single:
    m_hwpars.concatNbFrames= 0;
    break;
  case Concatenation:
    // concatNbFrames unchanged (or read in buffer ??)
    break;
  case Accumulation:
    m_hwpars.concatNbFrames= 0;
    _updateAccPars();
    break;
  }
  
  DEB_TRACE() << DEB_VAR1(m_hwpars);
}

void CtAcquisition::_apply()
{
  DEB_MEMBER_FUNCT();

  //inform first the hardware synch. about the new acquisition mode
  // can have effect for instance on available trigger mode
  m_hw_sync->setAcqMode(m_inpars.acqMode);

  if (m_changes.triggerMode) m_hw_sync->setTrigMode(m_inpars.triggerMode);
  if (m_changes.latencyTime) m_hw_sync->setLatTime(m_inpars.latencyTime);
  
  if(m_changes.acqMode || m_changes.acqNbFrames)
    {
      if(m_inpars.acqMode == Accumulation)
	{
	  _updateAccPars();
	  m_hw_sync->setNbFrames(m_acc_nframes * m_inpars.acqNbFrames);
	}
      else
	m_hw_sync->setNbFrames(m_inpars.acqNbFrames);
    }
  
  switch (m_inpars.acqMode) {
  case Single:
    if (m_changes.acqExpoTime) m_hw_sync->setExpTime(m_inpars.acqExpoTime);
    break;
  case Concatenation:
    if (m_changes.acqExpoTime) m_hw_sync->setExpTime(m_inpars.acqExpoTime);
    break;
  case Accumulation:
    if (m_changes.acqExpoTime || m_changes.accMaxExpoTime) {
      _updateAccPars();
      m_hw_sync->setExpTime(m_acc_exptime);
    }
    break;
  }
  m_hwpars = m_inpars;

  DEB_TRACE() << DEB_VAR1(m_hwpars);
  
  m_changes.set(0);
}

void CtAcquisition::setTriggerMode(TrigMode mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(mode);

  m_inpars.triggerMode= mode;
}

void CtAcquisition::getTriggerMode(TrigMode& mode) const
{
  DEB_MEMBER_FUNCT();

  mode = m_inpars.triggerMode;

  DEB_RETURN() << DEB_VAR1(mode);
}


void CtAcquisition::setAcqMode(AcqMode mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(mode);

  m_inpars.acqMode= mode;
  switch (m_inpars.acqMode) {
  case Single:
    if (m_inpars.acqNbFrames<1) m_inpars.acqNbFrames= 1;
    m_acc_nframes = -1;
    m_acc_live_time = -1.;
    m_acc_dead_time = -1.;
    break;
  case Accumulation:
    if (m_inpars.accMaxExpoTime<=0) {
      m_inpars.accMaxExpoTime= m_inpars.acqExpoTime;
    }
    break;
  case Concatenation:
    if (m_inpars.concatNbFrames<1)
      m_inpars.concatNbFrames= 1;
    m_acc_nframes = -1;
    m_acc_live_time = -1.;
    m_acc_dead_time = -1.;
    break;
  }
  
  DEB_TRACE() << DEB_VAR1(m_inpars);
}

void CtAcquisition::getAcqMode(AcqMode& mode) const
{
  DEB_MEMBER_FUNCT();

  mode = m_inpars.acqMode;
  
  DEB_RETURN() << DEB_VAR1(mode);
}

void CtAcquisition::getAccTimeMode(AccTimeMode &mode) const
{
  DEB_MEMBER_FUNCT();
  
  mode = m_inpars.accTimeMode;
  
  DEB_RETURN() << DEB_VAR1(mode);
}

void CtAcquisition::setAccTimeMode(AccTimeMode mode)
{
  DEB_MEMBER_FUNCT();

  m_inpars.accTimeMode = mode;

  DEB_RETURN() << DEB_VAR1(mode);
}

void CtAcquisition::setAcqNbFrames(int nframes)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(nframes);

  m_inpars.acqNbFrames= nframes;
}

void CtAcquisition::getAcqNbFrames(int& nframes) const 
{
  DEB_MEMBER_FUNCT();

  nframes= m_inpars.acqNbFrames;

  DEB_RETURN() << DEB_VAR1(nframes);
}

void CtAcquisition::setAcqExpoTime(double acq_time)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(acq_time);

  if (acq_time < 0) {
    DEB_ERROR() << "Invalid " << DEB_VAR1(acq_time);
    throw LIMA_CTL_EXC(InvalidValue, "Invalid exposure time");
  } else if (acq_time == 0) {
    DEB_TRACE() << "Setting GATE mode";
  } else if (acq_time < m_valid_ranges.min_exp_time) {
    DEB_WARNING() << "Exposure time too short, setting to " 
		  << DEB_VAR1(m_valid_ranges.min_exp_time);
    acq_time = m_valid_ranges.min_exp_time;
  } else if (acq_time > m_valid_ranges.max_exp_time) {
    DEB_ERROR() << "Specified " << DEB_VAR1(acq_time) << " too long: " 
		<< DEB_VAR1(m_valid_ranges.max_exp_time);
    throw LIMA_CTL_EXC(InvalidValue, "Exposure time too long");
  }

  m_inpars.acqExpoTime= acq_time;
}

void CtAcquisition::getAcqExpoTime(double& acq_time) const
{
  DEB_MEMBER_FUNCT();

  acq_time= m_inpars.acqExpoTime;
  
  DEB_RETURN() << DEB_VAR1(acq_time);
}

void CtAcquisition::getAccNbFrames(int& nframes) const
{
  DEB_MEMBER_FUNCT();

  _updateAccPars();
  nframes= m_acc_nframes;

  DEB_RETURN() << DEB_VAR1(nframes);
}

void CtAcquisition::getAccLiveTime(double &acc_live_time) const
{
  DEB_MEMBER_FUNCT();

  _updateAccPars();
  acc_live_time = m_acc_live_time;
  
  DEB_RETURN() << DEB_VAR1(acc_live_time);
}

void CtAcquisition::getAccDeadTime(double &acc_dead_time) const
{
  DEB_MEMBER_FUNCT();
  
  _updateAccPars();
  acc_dead_time = m_acc_dead_time;

  DEB_RETURN() << DEB_VAR1(acc_dead_time);
}
void CtAcquisition::setAccMaxExpoTime(double acc_time)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(acc_time);

  m_inpars.accMaxExpoTime= acc_time;
}

void CtAcquisition::getAccMaxExpoTime(double& acc_time) const
{
  DEB_MEMBER_FUNCT();

  acc_time= m_inpars.accMaxExpoTime;

  DEB_RETURN() << DEB_VAR1(acc_time);
}

void CtAcquisition::getAccExpoTime(double& acc_time) const
{
  DEB_MEMBER_FUNCT();

  _updateAccPars();
  acc_time= m_acc_exptime;

  DEB_RETURN() << DEB_VAR1(acc_time);
}

void CtAcquisition::_updateAccPars() const
{
  DEB_MEMBER_FUNCT();
  if(m_inpars.acqMode == Accumulation)
    {
      if(m_inpars.accTimeMode == Live)
	{
	  int acc_div = int(m_inpars.acqExpoTime / m_inpars.accMaxExpoTime);
	  double expTime = acc_div * m_inpars.accMaxExpoTime;
	  if(m_inpars.acqExpoTime - expTime > 1e-6)
	    {
	      m_acc_nframes= acc_div + 1;
	      m_acc_exptime= m_inpars.acqExpoTime / m_acc_nframes;
	    } 
	  else 
	    {
	      m_acc_nframes= acc_div;
	      m_acc_exptime= m_inpars.accMaxExpoTime;
	    }
	  m_acc_dead_time = (m_acc_nframes - 1) * m_valid_ranges.min_lat_time;
	}
      else			// Real
	{
	  m_acc_nframes = int(m_inpars.acqExpoTime / (m_inpars.accMaxExpoTime +
						      m_valid_ranges.min_lat_time)) + 1;
	  m_acc_dead_time = (m_acc_nframes - 1) * m_valid_ranges.min_lat_time;
	  m_acc_exptime = (m_inpars.acqExpoTime - m_acc_dead_time) / m_acc_nframes;
	  
	}
      m_acc_live_time = m_acc_nframes * m_acc_exptime;
    }
  else				// set to -1 all accumulation params
    {
      m_acc_nframes = -1;
      m_acc_exptime = -1.;
      m_acc_live_time = -1.;
      m_acc_dead_time = -1.;
    }
  DEB_TRACE() << DEB_VAR2(m_acc_nframes,m_acc_exptime);
}

void CtAcquisition::setConcatNbFrames(int nframes)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(nframes);

  m_inpars.concatNbFrames= nframes;
}

void CtAcquisition::getConcatNbFrames(int& nframes) const
{
  DEB_MEMBER_FUNCT();

  nframes= m_inpars.concatNbFrames;
       
  DEB_RETURN() << DEB_VAR1(nframes);
}

void CtAcquisition::setLatencyTime(double lat_time) 
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(lat_time);

  if (lat_time < m_valid_ranges.min_lat_time)
    throw LIMA_CTL_EXC(InvalidValue, "Latency time too short");
  if (lat_time > m_valid_ranges.max_lat_time)
    throw LIMA_CTL_EXC(InvalidValue, "Latency time too long");
  m_inpars.latencyTime= lat_time;
}

void CtAcquisition::getLatencyTime(double& time) const
{
  DEB_MEMBER_FUNCT();

  time= m_inpars.latencyTime;

  DEB_RETURN() << DEB_VAR1(time);
}


// ------------------
// struct Parameters
// ------------------
CtAcquisition::Parameters::Parameters()
{
  DEB_CONSTRUCTOR();

  reset();
}

void CtAcquisition::Parameters::reset()
{
  DEB_MEMBER_FUNCT();

  acqMode= Single;
  accTimeMode = Live;
  acqNbFrames= 1;
  acqExpoTime= 1.;
  accMaxExpoTime= 1.;
  concatNbFrames= 0;
  latencyTime= 0.;
  triggerMode= IntTrig;

  DEB_TRACE() << DEB_VAR7(acqMode,acqNbFrames,acqExpoTime,
			  accMaxExpoTime,concatNbFrames,
			  latencyTime,triggerMode);
}

// ------------------
// struct ChangedPars
// ------------------
CtAcquisition::ChangedPars::ChangedPars()
{
  DEB_CONSTRUCTOR();

  set(0);
}

void CtAcquisition::ChangedPars::set(bool val)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(val);

  acqExpoTime= val;
  acqNbFrames= val;
  latencyTime= val;
  triggerMode= val;
  accMaxExpoTime= val;
  acqMode = val;
  DEB_TRACE() << DEB_VAR5(acqExpoTime,acqNbFrames,
			  latencyTime,triggerMode,
			  accMaxExpoTime);
}

void CtAcquisition::ChangedPars::check(Parameters p1, 
				       Parameters p2)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(p1,p2);

  acqExpoTime= (p1.acqExpoTime != p2.acqExpoTime);
  acqNbFrames= (p1.acqNbFrames != p2.acqNbFrames);
  latencyTime= (p1.latencyTime != p2.latencyTime);
  triggerMode= (p1.triggerMode != p2.triggerMode);
  accMaxExpoTime= (p1.accMaxExpoTime != p2.accMaxExpoTime);
  acqMode = (p1.acqMode != p2.acqMode);

  DEB_TRACE() << DEB_VAR6(acqExpoTime,acqNbFrames,latencyTime,
			  triggerMode,accMaxExpoTime,acqMode);
}
