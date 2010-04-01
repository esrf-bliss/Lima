%Module lima 0

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
	aReturnFlag = PyString_Check(PyList_GetItem(sipPy,i));
return aReturnFlag;
}
 
*sipCppPtr = new std::vector<std::string>(PyList_Size(sipPy));
for(int i = 0;i < PyList_Size(sipPy);++i)
  (*sipCppPtr)->operator[](i) = PyString_AsString(PyList_GetItem(sipPy,i));

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

%MappedType std::vector<bool>
{
%TypeHeaderCode
#include <vector>
%End

%ConvertToTypeCode
if (sipIsErr == NULL)
{	
bool aReturnFlag = PyList_Check(sipPy);
for(int i = 0;aReturnFlag && i < PyList_Size(sipPy);++i)
	aReturnFlag = PyBool_Check(PyList_GetItem(sipPy,i));
return aReturnFlag;
}
 
*sipCppPtr = new std::vector<bool>(PyList_Size(sipPy));
for(int i = 0;i < PyList_Size(sipPy);++i)
  (*sipCppPtr)->operator[](i) = PyList_GetItem(sipPy,i) == Py_True;

    return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
PyObject* aReturnListPt = PyList_New(sipCpp->size());
std::vector<bool>::iterator vIter = sipCpp->begin();

for(int i = 0;vIter != sipCpp->end();++vIter,++i)
  PyList_SET_ITEM(aReturnListPt,i,PyBool_FromLong(*vIter));

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
for(int i = 0;aReturnFlag && i < PyList_Check(sipPy);++i)
	aReturnFlag = PyFloat_Check(PyList_GetItem(sipPy,i));
return aReturnFlag;
}

*sipCppPtr = new std::list<double>();
for(int i = 0;i < PyList_Size(sipPy);++i)
	(*sipCppPtr)->push_back(PyFloat_AS_DOUBLE(PyList_GetItem(sipPy,i)));
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



%ModuleHeaderCode
extern "C" { void lima_import_array();}
%End

%PostInitialisationCode
lima_import_array();
%End
