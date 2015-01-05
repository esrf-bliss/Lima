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
    typedef const char* ModuleType;
    static ModuleType All;

    CtConfig(CtControl &);
    ~CtConfig();
    

    // --- set current config into a context alias
    void store(const std::string& alias,
	       ModuleType);
    void store(const std::string& alias,
	       const std::list<ModuleType>&);
    // --- add current config to a context alias
    void update(const std::string& alias,
		ModuleType);
    void update(const std::string& alias,
		const std::list<ModuleType>&);
    // --- get all context aliases
    void getAlias(std::list<std::string>&) const;
    // --- get all register module type 
    void getAvailableModule(std::list<ModuleType>&) const;
    // --- apply context to current parameters
    void apply(const std::string&);
    void pop(const std::string&);
    // --- remove part/all context
    void remove(const std::string&,ModuleType = All);
    void remove(const std::string&,
		const std::list<ModuleType>&);
    // --- file management
    void setFilename(const std::string&);
    void getFilename(std::string&) const;

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
      std::string	m_module_type;
      int		m_ref_count;
    };

    void registerModule(ModuleTypeCallback*);
    void unregisterModule(const std::string& module_type);

  private:
    typedef std::map<std::string,ModuleTypeCallback*> ModuleMap;
    CtConfig(const CtConfig &other): m_ctrl(other.m_ctrl) {}

    CtControl&		m_ctrl;
    libconfig::Config*	m_config;
    std::string		m_file_name;
    ModuleMap 		m_module_type;
  };
}
#endif
#endif //WITH_CONFIG

