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
#include "Data.h"

#define CHECK_NULL()				\
  DEB_MEMBER_FUNCT();				\
  if(!m_setting) THROW_COM_ERROR(Error) << "Setting is Null";

bool Setting::get(const char* alias,bool& value) const
{
  CHECK_NULL();
  return m_setting->lookupValue(alias,value);
}

bool Setting::get(const char* alias,int& value) const
{
  CHECK_NULL();
  return m_setting->lookupValue(alias,value);
}

bool Setting::get(const char* alias,long& value) const
{
  CHECK_NULL();

  long long tmpValue;
  bool rFlag = m_setting->lookupValue(alias,tmpValue);
  if(rFlag) value = long(tmpValue);
  return rFlag;
}

bool Setting::get(const char* alias,long long& value) const
{
  CHECK_NULL();

  return m_setting->lookupValue(alias,value);
}

bool Setting::get(const char* alias,double& value) const
{
  CHECK_NULL();

  return m_setting->lookupValue(alias,value);
}

bool Setting::get(const char* alias,const char*& value) const
{
  CHECK_NULL();

  return m_setting->lookupValue(alias,value);
}

bool Setting::get(const char* alias,std::string& value) const
{
  CHECK_NULL();

  return m_setting->lookupValue(alias,value);
}

bool Setting::get(const char* alias,Data& data) const
{
  CHECK_NULL();
  Setting data_setting;
  if(getChild(alias,data_setting))
    {
      data_setting.get("frameNumber",data.frameNumber);
      data_setting.get("timestamp",data.timestamp);
      int type;
      bool has_type = data_setting.get("type",type);

      Setting dimensions_array;
      bool has_dimensions = data_setting.getArray("dimensions",dimensions_array);

      Setting header_group;
      if(data_setting.getChild("header",header_group))
	{
	  for(Setting::const_iterator i = header_group.begin();
	      i != header_group.end();++i)
	    {
	      const Setting& setting = *i;
	      const char* name = setting.getName();
	      const char* value = setting;
	      data.header.insert(name,value);
	    }
	}
      Setting buffer_setting;
      bool has_buffer = data_setting.getArray("buffer",buffer_setting);
      if(has_type && has_dimensions && has_buffer)
	{
	  data.type = (Data::TYPE)type;
	  for(Setting::const_iterator i = dimensions_array.begin();
	      i != dimensions_array.end();++i)
	    data.dimensions.push_back(*i);

	  int buffer_size = data.size();
	  Buffer* new_buffer = new Buffer(buffer_size);
	  data.setBuffer(new_buffer);
	  new_buffer->unref();

	  switch(data.type)
	    {
	    case Data::UINT8:
	      buffer_setting.t_get<unsigned char,int>(data);
	      break;
	    case Data::INT8:
	      buffer_setting.t_get<char,int>(data);
	      break;
	    case Data::UINT16:
	      buffer_setting.t_get<unsigned short,int>(data);
	      break;
	    case Data::INT16:
	      buffer_setting.t_get<short,int>(data);
	      break;
	    case Data::INT32:
	      buffer_setting.t_get<int,int>(data);
	      break;
	    case Data::UINT32:
	      buffer_setting.t_get<unsigned int,long long>(data);
	      break;
	    case Data::INT64:
	      buffer_setting.t_get<long long,long long>(data);
	      break;
	    case Data::FLOAT:
	      buffer_setting.t_get<float,double>(data);
	      break;
	    case Data::DOUBLE:
	      buffer_setting.t_get<double,double>(data);
	      break;
	    default: 
	      THROW_COM_ERROR(Error) << "Type not handle";
	    }
	  return true;
	}
      
    }
  return false;
}
// --- modifiers
#define SET_VALUE(Type)				\
  CHECK_NULL();					\
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

void Setting::set(const char* alias,
		  const Data& data)
{
  CHECK_NULL();

  Setting data_setting = addChild(alias);
  data_setting.set("frameNumber",data.frameNumber);
  data_setting.set("timestamp",data.timestamp);
  data_setting.set("type",data.type);

  Setting dimensions = data_setting.addArray("dimensions");
  for(std::vector<int>::const_iterator i = data.dimensions.begin();
      i != data.dimensions.end();++i)
    dimensions.append(*i);
  
  Setting header_setting = data_setting.addChild("header");
  const Data::HeaderContainer::Header &header = data.header.header();
  for(Data::HeaderContainer::Header::const_iterator i = header.begin();
      i != header.end();++i)
    header_setting.set(i->first.c_str(),i->second);
  
  Setting buffer_setting = data_setting.addArray("buffer");
  switch(data.type)
    {
    case Data::UINT8:
      buffer_setting.t_set<unsigned char,int>(data);
      break;
    case Data::INT8:
      buffer_setting.t_set<char,int>(data);
      break;
    case Data::UINT16:
      buffer_setting.t_set<unsigned short,int>(data);
      break;
    case Data::INT16:
      buffer_setting.t_set<short,int>(data);
      break;
    case Data::INT32:
      buffer_setting.t_set<int,int>(data);
      break;
    case Data::UINT32:
      buffer_setting.t_set<unsigned int,long long>(data);
      break;
    case Data::INT64:
      buffer_setting.t_set<long long,long long>(data);
      break;
    case Data::FLOAT:
      buffer_setting.t_set<float,double>(data);
      break;
    case Data::DOUBLE:
      buffer_setting.t_set<double,double>(data);
      break;
    default: 
      THROW_COM_ERROR(Error) << "Type not handle";
    }
}
// --- child management
#define ADD_NESTED_TYPE(type)						\
  CHECK_NULL();							\
  try									\
    {									\
      libconfig::Setting &alias_setting =				\
	m_setting->add(alias,type);					\
      return Setting(&alias_setting);					\
    }									\
  catch(libconfig::SettingNameException& exp)				\
    {									\
      THROW_COM_ERROR(Error) << exp.what();				\
    }									\
  catch(libconfig::SettingTypeException& exp)				\
    {									\
      THROW_COM_ERROR(Error) << exp.what();				\
    }

#define GET_NESTED_TYPE(_test_)			\
  CHECK_NULL();					\
  bool returnFlag = m_setting->exists(alias);	\
  if(returnFlag)				\
    {									\
      libconfig::Setting &alias_setting = m_setting->operator[](alias);	\
      returnFlag = _test_;						\
      if(returnFlag)							\
	child.m_setting = &alias_setting;				\
    }									\
  return returnFlag;

Setting Setting::addChild(const char *alias)
{
  ADD_NESTED_TYPE(libconfig::Setting::TypeGroup);
}

bool Setting::getChild(const char* alias,Setting& child) const
{
  GET_NESTED_TYPE(alias_setting.isGroup());
}

// --- list management
Setting Setting::addList(const char* alias)
{
  ADD_NESTED_TYPE(libconfig::Setting::TypeList);
}

bool Setting::getList(const char* alias,Setting& child) const
{
  GET_NESTED_TYPE(alias_setting.isList());
}

// --- array management
Setting Setting::addArray(const char* alias)
{
  ADD_NESTED_TYPE(libconfig::Setting::TypeArray);
}

bool Setting::getArray(const char* alias,Setting& child) const
{
  GET_NESTED_TYPE(alias_setting.isArray());
}

// --- list & array mgt
#define APPEND(Type)				\
  CHECK_NULL();					\
  try						\
    {								\
      libconfig::Setting& nSetting = m_setting->add(Type);	\
      nSetting = value;						\
    }								\
  catch(libconfig::SettingTypeException& exp)			\
    {								\
      THROW_COM_ERROR(Error) << exp.what();			\
    }								\

void Setting::append(bool value)
{
  APPEND(libconfig::Setting::TypeBoolean);
}

void Setting::append(int value)
{
  APPEND(libconfig::Setting::TypeInt);
}

void Setting::append(long value)
{
  APPEND(libconfig::Setting::TypeInt64);
}

void Setting::append(long long value)
{
  APPEND(libconfig::Setting::TypeInt64);
}

void Setting::append(double value)
{
  APPEND(libconfig::Setting::TypeFloat);
}

void Setting::append(const char* value)
{
  APPEND(libconfig::Setting::TypeString);
}

void Setting::append(const std::string &value)
{
  APPEND(libconfig::Setting::TypeString);
}

#define SET_VALUE_AT_POS()			\
  CHECK_NULL();						\
  try							\
    {									\
      libconfig::Setting &value_setting = m_setting->operator[](pos);	\
      value_setting = value;						\
    }									\
  catch(libconfig::SettingTypeException& exp)				\
    {									\
      THROW_COM_ERROR(Error) << exp.what();				\
    }									\
  catch(libconfig::SettingNotFoundException& exp)			\
    {									\
      THROW_COM_ERROR(Error) << exp.what();				\
    }

void Setting::set(int pos,bool value)
{
  SET_VALUE_AT_POS();
}

void Setting::set(int pos,int value)
{
  SET_VALUE_AT_POS();
}
void Setting::set(int pos,long value)
{
  SET_VALUE_AT_POS();
}
void Setting::set(int pos,long long value)
{
  SET_VALUE_AT_POS();
}
void Setting::set(int pos,double value)
{
  SET_VALUE_AT_POS();
}
void Setting::set(int pos,const char* value)
{
  SET_VALUE_AT_POS();
}
void Setting::set(int pos,const std::string &value)
{
  SET_VALUE_AT_POS();
}

template <class INPUT,class OUTPUT>
void Setting::t_set(const Data& data)
{
  OUTPUT saved_value;
  const INPUT* src = (const INPUT*)data.data();
  long count = data.size() / data.depth();
  for(int i = 0;i < count;++i,++src)
    {
      saved_value = OUTPUT(*src);
      append(saved_value);
    }
}

template<class OUTPUT,class INPUT>
void Setting::t_get(Data& data)
{
  INPUT data_value;
  OUTPUT* dst = (OUTPUT*)data.data();
  long count = data.size() / data.depth();
  for(Setting::const_iterator i = this->begin();
      i != this->end() && count;++i,--count,++dst)
    {
      data_value = *i;
      *dst = OUTPUT(data_value);
    }
}
// --- settings

Setting::Type Setting::getType() const
{
  CHECK_NULL();

  libconfig::Setting::Type aType = m_setting->getType();
  Setting::Type aReturnType;
  switch(aType)
    {
    case libconfig::Setting::TypeInt:		aReturnType = Int;	break;
    case libconfig::Setting::TypeInt64:		aReturnType = Int64;	break;
    case libconfig::Setting::TypeFloat:		aReturnType = Float;	break;
    case libconfig::Setting::TypeString:	aReturnType = String;	break;
    case libconfig::Setting::TypeBoolean:	aReturnType = Boolean;	break;
    case libconfig::Setting::TypeGroup:
      {
	// Test if it's a data type
	int type;
	Setting dimensions_array;
	Setting buffer_setting;

	if(get("type",type) &&
	   getArray("dimensions",dimensions_array) &&
	   getArray("buffer",buffer_setting))
	  aReturnType = DataType;
	else
	  aReturnType = Group;
      }
      break;
    case libconfig::Setting::TypeArray:		aReturnType = Array;	break;
    case libconfig::Setting::TypeList:		aReturnType = List;	break;
    default: 
      aReturnType = None;break;
    }
  return aReturnType;
}

const char* Setting::getName() const
{
  CHECK_NULL();

  return m_setting->getName();
}

bool Setting::isGroup() const
{
  CHECK_NULL();
  
  return m_setting->isGroup();
}

#define CAST_TYPE(Type)				\
  CHECK_NULL();					\
  try						\
    {						\
      return static_cast<Type>(*m_setting);	\
    }						\
  catch(libconfig::SettingTypeException& exp)	\
    {						\
      THROW_COM_ERROR(Error) << exp.what();	\
    }

Setting::operator bool() const
{
  CAST_TYPE(bool);
}

Setting::operator int() const
{
  CAST_TYPE(int);
}

Setting::operator long long() const
{
  CAST_TYPE(long long);
}

Setting::operator double() const
{
  CAST_TYPE(double);
}

Setting::operator const char*() const
{
  CAST_TYPE(const char*);
}

Setting::operator std::string() const
{
  CAST_TYPE(const char*);
}

#define ASSIGN()				\
  CHECK_NULL();					\
  try						\
    {						\
      m_setting->operator=(value);		\
      return *this;				\
    }						\
  catch(libconfig::SettingTypeException& exp)	\
    {						\
      THROW_COM_ERROR(Error) << exp.what();	\
    }


Setting& Setting::operator=(bool value)
{
  ASSIGN();
}

Setting& Setting::operator=(int value)
{
  ASSIGN();
}
 
Setting& Setting::operator=(long long value)
{
  ASSIGN();
}
 
Setting& Setting::operator=(double value)
{
  ASSIGN();
}

Setting& Setting::operator=(const char* value)
{
  ASSIGN();
}

Setting& Setting::operator=(const std::string& value)
{
  ASSIGN();
}

// --- iterator

Setting::const_iterator::const_iterator() :
  m_index(-1),
  m_nb_items(-1)
 {
 }

Setting::const_iterator::const_iterator(const Setting& setting,
					int init_index) :
  m_index(init_index)
{
  libconfig::Setting* parent = setting.m_setting;
  m_nb_items = parent->getLength();
  if(init_index >= 0 && init_index < m_nb_items)
    {
      libconfig::Setting& child = parent->operator[](init_index);
      m_setting = Setting(&child);
    }
}

Setting::const_iterator::const_iterator(const Setting::const_iterator& b) :
  m_setting(b.m_setting),
  m_index(b.m_index),
  m_nb_items(b.m_nb_items)
{
}

Setting::const_iterator& Setting::const_iterator::operator++()
{
  ++m_index;
  if(m_index < m_nb_items)
    {
      libconfig::Setting& parent = m_setting.m_setting->getParent();
      libconfig::Setting& child = parent[m_index];
      m_setting = Setting(&child);
    }
  else
    m_setting = Setting();
  return *this;
}

const Setting& Setting::const_iterator::operator*()
{
  return m_setting;
}

Setting::const_iterator Setting::begin() const
{
  CHECK_NULL();
  switch(getType())
    {
    case Group:
    case Array:
    case List:
      break;
    default:
      THROW_COM_ERROR(Error) << "Can't have iterator for this Setting type";
    }
  return Setting::const_iterator(*this,0);
}

Setting::const_iterator Setting::end() const
{
  CHECK_NULL();
  int nb_items = m_setting->getLength();
  return Setting::const_iterator(*this,nb_items);
}
