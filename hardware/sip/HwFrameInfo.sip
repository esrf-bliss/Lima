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
struct HwFrameInfoType
{
%TypeHeaderCode
#define NO_IMPORT_ARRAY
#define PY_ARRAY_UNIQUE_SYMBOL _LimacoreNumPy
#include "numpy/arrayobject.h"
#include "lima/HwFrameInfo.h"
#include <sstream>
using namespace lima;
%End
        enum OwnerShip {Managed,Transfer,Shared};

	int acq_frame_nb;

	void *frame_data
{
%GetCode
	if(!sipCpp->frame_ptr || !sipCpp->frame_dim.isValid())
   		Py_RETURN_NONE;

	Size size = sipCpp->frame_dim.getSize();
	npy_intp dims[2] = {
		size.getHeight(), size.getWidth()
	};

	int arr_type;
	ImageType image_type = sipCpp->frame_dim.getImageType();
	switch (image_type) {
	case Bpp8:  	arr_type = NPY_UBYTE;  	break;
	case Bpp8S:  	arr_type = NPY_BYTE;  	break;
	case Bpp16: 	arr_type = NPY_USHORT; 	break;
	case Bpp16S: 	arr_type = NPY_SHORT; 	break;
	case Bpp32: 	arr_type = NPY_UINT32; 	break;
	case Bpp32S: 	arr_type = NPY_INT32; 	break;
	default:
		return NULL;
	}

	sipPy = PyArray_SimpleNewFromData(2, dims, arr_type, 
					  sipCpp->frame_ptr);
%End
%SetCode
PyArrayObject *tmpObject = (PyArrayObject*)PyArray_ContiguousFromObject(sipPy,NPY_NOTYPE,0,0);
if(!tmpObject) sipErr = 1;

if(!sipErr)
{
  int nbDim = PyArray_NDIM(tmpObject);
  if(nbDim != 2) sipErr = 1;

  if(!sipErr)
  {
    ImageType image_type;
    switch(PyArray_TYPE(tmpObject))
    {
    case NPY_BYTE: 	image_type = Bpp8S; 	break;
    case NPY_UBYTE:  	image_type = Bpp8;  	break;
    case NPY_SHORT:	image_type = Bpp16S; 	break;
    case NPY_USHORT: 	image_type = Bpp16; 	break;
    case NPY_INT32:	image_type = Bpp32S; 	break;
    case NPY_UINT32: 	image_type = Bpp32; 	break;
    default:
    sipErr = 1;
    }
    
    if(!sipErr)
    {

      sipCpp->frame_dim = FrameDim(PyArray_DIM(tmpObject,1),
	        			   PyArray_DIM(tmpObject,0),
				           image_type);
      sipCpp->frame_ptr = PyArray_DATA(tmpObject);
    }
  }
}
Py_DECREF(tmpObject);
%End
};
	Timestamp frame_timestamp;
	int valid_pixels;
        HwFrameInfoType::OwnerShip buffer_owner_ship;

	HwFrameInfoType();
	HwFrameInfoType(int frame_nb, SIP_PYOBJECT arr,
			Timestamp timestamp, int pixels,
			HwFrameInfoType::OwnerShip);
%MethodCode
	PyArrayObject *tmpObject = (PyArrayObject *) PyArray_ContiguousFromObject(a1, NPY_NOTYPE,
							                    0, 0);
	if (tmpObject) 
	{
	  int nbDim = PyArray_NDIM(tmpObject);
  	  if(nbDim == 2)
	  {
 	    ImageType image_type;
    	    switch(PyArray_TYPE(tmpObject))
    	    {
	      case NPY_BYTE:	image_type = Bpp8S;  	break;
    	      case NPY_UBYTE:  	image_type = Bpp8;  	break;
    	      case NPY_SHORT:	image_type = Bpp16S; 	break;
    	      case NPY_USHORT: 	image_type = Bpp16; 	break;
    	      case NPY_INT32:	image_type = Bpp32S; 	break;
              case NPY_UINT32: 	image_type = Bpp32; 	break;
    	      default:
		goto clean;
            }
    
	      FrameDim tmpFrameDim(PyArray_DIM(tmpObject,1),
				   PyArray_DIM(tmpObject,0),
				   image_type);
              
	      sipCpp = new HwFrameInfoType(a0, PyArray_DATA(tmpObject),&tmpFrameDim, *a2, a3, a4);
	  }
	clean:
	  Py_DECREF(tmpObject);
	}
%End
	SIP_PYOBJECT __repr__() const;
%MethodCode
        LIMA_REPR_CODE
%End
};


