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

#include "lima/CtAcquisition.h"
#include "lima/CtSaving.h"

#include "math.h"
#include <algorithm>
using std::max;

#define CHECK_EXPOTIME(val)						\
  if (val < m_valid_ranges.min_exp_time)				\
  {									\
    THROW_CTL_ERROR(InvalidValue)					\
      << "Specified exposure time " << DEB_VAR1(val) << " too short: "	\
      << DEB_VAR1(m_valid_ranges.min_exp_time);				\
  }									\
  else if (val > m_valid_ranges.max_exp_time)				\
  {									\
    THROW_CTL_ERROR(InvalidValue)					\
      << "Specified exposure time " << DEB_VAR1(val) << " too long: "	\
      << DEB_VAR1(m_valid_ranges.max_exp_time);				\
  }

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
    m_acq._check_timing_ranges();
  }
private:
  CtAcquisition& m_acq;
};
/*----------------------------------------------------------------------
			  ModuleTypeCallback
----------------------------------------------------------------------*/
#ifdef WITH_CONFIG
class CtAcquisition::_ConfigHandler : public CtConfig::ModuleTypeCallback
{
public:
  _ConfigHandler(CtAcquisition& acq) :
    CtConfig::ModuleTypeCallback("Acquisition"),
    m_acq(acq) {}
  virtual void store(Setting& acq_setting)
  {
    CtAcquisition::Parameters pars;
    m_acq.getPars(pars);

    acq_setting.set("acqMode",convert_2_string(pars.acqMode));
    acq_setting.set("accTimeMode",convert_2_string(pars.accTimeMode));
    acq_setting.set("acqNbFrames",pars.acqNbFrames);
    acq_setting.set("acqExpoTime",pars.acqExpoTime);
    acq_setting.set("accMaxExpoTime",pars.accMaxExpoTime);
    acq_setting.set("concatNbFrames",pars.concatNbFrames);
    acq_setting.set("latencyTime",pars.latencyTime);
    acq_setting.set("triggerMode",convert_2_string(pars.triggerMode));
    acq_setting.set("autoExpoMode",convert_2_string(pars.autoExpoMode));
  }
  virtual void restore(const Setting& acq_setting)
  {
    CtAcquisition::Parameters pars;
    m_acq.getPars(pars);

    std::string strAcqMode;
    if(acq_setting.get("acqMode",strAcqMode))
      convert_from_string(strAcqMode,pars.acqMode);

    std::string straccTimeMode;
    if(acq_setting.get("accTimeMode",straccTimeMode))
      convert_from_string(straccTimeMode,pars.accTimeMode);

    acq_setting.get("acqNbFrames",pars.acqNbFrames);
    acq_setting.get("acqExpoTime",pars.acqExpoTime);
    acq_setting.get("accMaxExpoTime",pars.accMaxExpoTime);
    acq_setting.get("concatNbFrames",pars.concatNbFrames);
    acq_setting.get("latencyTime",pars.latencyTime);

    std::string strtriggerMode;
    if(acq_setting.get("triggerMode",strtriggerMode))
      convert_from_string(strtriggerMode,pars.triggerMode);

    std::string strautoExpoMode;
    if(acq_setting.get("autoExpoMode",strautoExpoMode))
      convert_from_string(strautoExpoMode,pars.autoExpoMode);

    m_acq.setPars(pars);
  }
private:
  CtAcquisition& m_acq;
};
#endif //WITH_CONFIG

CtAcquisition::CtAcquisition(HwInterface *hw) :
  m_acc_nframes(-1),
  m_acc_exptime(-1.),
  m_acc_live_time(-1.),
  m_acc_dead_time(-1.),
  m_valid_ranges_cb(NULL)
{
  DEB_CONSTRUCTOR();

  if (!hw->getHwCtrlObj(m_hw_sync))
    THROW_CTL_ERROR(Error) <<  "Cannot get hardware sync object";

  m_valid_ranges_cb = new _ValidRangesCallback(*this);
  m_hw_sync->getValidRanges(m_valid_ranges);
  m_hw_sync->registerValidRangesCallback(m_valid_ranges_cb);
  DEB_TRACE() << DEB_VAR1(m_valid_ranges);

  //Access mode Master or Monitor
  HwSyncCtrlObj::AccessMode access_mode;
  m_hw_sync->getAccessMode(access_mode);
  m_monitor_mode = access_mode == HwSyncCtrlObj::Monitor;

  reset();
}

CtAcquisition::~CtAcquisition()
{
  DEB_DESTRUCTOR();
  delete m_valid_ranges_cb;
}

void CtAcquisition::setPars(const Parameters &pars)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(pars);

  if(m_monitor_mode) 
    THROW_CTL_ERROR(Error) << "In monitor mode, you can not change acquisition parameters";

  setAcqMode(pars.acqMode);
  setAcqNbFrames(pars.acqNbFrames);
  setAcqExpoTime(pars.acqExpoTime);
  setAccMaxExpoTime(pars.accMaxExpoTime);
  setConcatNbFrames(pars.concatNbFrames);
  setLatencyTime(pars.latencyTime);
  setTriggerMode(pars.triggerMode);
  setAutoExposureMode(pars.autoExpoMode);
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
  m_inpars.latencyTime = 0;
  m_applied_once= false;

  //Check auto exposure capability
  m_inpars.autoExpoMode = 
    m_hw_sync->checkAutoExposureMode(HwSyncCtrlObj::OFF) ? OFF : ON;

  _check_timing_ranges();
}

void CtAcquisition::_check_timing_ranges()
{
  //Set exposure time and accumulation time in valide ranges
  if(m_inpars.acqExpoTime < m_valid_ranges.min_exp_time)
    m_inpars.acqExpoTime = m_valid_ranges.min_exp_time;
  else if(m_inpars.acqExpoTime > m_valid_ranges.max_exp_time)
    m_inpars.acqExpoTime = m_valid_ranges.max_exp_time;

  if(m_inpars.accMaxExpoTime < m_valid_ranges.min_exp_time)
    m_inpars.accMaxExpoTime = m_valid_ranges.min_exp_time;
  else if(m_inpars.accMaxExpoTime > m_valid_ranges.max_exp_time)
    m_inpars.accMaxExpoTime = m_valid_ranges.max_exp_time;
}

void CtAcquisition::apply(CtControl::ApplyPolicy policy, CtControl *control)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(policy);

  // do not change acquisition parameters in monitor mode
  if(m_monitor_mode) return;

  CtControl::ApplyPolicy use_policy;

  use_policy= m_applied_once ? policy : CtControl::All;
	
  switch (use_policy) {
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
  m_applied_once = true;

  //Add acquisition parameters into internal header
  if (control) {
    CtSaving* saving = control->saving();
    
    Parameters pars; getPars(pars);
    saving->addToInternalCommonHeader("acq_mode", pars.acqMode);
    if (pars.acqMode == Accumulation) {
      saving->addToInternalCommonHeader("acq_acc_max_expotime",pars.accMaxExpoTime);
      saving->addToInternalCommonHeader("acq_acc_time_mode",pars.accTimeMode);
    } else if (pars.acqMode == Concatenation) {
      saving->addToInternalCommonHeader("acq_concat_nb_frames",pars.concatNbFrames);
    }
    
    saving->addToInternalCommonHeader("acq_nb_frames",pars.acqNbFrames);

    saving->addToInternalCommonHeader("acq_expo_time",pars.acqExpoTime);
    saving->addToInternalCommonHeader("acq_latency_time",pars.latencyTime);
    saving->addToInternalCommonHeader("acq_trigger_mode",pars.triggerMode);
    saving->addToInternalCommonHeader("acq_autoexpo_mode",pars.autoExpoMode);    
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
  if (m_hwpars.latencyTime <= m_valid_ranges.min_lat_time)
    m_hwpars.latencyTime = 0;
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
  double lat_time = max(m_inpars.latencyTime, m_valid_ranges.min_lat_time);
  if (m_changes.latencyTime) m_hw_sync->setLatTime(lat_time);
  
  if(m_changes.acqMode || m_changes.acqNbFrames)
    {
      if(m_inpars.acqMode == Accumulation)
	{
	  _updateAccPars();
	  if(m_inpars.triggerMode == IntTrigMult)
	    m_hw_sync->setNbFrames(m_acc_nframes);
	  else
	    m_hw_sync->setNbFrames(m_acc_nframes * m_inpars.acqNbFrames);
	}
      else
	m_hw_sync->setNbFrames(m_inpars.acqNbFrames);
    }

  bool autoExpoModeFlag = (m_changes.autoExpoMode || 
		       (m_inpars.acqNbFrames && m_inpars.autoExpoMode == ON_LIVE));

  if(m_inpars.autoExpoMode == OFF ||
     (m_inpars.autoExpoMode == ON_LIVE && m_inpars.acqNbFrames))
    {
      if(autoExpoModeFlag) m_hw_sync->setAutoExposureMode(HwSyncCtrlObj::OFF);

      bool acqExpoTimeFlag = m_changes.acqExpoTime || m_changes.autoExpoMode;
      switch (m_inpars.acqMode)
	{
	case Single:
	  if (acqExpoTimeFlag) m_hw_sync->setExpTime(m_inpars.acqExpoTime);
	  break;
	case Concatenation:
	  if (acqExpoTimeFlag) m_hw_sync->setExpTime(m_inpars.acqExpoTime);
	  break;
	case Accumulation:
	  if (acqExpoTimeFlag || m_changes.accMaxExpoTime) {
	    _updateAccPars();
	    m_hw_sync->setExpTime(m_acc_exptime);
	  }
	  break;
	}
    }
  else if(autoExpoModeFlag)
    m_hw_sync->setAutoExposureMode(HwSyncCtrlObj::ON);

  m_hwpars = m_inpars;

  DEB_TRACE() << DEB_VAR1(m_hwpars);
  
  m_changes.set(0);
}

void CtAcquisition::setTriggerMode(TrigMode mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(mode);
  if(m_monitor_mode) 
    THROW_CTL_ERROR(Error) << "In monitor mode, you can not change trigger mode";

  if(!m_hw_sync->checkTrigMode(mode))
    THROW_CTL_ERROR(Error) << "Trigger mode:" << DEB_VAR1(mode) << " not available";

  m_inpars.triggerMode= mode;
}

void CtAcquisition::getTriggerMode(TrigMode& mode) const
{
  DEB_MEMBER_FUNCT();

  mode = m_inpars.triggerMode;

  DEB_RETURN() << DEB_VAR1(mode);
}

void CtAcquisition::getTriggerModeList(TrigModeList& modes) const
{
  DEB_MEMBER_FUNCT();

  for(int i = 0;TrigMode(i) <= ExtTrigReadout;++i)
    {
      TrigMode tmpTrig = (TrigMode)i;
      if(m_hw_sync->checkTrigMode(tmpTrig))
	modes.push_back(tmpTrig);
    }
}

void CtAcquisition::setAcqMode(AcqMode mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(mode);

  if(m_monitor_mode) 
    THROW_CTL_ERROR(Error) << "In monitor mode, you can not change acquisition mode";

  m_inpars.acqMode= mode;
  switch (m_inpars.acqMode) {
  case Single:
    if (m_inpars.acqNbFrames<1) m_inpars.acqNbFrames= 1;
    m_acc_nframes = -1;
    m_acc_live_time = -1.;
    m_acc_dead_time = -1.;
    try{
      CHECK_EXPOTIME(m_inpars.acqExpoTime);
    }
    catch(...) {
      m_inpars.acqExpoTime = m_inpars.accMaxExpoTime;
    }
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

  if(m_monitor_mode) 
    THROW_CTL_ERROR(Error) << "In monitor mode, you can not change accumulation mode";

  m_inpars.accTimeMode = mode;

  DEB_RETURN() << DEB_VAR1(mode);
}

void CtAcquisition::setAcqNbFrames(int nframes)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(nframes);

  if(m_monitor_mode) 
    THROW_CTL_ERROR(Error) << "In monitor mode, you can not change nb frames";

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

  if(m_monitor_mode) 
    THROW_CTL_ERROR(Error) << "In monitor mode, you can not change acquisition exposition time";

  if(m_inpars.autoExpoMode == ON)
    THROW_CTL_ERROR(Error) << "Should disable auto exposure to set exposure time";

  if(m_inpars.acqMode != Accumulation)
  {
    CHECK_EXPOTIME(acq_time);
  }
  else if (acq_time < m_valid_ranges.min_exp_time)
  {
    THROW_CTL_ERROR(InvalidValue)
      << "Specified exposure time " << DEB_VAR1(acq_time) << " too short: "
      << DEB_VAR1(m_valid_ranges.min_exp_time);
  }
  m_inpars.acqExpoTime= acq_time;
}

void CtAcquisition::getAcqExpoTime(double& acq_time) const
{
  DEB_MEMBER_FUNCT();

  HwSyncCtrlObj::AutoExposureMode mode;
  m_hw_sync->getAutoExposureMode(mode);
  if(mode == HwSyncCtrlObj::ON || m_monitor_mode)
    m_hw_sync->getExpTime(acq_time);
  else
    acq_time= m_inpars.acqExpoTime;
  
  DEB_RETURN() << DEB_VAR1(acq_time);
}

bool CtAcquisition::checkAutoExposureMode(AutoExposureMode mode) const
{
  bool check_flag = false;
  switch(mode)
    {
    case ON:
      check_flag = m_hw_sync->checkAutoExposureMode(HwSyncCtrlObj::ON);
      break;
    case OFF:
      check_flag = m_hw_sync->checkAutoExposureMode(HwSyncCtrlObj::OFF);
      break;
    case ON_LIVE:
      check_flag = m_hw_sync->checkAutoExposureMode(HwSyncCtrlObj::ON) &&
	m_hw_sync->checkAutoExposureMode(HwSyncCtrlObj::OFF);
      break;
    default: 
      break;
    }
  return check_flag;
}

void CtAcquisition::getAutoExposureModeList(AutoExposureModeList& modes) const
{
  DEB_MEMBER_FUNCT();
  int nb_modes = 0;
  if(m_hw_sync->checkAutoExposureMode(HwSyncCtrlObj::OFF))
    modes.push_back(OFF),++nb_modes;
  if(m_hw_sync->checkAutoExposureMode(HwSyncCtrlObj::ON))
    modes.push_back(ON),++nb_modes;

  if(nb_modes == 2)		// All modes
    modes.push_back(ON_LIVE);
}

void CtAcquisition::setAutoExposureMode(AutoExposureMode mode)
{
  DEB_MEMBER_FUNCT();

  if(m_monitor_mode) 
    THROW_CTL_ERROR(Error) << "In monitor mode, you can not change auto exposure mode";

  if(!checkAutoExposureMode(mode))
    THROW_CTL_ERROR(NotSupported) << DEB_VAR1(mode);
  m_inpars.autoExpoMode = mode;
}

void CtAcquisition::getAutoExposureMode(AutoExposureMode& mode) const
{
  DEB_MEMBER_FUNCT();
  DEB_RETURN() << m_inpars.autoExpoMode;
  mode = m_inpars.autoExpoMode;
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
  CHECK_EXPOTIME(acc_time);

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

  if(m_monitor_mode) 
    THROW_CTL_ERROR(Error) << "In monitor mode, you can not change the nb of concatenation frames";

  if (nframes < 0)
    THROW_CTL_ERROR(InvalidValue) << "Invalid concat. " << DEB_VAR1(nframes);

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

  if(m_monitor_mode) 
    THROW_CTL_ERROR(Error) << "In monitor mode, you can not change the latency time";

  if (lat_time <= m_valid_ranges.min_lat_time)
    lat_time = 0;
  if (lat_time > m_valid_ranges.max_lat_time)
    THROW_CTL_ERROR(InvalidValue) 
      << "Specified latency time " << DEB_VAR1(lat_time) << " too long: "
      << DEB_VAR1(m_valid_ranges.max_lat_time);	
  m_inpars.latencyTime= lat_time;
}

void CtAcquisition::getLatencyTime(double& time) const
{
  DEB_MEMBER_FUNCT();

  time= max(m_inpars.latencyTime, m_valid_ranges.min_lat_time);

  DEB_RETURN() << DEB_VAR1(time);
}

#ifdef WITH_CONFIG
CtConfig::ModuleTypeCallback* CtAcquisition::_getConfigHandler()
{
  return new _ConfigHandler(*this);
}
#endif //WITH_CONFIG

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

  DEB_TRACE() << DEB_VAR7(acqMode,acqNbFrames,acqExpoTime, accMaxExpoTime,concatNbFrames, latencyTime,triggerMode);
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
  autoExpoMode = val;
  DEB_TRACE() << DEB_VAR6(acqExpoTime,acqNbFrames,
			  latencyTime,triggerMode,
			  accMaxExpoTime,autoExpoMode);
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
  autoExpoMode = (p1.autoExpoMode != p2.autoExpoMode);

  DEB_TRACE() << DEB_VAR6(acqExpoTime,acqNbFrames,latencyTime,
			  triggerMode,accMaxExpoTime,acqMode);
}
