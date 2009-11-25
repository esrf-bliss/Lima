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


	int acq_frame_nb;

	void *frame_ptr;
%GetCode
	if(!sipCpp->frame_ptr || !sipCpp->frame_dim)
   		return Py_None;

	Size size = sipCpp->frame_dim->getSize();
	npy_intp dims[2] = {
		size.getHeight(), size.getWidth()
	};

	int arr_type;
	ImageType image_type = sipCpp->frame_dim->getImageType();
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

	const FrameDim *frame_dim;
	Timestamp frame_timestamp;
	int valid_pixels;

	HwFrameInfoType();
	HwFrameInfoType(int frame_nb, SIP_PYOBJECT arr, FrameDim *dim,
			Timestamp timestamp, int pixels);
%MethodCode
	PyArrayObject *arr;
	arr = (PyArrayObject *) PyArray_ContiguousFromObject(a1, NPY_NOTYPE,
					                     0, 0);
	if (!arr) 
		return NULL; //should throw an error
	sipCpp = new HwFrameInfoType(a0, PyArray_DATA(a1), a2, *a3, a4);
%End
	const char* __repr__();
%MethodCode
	std::ostringstream str;
	str << *sipCpp;	
	sipRes = str.str().c_str();
%End
};


