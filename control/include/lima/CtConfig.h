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
#ifdef WITH_CONFIG
#ifndef CTCONFIGCONTEXT_H
#define CTCONFIGCONTEXT_H
#include <string>
#include <map>

#include "lima/CtControl.h"
#include "lima/ConfigUtils.h"

namespace libconfig
{
  class Config;
}

namespace lima
{
  class LIMACORE_API CtConfig
  {
    DEB_CLASS_NAMESPC(DebModControl,"Config","Control");
  public:
    typedef std::string ModuleType;
    typedef std::list<ModuleType> ModuleListType;
    typedef std::string AliasType;
    typedef std::list<AliasType> AliasListType;

    static const ModuleType All;

    CtConfig(CtControl &);
    ~CtConfig();
    

    // --- set current config into a context alias
    void store(const AliasType& alias,
	       const ModuleType& module_to_save);
    void store(const AliasType& alias,
	       const ModuleListType& modules_to_save);
    // --- add current config to a context alias
    void update(const AliasType& alias,
		const ModuleType& module_to_save);
    void update(const AliasType& alias,
		const ModuleListType& modules_to_save);
    // --- get all context aliases
    void getAlias(AliasListType& aliases) const;
    // --- get all register module type 
    void getAvailableModule(ModuleListType& modules) const;
    // --- apply context to current parameters
    void apply(const AliasType& alias);
    void pop(const AliasType& alias);
    // --- remove part/all context
    void remove(const AliasType& alias,
		const ModuleType& module_to_remove = All);
    void remove(const AliasType& alias,
		const ModuleListType& modules_to_remove);
    // --- file management
    void setFilename(const std::string& full_path);
    void getFilename(std::string& full_path) const;

    void save();
    void load();

    // --- callback to manage extra module type
    class LIMACORE_API ModuleTypeCallback
    {
      friend class CtConfig;
    public:
      explicit ModuleTypeCallback(ModuleType);

      virtual void store(Setting&) = 0;
      virtual void restore(const Setting &) = 0;

      void ref();
      void unref();
    protected:
      virtual ~ModuleTypeCallback();
    private:
      ModuleType	m_module_type;
      int		m_ref_count;
    };

    void registerModule(ModuleTypeCallback*);
    void unregisterModule(const ModuleType& module_type);

  private:
    typedef std::map<std::string,ModuleTypeCallback*> ModuleMap;
    CtConfig(const CtConfig &other): m_ctrl(other.m_ctrl), m_config(NULL) {}

    CtControl&		m_ctrl;
    libconfig::Config*	m_config;
    std::string		m_file_name;
    ModuleMap 		m_module_map;
  };
}
#endif
#endif //WITH_CONFIG

