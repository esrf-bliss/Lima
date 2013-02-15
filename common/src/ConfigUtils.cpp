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
#pragma warning(disable : 4290)
#endif
#include <libconfig.h++>

#include "Exceptions.h"
#include "ConfigUtils.h"
using namespace lima;

bool Setting::get(const char* alias,bool& value) const
{
  return m_setting->lookupValue(alias,value);
}

bool Setting::get(const char* alias,int& value) const
{
  return m_setting->lookupValue(alias,value);
}

bool Setting::get(const char* alias,long& value) const
{
  long long tmpValue;
  bool rFlag = m_setting->lookupValue(alias,tmpValue);
  if(rFlag) value = long(tmpValue);
  return rFlag;
}

bool Setting::get(const char* alias,long long& value) const
{
  return m_setting->lookupValue(alias,value);
}

bool Setting::get(const char* alias,double& value) const
{
  return m_setting->lookupValue(alias,value);
}

bool Setting::get(const char* alias,const char*& value) const
{
  return m_setting->lookupValue(alias,value);
}

bool Setting::get(const char* alias,std::string& value) const
{
  return m_setting->lookupValue(alias,value);
}

// --- modifiers
#define SET_VALUE(Type)				\
  if(m_setting->exists(alias))			\
    m_setting->operator[](alias) = value;	\
  else						\
    {									\
      libconfig::Setting& nSetting = m_setting->add(alias,Type);	\
      nSetting = value;							\
    }

void Setting::set(const char* alias,bool value)
{
  SET_VALUE(libconfig::Setting::TypeBoolean);
}

void Setting::set(const char* alias,int value)
{
  SET_VALUE(libconfig::Setting::TypeInt);
}

void Setting::set(const char* alias,long value)
{
  SET_VALUE(libconfig::Setting::TypeInt64);
}

void Setting::set(const char* alias,long long value)
{
  SET_VALUE(libconfig::Setting::TypeInt64);
}

void Setting::set(const char* alias,double value)
{
  SET_VALUE(libconfig::Setting::TypeFloat);
}

void Setting::set(const char* alias,const char* value)
{
  SET_VALUE(libconfig::Setting::TypeString);
}

void Setting::set(const char* alias,
		  const std::string &value)
{
  SET_VALUE(libconfig::Setting::TypeString);
}

// --- child management
Setting Setting::addChild(const char *alias)
{
  DEB_MEMBER_FUNCT();
  try
    {
      libconfig::Setting &alias_setting = m_setting->add(alias,libconfig::Setting::TypeGroup);
      return Setting(&alias_setting);
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

bool Setting::getChild(const char* alias,Setting& child) const
{
  bool returnFlag = m_setting->exists(alias);
  if(returnFlag)
    {
      libconfig::Setting &alias_setting = m_setting->operator[](alias);
      returnFlag = alias_setting.isGroup();
      if(returnFlag)
	child.m_setting = &alias_setting;
    }
  return returnFlag;
}
