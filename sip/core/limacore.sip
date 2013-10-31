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
%Module(name=limacore,version=0)

%Platforms {WIN32_PLATFORM WIN64_PLATFORM POSIX_PLATFORM}

%Feature WITH_CONFIG

%ModuleHeaderCode
  #include <SoftOpId.h>
%End

%MappedType std::string
{
%TypeHeaderCode
#include <string>
%End

%ConvertToTypeCode
if (sipIsErr == NULL)
    return PyString_Check(sipPy);

    *sipCppPtr = new std::string(PyString_AsString(sipPy), PyString_Size(sipPy));

    return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
return PyString_FromStringAndSize(sipCpp->data(),sipCpp->length());
%End

};

%MappedType std::vector<std::string>
{
%TypeHeaderCode
#include <string>
#include <vector>
%End

%ConvertToTypeCode
if (sipIsErr == NULL)
{	
bool aReturnFlag = PyList_Check(sipPy);
for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
	aReturnFlag = PyString_Check(PyList_GET_ITEM(sipPy,i));
return aReturnFlag;
}
 
*sipCppPtr = new std::vector<std::string>(PyList_Size(sipPy));
for(int i = 0;i < PyList_Size(sipPy);++i)
  (*sipCppPtr)->operator[](i) = PyString_AsString(PyList_GET_ITEM(sipPy,i));

    return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
PyObject* aReturnListPt = PyList_New(sipCpp->size());
std::vector<std::string>::iterator vIter = sipCpp->begin();

for(int i = 0;vIter != sipCpp->end();++vIter,++i)
   PyList_SET_ITEM(aReturnListPt,i,PyString_FromStringAndSize(vIter->data(),vIter->length()));

return aReturnListPt;
%End

};

%MappedType std::vector<int>
{
%TypeHeaderCode
#include <vector>
%End

%ConvertToTypeCode
if (sipIsErr == NULL)
{	
bool aReturnFlag = PyList_Check(sipPy);
for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
	aReturnFlag = PyInt_Check(PyList_GET_ITEM(sipPy,i));
return aReturnFlag;
}
 
*sipCppPtr = new std::vector<int>(PyList_Size(sipPy));
for(int i = 0;i < PyList_Size(sipPy);++i)
  (*sipCppPtr)->operator[](i) = PyInt_AS_LONG(PyList_GET_ITEM(sipPy,i));

    return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
PyObject* aReturnListPt = PyList_New(sipCpp->size());
std::vector<int>::iterator vIter = sipCpp->begin();

for(int i = 0;vIter != sipCpp->end();++vIter,++i)
  PyList_SET_ITEM(aReturnListPt,i,PyInt_FromLong(*vIter));

return aReturnListPt;
%End

};

%MappedType std::vector<long>
{
%TypeHeaderCode
#include <vector>
%End

%ConvertToTypeCode
if (sipIsErr == NULL)
{
bool aReturnFlag = PyList_Check(sipPy);
for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
        aReturnFlag = PyInt_Check(PyList_GET_ITEM(sipPy,i));
return aReturnFlag;
}

*sipCppPtr = new std::vector<long>(PyList_Size(sipPy));
for(int i = 0;i < PyList_Size(sipPy);++i)
  (*sipCppPtr)->operator[](i) = PyInt_AS_LONG(PyList_GET_ITEM(sipPy,i));

    return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
PyObject* aReturnListPt = PyList_New(sipCpp->size());
std::vector<long>::iterator vIter = sipCpp->begin();

for(int i = 0;vIter != sipCpp->end();++vIter,++i)
  PyList_SET_ITEM(aReturnListPt,i,PyInt_FromLong(*vIter));

return aReturnListPt;
%End

};

%MappedType std::list<double>
{
%TypeHeaderCode
#include <list>
%End

%ConvertToTypeCode
if(sipIsErr == NULL)
{
bool aReturnFlag = PyList_Check(sipPy);
for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
	aReturnFlag = PyFloat_Check(PyList_GET_ITEM(sipPy,i));
return aReturnFlag;
}

*sipCppPtr = new std::list<double>();
for(int i = 0;i < PyList_Size(sipPy);++i)
	(*sipCppPtr)->push_back(PyFloat_AS_DOUBLE(PyList_GET_ITEM(sipPy,i)));
return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
PyObject* aReturnListPt = PyList_New(sipCpp->size());
std::list<double>::iterator lIter = sipCpp->begin();

for(int i = 0;lIter != sipCpp->end();++lIter,++i)
   PyList_SET_ITEM(aReturnListPt,i,PyFloat_FromDouble(*lIter));
return aReturnListPt;
%End
};

%MappedType std::list<int>
{
%TypeHeaderCode
#include <list>
%End

%ConvertToTypeCode
if(sipIsErr == NULL)
{
bool aReturnFlag = PyList_Check(sipPy);
for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
	aReturnFlag = PyInt_Check(PyList_GET_ITEM(sipPy,i));
return aReturnFlag;
}

*sipCppPtr = new std::list<int>();
for(int i = 0;i < PyList_Size(sipPy);++i)
	(*sipCppPtr)->push_back(PyInt_AS_LONG(PyList_GET_ITEM(sipPy,i)));
return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
PyObject* aReturnListPt = PyList_New(sipCpp->size());
std::list<int>::iterator lIter = sipCpp->begin();

for(int i = 0;lIter != sipCpp->end();++lIter,++i)
   PyList_SET_ITEM(aReturnListPt,i,PyInt_FromLong(*lIter));
return aReturnListPt;
%End
};

%MappedType std::map<int,std::string>
{
%TypeHeaderCode
#include <map>
#include <string>
%End

%ConvertToTypeCode
PyObject *key, *value;
SIP_SSIZE_T pos;

if(sipIsErr == NULL)
{
  bool aReturnFlag = PyDict_Check(sipPy);

  pos = 0;
  while(aReturnFlag && PyDict_Next(sipPy, &pos, &key, &value))
     aReturnFlag = PyInt_Check(key) && PyString_Check(value);

  return aReturnFlag;
}

*sipCppPtr = new std::map<int,std::string>();
pos = 0;
while(PyDict_Next(sipPy, &pos, &key, &value))
{
  int anIndex = PyInt_AS_LONG(key);
  char *s = PyString_AsString(value);
  std::pair<int,std::string> item(anIndex,s);
  (*sipCppPtr)->insert(item);
}
return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
PyObject* aReturnDictPt = PyDict_New();

std::map<int,std::string>::iterator mIter, mEnd = sipCpp->end();
for(mIter = sipCpp->begin(); mIter != mEnd; ++mIter)
   {
    PyObject *aKeyPt = PyInt_FromLong(mIter->first);
    std::string *sPt = &mIter->second;
    PyObject *aValuePt = PyString_FromStringAndSize(sPt->data(),sPt->length());
    PyDict_SetItem(aReturnDictPt,aKeyPt,aValuePt);
    Py_DECREF(aKeyPt);Py_DECREF(aValuePt);
   }
return aReturnDictPt;
%End
};

%MappedType std::map<int,std::list<std::string>>
{
%TypeHeaderCode
#include <map>
#include <list>
#include <string>
%End

%ConvertToTypeCode
PyObject *key, *value;
SIP_SSIZE_T pos;

if(sipIsErr == NULL)
{
  bool aReturnFlag = PyDict_Check(sipPy);

  pos = 0;
  while(aReturnFlag && PyDict_Next(sipPy, &pos, &key, &value))
   {
     aReturnFlag = PyInt_Check(key) && PyList_Check(value);
     for(int i = 0;aReturnFlag && i < PyList_Size(value);++i)
	aReturnFlag = PyString_Check(PyList_GET_ITEM(value,i));
   }
  return aReturnFlag;
}

*sipCppPtr = new std::map<int,std::list<std::string> >();
pos = 0;
while(PyDict_Next(sipPy, &pos, &key, &value))
{
  int anIndex = PyInt_AS_LONG(key);
  std::pair<std::map<int,std::list<std::string> >::iterator, bool> aResult = 
  (*sipCppPtr)->insert(std::pair<int,std::list<std::string> >(anIndex,std::list<std::string>()));
  
  for(int i = 0;i < PyList_Size(value);++i)
  {
    char *s = PyString_AsString(PyList_GET_ITEM(value,i));
    aResult.first->second.push_back(s);
  }
}
return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
PyObject* aReturnDictPt = PyDict_New();


for(std::map<int,std::list<std::string> >::iterator mIter = sipCpp->begin();
    mIter != sipCpp->end();++mIter)
   {
    PyObject *aKeyPt = PyInt_FromLong(mIter->first);
    PyObject *aValuePt = PyList_New(mIter->second.size());
    PyDict_SetItem(aReturnDictPt,aKeyPt,aValuePt);
    Py_DECREF(aKeyPt);Py_DECREF(aValuePt);
    std::list<std::string>::iterator sPt = mIter->second.begin();
    for(int i = 0;sPt != mIter->second.end();++i,++sPt)
      PyList_SET_ITEM(aValuePt,i,PyString_FromStringAndSize(sPt->data(),sPt->length()));
   }
return aReturnDictPt;
%End
};

%MappedType std::list<std::string>
{
%TypeHeaderCode
#include <list>
#include <string>
%End

%ConvertToTypeCode
if(sipIsErr == NULL)
{
bool aReturnFlag = PyList_Check(sipPy);
for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
  aReturnFlag = PyString_Check(PyList_GET_ITEM(sipPy,i));
return aReturnFlag;
}


*sipCppPtr = new std::list<std::string>();
for(int i = 0;i < PyList_Size(sipPy);++i)
  (*sipCppPtr)->push_back(PyString_AsString(PyList_GET_ITEM(sipPy,i)));

return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
PyObject* aReturnListPt = PyList_New(sipCpp->size());
std::list<std::string>::iterator vIter = sipCpp->begin();

for(int i = 0;vIter != sipCpp->end();++vIter,++i)
   PyList_SET_ITEM(aReturnListPt,i,PyString_FromStringAndSize(vIter->data(),vIter->length()));

return aReturnListPt;
%End
};

%MappedType std::list<const char*>
{
%TypeHeaderCode
#include <list>
%End

%ConvertToTypeCode
if(sipIsErr == NULL)
{
bool aReturnFlag = PyList_Check(sipPy);
for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
  aReturnFlag = PyString_Check(PyList_GET_ITEM(sipPy,i));
return aReturnFlag;
}


*sipCppPtr = new std::list<const char*>();
for(int i = 0;i < PyList_Size(sipPy);++i)
  (*sipCppPtr)->push_back(PyString_AsString(PyList_GET_ITEM(sipPy,i)));

return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
PyObject* aReturnListPt = PyList_New(sipCpp->size());
std::list<const char*>::iterator vIter = sipCpp->begin();

for(int i = 0;vIter != sipCpp->end();++vIter,++i)
   PyList_SET_ITEM(aReturnListPt,i,PyString_FromString(*vIter));

return aReturnListPt;
%End
};

%MappedType std::list<std::list<long long>>
{
%TypeHeaderCode
#include <list>
%End

%ConvertToTypeCode
if(sipIsErr == NULL)
{
  bool aReturnFlag = PyList_Check(sipPy);
  for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
  {
    PyObject *listPt = PyList_GET_ITEM(sipPy,i);
    aReturnFlag = PyList_Check(listPt);
    for(int j = 0;aReturnFlag && j < PyList_Size(listPt);++j)
      aReturnFlag = PyInt_Check(PyList_GET_ITEM(listPt,j));
  }
  return aReturnFlag;
}


*sipCppPtr = new std::list<std::list<long long> >();
for(int i = 0;i < PyList_Size(sipPy);++i)
{
  PyObject *listPt = PyList_GET_ITEM(sipPy,i);
  (*sipCppPtr)->push_back(std::list<long long>());
  std::list<long long> &aList = (*sipCppPtr)->back();
  for(int j = 0;j <  PyList_Size(listPt);++j)
    aList.push_back(PyInt_AS_LONG(PyList_GET_ITEM(listPt,j)));
}

return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
PyObject* aReturnListPt = PyList_New(sipCpp->size());
std::list<std::list<long long> >::iterator vIter = sipCpp->begin();

for(int i = 0;vIter != sipCpp->end();++vIter,++i)
{
   std::list<long long> &aList = *vIter;
   PyObject* aListPyPt = PyList_New(aList.size());
   PyList_SET_ITEM(aReturnListPt,i,aListPyPt);
   std::list<long long>::iterator sIter = aList.begin();
   for(int j = 0;sIter != aList.end();++sIter,++j)
     PyList_SET_ITEM(aListPyPt,j,PyInt_FromLong(*sIter));
}
return aReturnListPt;
%End
};

%ModuleHeaderCode
extern "C" { void lima_import_array();}
%End

%PostInitialisationCode
lima_import_array();
%End
