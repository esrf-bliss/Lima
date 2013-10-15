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
class Setting;

%ExportedHeaderCode
 //START TRICK
#endif
#ifndef _SEB_CORE_TRICK_
#define _SEB_CORE_TRICK_
#ifdef _limacoreAPI_H
#include "sipAPIlimacore.h"
 //END TRICK

#include <ConfigUtils.h>
#ifdef WIN32
#pragma warning( disable : 4290 )
#endif

inline int _fill_setting_with_dict(Setting &setting,
				   PyObject *dict)
{
  PyObject *key, *value;
  Py_ssize_t pos = 0;
  int return_value = 0;
  try
    {
      while(PyDict_Next(dict, &pos, &key, &value))
	{
	  if(!PyString_Check(key))
	    {
	      PyErr_SetString(PyExc_ValueError,
			      "key can only by string type");
	      return_value = 1;
	      break;
	    }
       
	  if(PyString_Check(value))
	    setting.set(PyString_AS_STRING(key),
			PyString_AS_STRING(value));
	  else if(PyFloat_Check(value))
	    setting.set(PyString_AS_STRING(key),
			PyFloat_AS_DOUBLE(value));
	  else if(PyLong_Check(value))
	    {
#if PY_VERSION_HEX >= 0x02070000
	      int overflow;
	      long cValue = PyLong_AsLongAndOverflow(value,&overflow);
	      if(overflow == 1 || overflow == -1)
		{
		  PyErr_SetString(PyExc_ValueError,
				  "don't manage big int (overflow)");
		  return_value = 1;
		  break;
		}
#else
	      long cValue = PyLong_AsLong(value);
#endif
	      setting.set(PyString_AS_STRING(key),
			  cValue);
	    }
	  else if(PyInt_Check(value))
	    setting.set(PyString_AS_STRING(key),
			int(PyInt_AsLong(value)));
	  else if(PyBool_Check(value))
	    setting.set(PyString_AS_STRING(key),
			PyObject_IsTrue(value));
	  else if(PyDict_Check(value))
	    {
	      Setting child = setting.addChild(PyString_AS_STRING(key));
	      return_value = _fill_setting_with_dict(child,value);
	    }
	  else if(sipCanConvertToType(value,sipType_Data,SIP_NOT_NONE))
	    {
	      int state;
	      int error = 0;
	      int *sipIsErr = &error;
	      Data *data = 
		reinterpret_cast<Data*>(sipConvertToType(value,
							 sipType_Data, 0,	
							 SIP_NOT_NONE,
							 &state, sipIsErr));
	      if(*sipIsErr)
		{
		  sipReleaseType(data,sipType_Data,state);
		  delete data;
		  PyErr_SetString(PyExc_ValueError,
			      "Error data convertion");
		  return_value = 1;
		  break;
		}
	      else
		{
		  setting.set(PyString_AS_STRING(key),*data);
		  sipReleaseType(data,sipType_Data,state);
		}
	    }
	  else
	    {
	      PyErr_SetString(PyExc_ValueError,
			      "Type not yet managed");
	      return_value = 1;
	      break;
	    }
	}
    }
  catch(...)
    {
      Py_BEGIN_ALLOW_THREADS;
      throw;
      Py_END_ALLOW_THREADS;
    }
  return return_value;
}

inline bool _build_dict_from_setting(const Setting &setting,
				     PyObject* dict)
{
  bool return_value = 0;
  try
    {
      for(Setting::const_iterator i = setting.begin();
	  !return_value && i != setting.end();++i)
	{
	  Setting sub_setting = *i;
	  PyObject* py_value;
	  switch(sub_setting.getType())
	    {
	    case Setting::Int:
	      {
		int value = sub_setting;
		py_value = PyInt_FromLong(long(value));
		break;
	      }
	    case Setting::Int64:
	      {
		long long value = sub_setting;
		py_value = PyLong_FromLongLong(value);
		break;
	      }
	    case Setting::Float:
	      {
		double value = sub_setting;
		py_value = PyFloat_FromDouble(value);
		break;
	      }
	    case Setting::String:
	      {
		const char* value = sub_setting;
		py_value = PyString_FromString(value);
		break;
	      }
	    case Setting::Boolean:
	      {
		bool value = sub_setting;
		py_value = PyBool_FromLong(value);
		break;
	      }
	    case Setting::Group:
	      {
		py_value = PyDict_New();
		return_value = _build_dict_from_setting(sub_setting,
							py_value);
		break;
	      }
	    case Setting::DataType:
	      {
		Data value;
		setting.get(sub_setting.getName(),value);
		Data *dataPt = new Data(value);
		py_value = sipConvertFromNewType(dataPt,sipType_Data,NULL);
		break;
	      }
	    default:
	      PyErr_SetString(PyExc_ValueError,
			      "Type not yet managed");
	      py_value = NULL;
	      return_value = 1;
	      break;
	    }

	  if(!return_value)
	    return_value = PyDict_SetItemString(dict,
						sub_setting.getName(),
						py_value);
	  
	  if(py_value)
	    Py_DECREF(py_value);
	}
    }
  catch(Exception &exp)
    {
    const std::string &tmpString = exp.getErrMsg();
    const char* detail = tmpString.c_str();
      PyErr_SetString(sipException_Exception,detail);
      return_value = 1;
    }
  return return_value;
}
#endif
#endif
#ifndef _limacoreAPI_H_SEB
#define _limacoreAPI_H_SEB
%End

class CtConfig
{
%TypeHeaderCode
#include <CtConfig.h>
using namespace lima;
%End
public:
  static const char* All;

  CtConfig(CtControl &);
  ~CtConfig();
    
  // --- set current config into a context alias
  void store(const std::string& alias,
	     const char*);
  void store(const std::string& alias,
	     const std::list<const char*>&);
  // --- add current config to a context alias
  void update(const std::string& alias,
	      const char*);
  void update(const std::string& alias,
	      const std::list<const char*>&);
    
  void getAlias(std::list<std::string>& /Out/) const;
  void getAvailableModule(std::list<const char*>& /Out/) const;

  void apply(const std::string&);
  void pop(const std::string&);
  void remove(const std::string&,const char* = CtConfig::All);
  void remove(const std::string&,
	      const std::list<const char*>&);

  void setFilename(const std::string&);
  void getFilename(std::string& /Out/) const;

  void save();
  void load();
  
  class ModuleTypeCallback
    {
%TypeHeaderCode
#include <CtConfig.h>
using namespace lima;
%End


    public:
      explicit ModuleTypeCallback(const char*);

      virtual SIP_PYDICT store() = 0 [void (Setting&)];
%MethodCode
sipIsErr = 1;			// should never happen
%End

%VirtualCatcherCode
      PyObject *result;
      result = sipCallMethod(&sipIsErr, sipMethod,"");
      if(result)
	{
	  if(!PyDict_Check(result))
	    {
	      PyErr_SetString(PyExc_ValueError,
			      "Only dictionnary is allowed");
	      sipIsErr = 1;
	    }
	  else
	    sipIsErr = _fill_setting_with_dict(a0,result);
	}
%End
virtual void restore(SIP_PYDICT) = 0 [void (const Setting&)];
%MethodCode
sipIsErr = 1;			// should never happen
%End
%VirtualCatcherCode
      if(a0.isGroup())
	{
	  PyObject *dict = PyDict_New();
	  sipIsErr = _build_dict_from_setting(a0,dict);
	  if(!sipIsErr)
	    sipCallMethod(&sipIsErr,sipMethod,"R",dict);

	  Py_DECREF(dict);
	}
      else
	sipIsErr = 1;

%End
    protected:
      virtual ~ModuleTypeCallback();
%MethodCode
      Py_BEGIN_ALLOW_THREADS
      sipCpp->unref();
      sipCppV = NULL;
      Py_END_ALLOW_THREADS
%End
    };

  void registerModule(ModuleTypeCallback*);
  void unregisterModule(const std::string& module_type);

private:
  CtConfig(const CtConfig &);
};
