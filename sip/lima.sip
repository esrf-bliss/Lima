%Module lima 0

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
#include <map>
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

%ModuleHeaderCode
extern "C" { void lima_import_array();}
%End

%PostInitialisationCode
lima_import_array();
%End
