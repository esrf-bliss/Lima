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
#ifdef WIN32
#pragma warning( disable : 4290 )
#endif

#include <libconfig.h++>
#include <unistd.h>

#include "CtAccumulation.h"
#include "CtAcquisition.h"
#include "CtConfig.h"
#include "CtImage.h"
#include "CtSaving.h"
#include "CtShutter.h"
#include "CtVideo.h"

#include "HwConfigCtrlObj.h"
#include "HwInterface.h"

#include "ConfigUtils.h"

using namespace lima;

const char* CtConfig::All = "All";

//Static function
static void _remove_if_exists(libconfig::Setting &setting,
			      const std::string& alias)
{
  if(setting.exists(alias))
    setting.remove(alias);
}

class _HwWrapperCallback : public CtConfig::ModuleTypeCallback
{
public:
  _HwWrapperCallback(HwConfigCtrlObj* hwconfig) :
    CtConfig::ModuleTypeCallback("Camera"),
    m_hwconfig(hwconfig) {}
    virtual void store(Setting& setting)
  {
    m_hwconfig->store(setting);
  }
  virtual void restore(const Setting& setting)
  {
    m_hwconfig->restore(setting);
  }
private:
  HwConfigCtrlObj* m_hwconfig;
};

CtConfig::CtConfig(CtControl &control) :
  m_ctrl(control),
  m_config(new libconfig::Config())
{
  HwInterface *hw = control.hwInterface();
  HwConfigCtrlObj *hwconfig;
  if(hw->getHwCtrlObj(hwconfig))
    this->registerModule(new _HwWrapperCallback(hwconfig));
}

CtConfig::~CtConfig()
{
  try
    {
      m_config->writeFile(m_file_name.c_str());
    }
  catch(...)
    {
    }
  delete m_config;

  for(ModuleMap::iterator i = m_module_type.begin();
      i != m_module_type.end();++i)
    i->second->unref();
}
/** @brief set the full path for the filename used for saving configuration
 */
void CtConfig::setFilename(const std::string &full_path)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(full_path);

  m_file_name = full_path;
}

void CtConfig::getFilename(std::string &full_path) const
{
  full_path = m_file_name;
}

void CtConfig::store(const std::string& alias,
		     ModuleType module_to_save)
{
  std::list<ModuleType> modules_to_save;
  modules_to_save.push_back(module_to_save);

  store(alias,modules_to_save);
}

void CtConfig::store(const std::string& alias,
		     const std::list<ModuleType>& modules_to_save)
{
  DEB_MEMBER_FUNCT();

  libconfig::Setting& root = m_config->getRoot();
  _remove_if_exists(root,alias);
  try
    {
      libconfig::Setting& alias_setting =
	root.add(alias,libconfig::Setting::TypeGroup);
 
      bool anAllFlag = false;
      for(std::list<ModuleType>::const_iterator i = modules_to_save.begin();
	  i != modules_to_save.end() && !anAllFlag;++i)
	anAllFlag = *i == All;

      if(anAllFlag)
	{
	  for(ModuleMap::iterator module = m_module_type.begin();
	      module != m_module_type.end();++module)
	    {
	      libconfig::Setting &setting = alias_setting.add(module->first,
							      libconfig::Setting::TypeGroup);
	      lima::Setting tmpSetting(&setting);
	      module->second->store(tmpSetting);
	    }
	}
      else
	{
	  for(std::list<ModuleType>::const_iterator i = modules_to_save.begin();
	      i != modules_to_save.end();++i)
	    {
	      ModuleMap::iterator module = m_module_type.find(*i);
	      if(module != m_module_type.end())
		{
		  libconfig::Setting &setting = alias_setting.add(module->first,
								  libconfig::Setting::TypeGroup);
		  lima::Setting tmpSetting(&setting);
		  ModuleTypeCallback* cbk = module->second;
		  cbk->store(tmpSetting);
		}
	      else
		{
		  THROW_CTL_ERROR(InvalidValue) << "Module type: " 
						<< *i << " Doesn't exist";
		}
	    }
	}
    }
  catch(libconfig::SettingNameException& exp)
    {
      THROW_COM_ERROR(Error) << exp.what();
    }
  catch(libconfig::SettingTypeException& exp)
    {
      THROW_COM_ERROR(Error) << exp.what();
    }
}

void CtConfig::update(const std::string& alias,
			ModuleType module_to_save)
{
  std::list<ModuleType> modules_to_save;
  modules_to_save.push_back(module_to_save);
  update(alias,modules_to_save);
}

void CtConfig::update(const std::string& alias,
		      const std::list<ModuleType>& modules_to_save)

{
  DEB_MEMBER_FUNCT();
  
  libconfig::Setting& root = m_config->getRoot();
  bool anAllFlag = false;
  for(std::list<ModuleType>::const_iterator i = modules_to_save.begin();
      i != modules_to_save.end() && !anAllFlag;++i)
    anAllFlag = *i == All;

  if(!anAllFlag && root.exists(alias))
    {
      libconfig::Setting& alias_setting = root[alias];

      for(std::list<ModuleType>::const_iterator i = modules_to_save.begin();
	  i != modules_to_save.end();++i)
	{
	  ModuleMap::iterator module = m_module_type.find(*i);
	  if(module != m_module_type.end())
	    {
	      _remove_if_exists(alias_setting,*i);
	      try
		{
		  libconfig::Setting &setting = alias_setting.add(module->first,
								  libconfig::Setting::TypeGroup);
		  lima::Setting tmpSetting(&setting);
		  module->second->store(tmpSetting);
		}
	      catch(libconfig::SettingNameException& exp)
		{
		  THROW_COM_ERROR(Error) << exp.what();
		}
	      catch(libconfig::SettingTypeException& exp)
		{
		  THROW_COM_ERROR(Error) << exp.what();
		}
	    }
	  else
	    {
	      THROW_CTL_ERROR(InvalidValue) << "Module type: " 
					    << *i << " Doesn't exist";
	    }
	}
    }
  else
    store(alias,modules_to_save);
}

  void CtConfig::getAlias(std::list<std::string>& aliases) const
  {
    libconfig::Setting& root = m_config->getRoot();
    int nbAlias = root.getLength();
    for(int i = 0;i < nbAlias;++i)
      {
	const libconfig::Setting &a = root[i];
	if(a.isGroup() && a.getName())
	  aliases.push_back(a.getName());
      }
  }

  void CtConfig::getAvailableModule(std::list<ModuleType>& module) const
  {
    for(ModuleMap::const_iterator i = m_module_type.begin();
	i != m_module_type.end();++i)
      module.push_back(i->first.c_str());
  }

  void CtConfig::apply(const std::string& alias)
  {
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(alias);

    libconfig::Setting& root = m_config->getRoot();
    try
      {  
	libconfig::Setting& alias_setting = root[alias];
	for(ModuleMap::iterator i = m_module_type.begin();
	    i != m_module_type.end();++i)
	  {
	    if(alias_setting.exists(i->first))
	      {
		libconfig::Setting &setting = alias_setting[i->first];
		if(setting.isGroup())
		  {
		
		    lima::Setting tmpSetting(&setting);
		    i->second->restore(tmpSetting);
		  }
	      }
	  }
      }
    catch(libconfig::SettingTypeException &exp)
      {
	THROW_CTL_ERROR(Error) << exp.what();
      }
    catch(libconfig::SettingNotFoundException& exp)
      {
	THROW_CTL_ERROR(Error) << exp.what();
      }
  }

  void CtConfig::pop(const std::string& alias)
  {
    apply(alias);
    remove(alias);
  }

  void CtConfig::remove(const std::string& alias,ModuleType modules_to_remove)
  {
    libconfig::Setting& root = m_config->getRoot();
    if(modules_to_remove == All)
      _remove_if_exists(root,alias);
    else if(root.exists(alias))
      {
	libconfig::Setting& alias_setting = root[alias];
	_remove_if_exists(alias_setting,modules_to_remove);
      }
  }

  void CtConfig::remove(const std::string& alias,
			const std::list<ModuleType>& modules_to_remove)
  {
    libconfig::Setting& root = m_config->getRoot();
    if(!root.exists(alias)) return;

    libconfig::Setting& alias_setting = root[alias];
    for(std::list<ModuleType>::const_iterator i = modules_to_remove.begin();
	i != modules_to_remove.end();++i)
      {
	if(*i == All)
	  {
	    root.remove(alias);
	    break;
	  }
	else
	  _remove_if_exists(alias_setting,*i);
      }
  }

  void CtConfig::save()
  {
    DEB_MEMBER_FUNCT();

    try
      {
	DEB_TRACE() << DEB_VAR1(m_file_name);
	m_config->writeFile(m_file_name.c_str());
      }
    catch(libconfig::FileIOException &exp)
      {
	THROW_CTL_ERROR(Error) << exp.what();
      }
  }

  void CtConfig::load()
  {
    DEB_MEMBER_FUNCT();

    try
      {
	DEB_TRACE() << DEB_VAR1(m_file_name);
	m_config->readFile(m_file_name.c_str());
      }
    catch(libconfig::ParseException &exp)
      {
	THROW_CTL_ERROR(Error) << exp.getError() 
			       << " (" << exp.getFile()
			       << ":" << exp.getLine() << ")";
      }
    catch(libconfig::FileIOException &exp)
      {
	THROW_CTL_ERROR(Error) << exp.what();
      }
  }

  void CtConfig::registerModule(ModuleTypeCallback *modulePt)
  {
    DEB_MEMBER_FUNCT();
  
    if(!modulePt) return;
    DEB_PARAM() << DEB_VAR2(modulePt->m_module_type,modulePt);

    modulePt->ref();
    std::pair<ModuleMap::iterator,bool> result = 
      m_module_type.insert(ModuleMap::value_type(modulePt->m_module_type.c_str(),
						 modulePt));
    //if the module already exist, replace
    if(!result.second)
      {
	DEB_WARNING() << "Already exist:" << DEB_VAR2(modulePt->m_module_type,
						      modulePt);
	result.first->second->unref();
	result.first->second = modulePt;
      }
  }

  void CtConfig::unregisterModule(const std::string& module_type)
  {
    ModuleMap::iterator i = m_module_type.find(module_type.c_str());
    if(i != m_module_type.end())
      m_module_type.erase(i);
  }

  // --- ModuleTypeCallback
  CtConfig::ModuleTypeCallback::ModuleTypeCallback(CtConfig::ModuleType aType) :
    m_module_type(aType),
    m_ref_count(1)
      {
      }

    CtConfig::ModuleTypeCallback::~ModuleTypeCallback()
      {
      }

    void CtConfig::ModuleTypeCallback::ref()
    {
      ++m_ref_count;
    }

    void CtConfig::ModuleTypeCallback::unref()
    {
      if(!--m_ref_count)
	delete this;

    }
