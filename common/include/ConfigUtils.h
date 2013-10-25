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
#ifndef CONFIGUTILS_H
#define CONFIGUTILS_H
#include "Debug.h"
#include "LimaCompatibility.h"

struct Data;

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
    enum Type {None,Int,Int64,Float,String,
	       Boolean,Group,Array,List,DataType};
    Setting(libconfig::Setting* setting = NULL) : m_setting(setting) {}
    Setting(const Setting &setting) : m_setting(setting.m_setting) {}

    // --- lookup
    bool get(const char* alias,bool& value) const;
    bool get(const char* alias,int& value) const;
    bool get(const char* alias,long& value) const;
    bool get(const char* alias,long long& value) const;
    bool get(const char* alias,double& value) const;
    bool get(const char* alias,const char*& value) const;
    bool get(const char* alias,std::string &value) const;
    bool get(const char* alias,Data&) const;

    // --- modifiers
    void set(const char* alias,bool value);
    void set(const char* alias,int value);
    void set(const char* alias,long value);
    void set(const char* alias,long long value);
    void set(const char* alias,double value);
    void set(const char* alias,const char* value);
    void set(const char* alias,const std::string &value);
    void set(const char* alias,const Data&);

    // --- child
    Setting addChild(const char* alias);
    bool getChild(const char* alias,Setting& child) const;

    // --- list
    Setting addList(const char* alias);
    bool getList(const char* alias,Setting& list) const;

    // --- array
    Setting addArray(const char* alias);
    bool getArray(const char* alias,Setting& array) const;

    // --- list & array mgt
    void append(bool value);
    void append(int value);
    void append(long value);
    void append(long long value);
    void append(double value);
    void append(const char* value);
    void append(const std::string &value);

    void get(int pos,bool& value) const;
    void get(int pos,int& value) const;
    void get(int pos,long& value) const;
    void get(int pos,long long& value) const;
    void get(int pos,double& value) const;
    void get(int pos,const char*& value) const;
    void get(int pos,std::string &value) const;

    void set(int pos,bool value);
    void set(int pos,int value);
    void set(int pos,long value);
    void set(int pos,long long value);
    void set(int pos,double value);
    void set(int pos,const char* value);
    void set(int pos,const std::string &value);

    // --- settings
    Type getType() const;
    const char* getName() const;
    bool isGroup() const;

    operator bool() const;
    operator int() const;
    operator long long() const;
    operator double() const;
    operator const char*() const;
    operator std::string() const;

    Setting& operator=(bool value);
    Setting& operator=(int value);
    Setting& operator=(long long value);
    Setting& operator=(double value);
    Setting& operator=(const char* value);
    Setting& operator=(const std::string& value);

    const libconfig::Setting* get_raw_setting() const {return m_setting;}

    class const_iterator;

    const_iterator begin() const;
    const_iterator end() const;
  private:
    template<class INPUT,class OUTPUT> void t_set(const Data&);
    template<class OUTPUT,class INPUT> void t_get(Data& data);

    libconfig::Setting* m_setting;
  };

  class LIMACORE_API Setting::const_iterator
    {
      friend bool operator!=(const Setting::const_iterator& a,
			     const Setting::const_iterator& b);
      friend class Setting;
    public:
      const_iterator();
      const_iterator(const const_iterator&);

      const_iterator& operator++();
      const Setting& operator*();
    private:
      explicit const_iterator(const Setting&,
			      int init_index);
      
      Setting		m_setting;
      int		m_index;
      int		m_nb_items;
    };

  inline bool operator!=(const Setting::const_iterator& a,
			 const Setting::const_iterator& b)
  {
    return a.m_index != b.m_index;
  }
}
#endif
#endif //WITH_CONFIG
