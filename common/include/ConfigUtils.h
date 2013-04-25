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
#ifndef CONFIGUTILS_H
#define CONFIGUTILS_H
#include "Debug.h"
#include "LimaCompatibility.h"

namespace libconfig
{
  class Setting;
}
namespace lima
{
  class LIMACORE_API Setting
  {
    DEB_CLASS_NAMESPC(DebModCommon,"Setting","Common");
  public:
    Setting(libconfig::Setting* setting = NULL) : m_setting(setting) {}
    // --- lookup
    bool get(const char* alias,bool& value) const;
    bool get(const char* alias,int& value) const;
    bool get(const char* alias,long& value) const;
    bool get(const char* alias,long long& value) const;
    bool get(const char* alias,double& value) const;
    bool get(const char* alias,const char*& value) const;
    bool get(const char* alias,std::string &value) const;

    // --- modifiers
    void set(const char* alias,bool value);
    void set(const char* alias,int value);
    void set(const char* alias,long value);
    void set(const char* alias,long long value);
    void set(const char* alias,double value);
    void set(const char* alias,const char* value);
    void set(const char* alias,const std::string &value);
    // --- child
    Setting addChild(const char* alias);
    bool getChild(const char* alias,Setting& child) const;

    const libconfig::Setting* get_raw_setting() const {return m_setting;}
  private:
    libconfig::Setting* m_setting;
  };
}
#endif
