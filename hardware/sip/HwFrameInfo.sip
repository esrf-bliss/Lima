struct HwFrameInfoType
{
%TypeHeaderCode
#define NO_IMPORT_ARRAY
#define PY_ARRAY_UNIQUE_SYMBOL _LimaNumPy
#include "numpy/arrayobject.h"
#include <HwFrameInfo.h>
#include <sstream>
using namespace lima;
%End
        enum OwnerShip {Managed,Transfer};

	int acq_frame_nb;

	void *frame_data;
%GetCode
	if(!sipCpp->frame_ptr || !sipCpp->frame_dim.isValid())
   		return Py_None;

	Size size = sipCpp->frame_dim.getSize();
	npy_intp dims[2] = {
		size.getHeight(), size.getWidth()
	};

	int arr_type;
	ImageType image_type = sipCpp->frame_dim.getImageType();
	switch (image_type) {
	case Bpp8:  arr_type = NPY_UBYTE;  break;
	case Bpp16: arr_type = NPY_USHORT; break;
	case Bpp32: arr_type = NPY_UINT32; break;
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
    switch(tmpObject->descr->type_num)
    {
    case NPY_BYTE:
    case NPY_UBYTE:  image_type = Bpp8;  break;
    case NPY_SHORT:
    case NPY_USHORT: image_type = Bpp16; break;
    case NPY_INT32:
    case NPY_UINT32: image_type = Bpp32; break;
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
    	    switch(tmpObject->descr->type_num)
    	    {
	      case NPY_BYTE:
    	      case NPY_UBYTE:  image_type = Bpp8;  break;
    	      case NPY_SHORT:
    	      case NPY_USHORT: image_type = Bpp16; break;
    	      case NPY_INT32:
              case NPY_UINT32: image_type = Bpp32; break;
    	      default:
		goto clean;
            }
    
	      FrameDim tmpFrameDim(PyArray_DIM(tmpObject,1),
				   PyArray_DIM(tmpObject,0),
				   image_type);
              
	      sipCpp = new HwFrameInfoType(a0, PyArray_DATA(a1),&tmpFrameDim, *a2, a3, a4);
	  }
	clean:
	  Py_DECREF(tmpObject);
	}
%End
	const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	sipRes = str.str().c_str();
%End
};


