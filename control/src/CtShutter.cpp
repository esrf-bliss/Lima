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
#include "CtShutter.h"

using namespace lima;

#ifdef CHECK_AVAILABILITY
#undef CHECK_AVAILABILITY
#endif
#define CHECK_AVAILABILITY(func) \
  if(m_has_shutter)\
    func;	   \
  else\
    THROW_CTL_ERROR(Error) <<  "No shutter capability";

#ifdef WITH_CONFIG
// --- config management
class CtShutter::_ConfigHandler : public CtConfig::ModuleTypeCallback
{
public:
  _ConfigHandler(CtShutter& shutter) :
    CtConfig::ModuleTypeCallback("Shutter"),
    m_shutter(shutter) {}
  virtual void store(Setting& shutter_setting)
  {
    CtShutter::Parameters pars;
    m_shutter.getParameters(pars);
    
    shutter_setting.set("mode",convert_2_string(pars.mode));
    shutter_setting.set("close_time",pars.close_time);
    shutter_setting.set("open_time",pars.open_time);
  }
  virtual void restore(const Setting& shutter_setting)
  {
    CtShutter::Parameters pars;
    m_shutter.getParameters(pars);

    std::string strmode;
    if(shutter_setting.get("mode",strmode))
      convert_from_string(strmode,pars.mode);

    shutter_setting.get("close_time",pars.close_time);
    shutter_setting.get("open_time",pars.open_time);
  
    m_shutter.setParameters(pars);
  }
private:
  CtShutter& m_shutter;
};
#endif //WITH_CONFIG

CtShutter::CtShutter(HwInterface *hw)
{
  DEB_CONSTRUCTOR();

  m_has_shutter = hw->getHwCtrlObj(m_hw_shutter);

  DEB_TRACE() << DEB_VAR2(m_has_shutter,m_hw_shutter);
  reset();
}

CtShutter::~CtShutter()
{
  DEB_DESTRUCTOR();
}

bool CtShutter::hasCapability() const
{
  DEB_MEMBER_FUNCT();
  return m_has_shutter;
}

void CtShutter::getModeList(ShutterModeList& mode_list) const
{
  DEB_MEMBER_FUNCT();
  if(m_has_shutter)
    m_hw_shutter->getModeList(mode_list);
}
    
bool CtShutter::checkMode(ShutterMode shut_mode) const
{
  DEB_MEMBER_FUNCT();
  bool returnFlag = false;
  if(m_has_shutter)
    returnFlag = m_hw_shutter->checkMode(shut_mode);
  return returnFlag;
}

void CtShutter::setMode(ShutterMode  shut_mode)
{
  DEB_MEMBER_FUNCT();
  CHECK_AVAILABILITY(m_pars.mode = shut_mode);
}

void CtShutter::getMode(ShutterMode& shut_mode) const
{
  DEB_MEMBER_FUNCT();
  CHECK_AVAILABILITY(shut_mode = m_pars.mode);
}

void CtShutter::setState(bool  shut_open)
{
  DEB_MEMBER_FUNCT();
  CHECK_AVAILABILITY(m_hw_shutter->setState(shut_open));
}

void CtShutter::getState(bool& shut_open) const
{
  DEB_MEMBER_FUNCT();
  CHECK_AVAILABILITY(m_hw_shutter->getState(shut_open));
}

void CtShutter::setOpenTime (double  shut_open_time)
{
  DEB_MEMBER_FUNCT();
  CHECK_AVAILABILITY(m_pars.open_time = shut_open_time);
 }

void CtShutter::getOpenTime (double& shut_open_time) const
{
  DEB_MEMBER_FUNCT();
  CHECK_AVAILABILITY(shut_open_time = m_pars.open_time);
}

void CtShutter::setCloseTime(double  shut_close_time)
{
  DEB_MEMBER_FUNCT();
  CHECK_AVAILABILITY(m_pars.close_time = shut_close_time);
}

void CtShutter::getCloseTime(double& shut_close_time) const
{
  DEB_MEMBER_FUNCT();
  CHECK_AVAILABILITY(shut_close_time = m_pars.close_time);
}
/** @brief get hw pars and sync with cache
 */
void CtShutter::reset()
{
  DEB_MEMBER_FUNCT();
  if(m_has_shutter)
    {
      m_hw_shutter->getMode(m_hw_pars.mode);
      m_hw_shutter->getCloseTime(m_hw_pars.close_time);
      m_hw_shutter->getOpenTime(m_hw_pars.open_time);

      m_pars = m_hw_pars;

      DEB_TRACE() << DEB_VAR1(m_pars);
    }
}
void CtShutter::getParameters(CtShutter::Parameters& pars) const
{
  DEB_MEMBER_FUNCT();
  CHECK_AVAILABILITY(pars = m_pars);
}

void CtShutter::setParameters(const CtShutter::Parameters& pars)
{
  DEB_MEMBER_FUNCT();
  CHECK_AVAILABILITY(m_pars = pars);
}
/** @brief apply cache parameters to hardware
 */
void CtShutter::apply()
{
  DEB_MEMBER_FUNCT();
  if(m_has_shutter)
    {
      try
	{
	  DEB_TRACE() << DEB_VAR2(m_pars,m_hw_pars);
	  if(m_pars.mode != m_hw_pars.mode)
	    m_hw_shutter->setMode(m_pars.mode);
	  if(m_pars.close_time != m_hw_pars.close_time)
	    m_hw_shutter->setCloseTime(m_pars.close_time);
	  if(m_pars.open_time != m_hw_pars.open_time)
	    m_hw_shutter->setOpenTime(m_pars.open_time);

	  m_hw_pars = m_pars;
	}
      catch(Exception &exp)			// rollback
	{
	  m_pars = m_hw_pars;
	  throw exp;
	}
    }
}

#ifdef WITH_CONFIG
CtConfig::ModuleTypeCallback* CtShutter::_getConfigHandler()
{
  if(m_has_shutter)
    return new _ConfigHandler(*this);
  else
    return NULL;
}
#endif //WITH_CONFIG