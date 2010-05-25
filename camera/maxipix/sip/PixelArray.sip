namespace Maxipix {

%TypeHeaderCode
#define NO_IMPORT_ARRAY
#define PY_ARRAY_UNIQUE_SYMBOL _LimaNumPy
#include "numpy/arrayobject.h"

#include "MaxipixDet.h"
#include "PixelArray.h"
#include <string>

using namespace lima;
%End

class PixelConfigArray {

  public:

    PixelConfigArray(Maxipix::MaxipixDet::Version);
    ~PixelConfigArray();

    void convert(std::string&);

    char* maskArray;
%GetCode
    if (!sipCpp->maskArray) return Py_None;
    npy_intp dims[2]= { 256, 256 };
    sipPy = PyArray_SimpleNewFromData(2, dims, NPY_UBYTE, sipCpp->maskArray);
%End
%SetCode
    PyArrayObject *tmpObject= (PyArrayObject*)PyArray_ContiguousFromObject(sipPy,NPY_NOTYPE,0,0);
    if(!tmpObject) sipErr= 1;

    if (!sipErr) {
        int nbBytes= PyArray_NBYTES(tmpObject);
        if (nbBytes != 256*256) sipErr= 1;

        if (!sipErr) {
	    sipCpp->maskArray= PyArray_BYTES(tmpObject);
        }
    }
    Py_DECREF(tmpObject);
%End

    char* testArray;
%GetCode
    if (!sipCpp->testArray) return Py_None;
    npy_intp dims[2]= { 256, 256 };
    sipPy = PyArray_SimpleNewFromData(2, dims, NPY_UBYTE, sipCpp->testArray);
%End
%SetCode
    PyArrayObject *tmpObject= (PyArrayObject*)PyArray_ContiguousFromObject(sipPy,NPY_NOTYPE,0,0);
    if(!tmpObject) sipErr= 1;

    if (!sipErr) {
        int nbBytes= PyArray_NBYTES(tmpObject);
        if (nbBytes != 256*256) sipErr= 1;

        if (!sipErr) {
	    sipCpp->testArray= PyArray_BYTES(tmpObject);
        }
    }
    Py_DECREF(tmpObject);
%End

    char* lowArray;
%GetCode
    if (!sipCpp->lowArray) return Py_None;
    npy_intp dims[2]= { 256, 256 };
    sipPy = PyArray_SimpleNewFromData(2, dims, NPY_UBYTE, sipCpp->lowArray);
%End
%SetCode
    PyArrayObject *tmpObject= (PyArrayObject*)PyArray_ContiguousFromObject(sipPy,NPY_NOTYPE,0,0);
    if(!tmpObject) sipErr= 1;

    if (!sipErr) {
        int nbBytes= PyArray_NBYTES(tmpObject);
        if (nbBytes != 256*256) sipErr= 1;

        if (!sipErr) {
	    sipCpp->lowArray= PyArray_BYTES(tmpObject);
        }
    }
    Py_DECREF(tmpObject);
%End

    char* highArray;
%GetCode
    if (!sipCpp->highArray) return Py_None;
    npy_intp dims[2]= { 256, 256 };
    sipPy = PyArray_SimpleNewFromData(2, dims, NPY_UBYTE, sipCpp->highArray);
%End
%SetCode
    PyArrayObject *tmpObject= (PyArrayObject*)PyArray_ContiguousFromObject(sipPy,NPY_NOTYPE,0,0);
    if(!tmpObject) sipErr= 1;

    if (!sipErr) {
        int nbBytes= PyArray_NBYTES(tmpObject);
        if (nbBytes != 256*256) sipErr= 1;

        if (!sipErr) {
	    sipCpp->highArray= PyArray_BYTES(tmpObject);
        }
    }
    Py_DECREF(tmpObject);
%End

};

};

