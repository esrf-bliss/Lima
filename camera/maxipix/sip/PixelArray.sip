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

    void convert(std::string& /Out/);

    void* maskArray;
%GetCode
    if (!sipCpp->maskArray) Py_RETURN_NONE;
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

    void* testArray;
%GetCode
    if (!sipCpp->testArray) Py_RETURN_NONE;
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

    void* lowArray;
%GetCode
    if (!sipCpp->lowArray) Py_RETURN_NONE;
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

    void* highArray;
%GetCode
    if (!sipCpp->highArray) Py_RETURN_NONE;
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

