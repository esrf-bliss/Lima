//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2026
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
class BufferAllocMgr
{
%TypeHeaderCode
#include "lima/HwBufferMgr.h"
using namespace lima;
%End
 public:
	BufferAllocMgr();
	virtual ~BufferAllocMgr();

	virtual void setAllocParameters(const BufferHelper::Parameters& alloc_params);
	virtual void getAllocParameters(      BufferHelper::Parameters& alloc_params /Out/);
	virtual void prepareAlloc(int nb_buffers, const FrameDim& frame_dim);

	virtual int getMaxNbBuffers(const FrameDim& frame_dim) = 0;
	virtual void allocBuffers(int nb_buffers, 
				  const FrameDim& frame_dim) = 0;
	virtual const FrameDim& getFrameDim() = 0;
	virtual void getNbBuffers(int& nb_buffers) = 0;
	virtual void releaseBuffers() = 0;

	virtual void *getBufferPtr(int buffer_nb) = 0;

	virtual void clearBuffer(int buffer_nb);
	virtual void clearAllBuffers();

 private:
	BufferAllocMgr(const BufferAllocMgr& o);
};

class SoftBufferAllocMgr : BufferAllocMgr
{
%TypeHeaderCode
#include "lima/HwBufferMgr.h"
using namespace lima;
%End
 public:
	SoftBufferAllocMgr();
	virtual ~SoftBufferAllocMgr();

	virtual void setAllocParameters(const BufferHelper::Parameters& alloc_params);
	virtual void getAllocParameters(      BufferHelper::Parameters& alloc_params /Out/);
	virtual void prepareAlloc(int nb_buffers, const FrameDim& frame_dim);

	virtual int getMaxNbBuffers(const FrameDim& frame_dim);
	virtual void allocBuffers(int nb_buffers, 
				  const FrameDim& frame_dim);
	virtual const FrameDim& getFrameDim();
	virtual void getNbBuffers(int& nb_buffers);
	virtual void releaseBuffers();

	virtual void *getBufferPtr(int buffer_nb);
	
};

class BufferCbMgr : HwFrameCallbackGen
{
%TypeHeaderCode
#include "lima/HwBufferMgr.h"
using namespace lima;
%End
 public:
	enum Cap {
		Basic=0, Concat=1, Acc=2, // bit mask
	};

	BufferCbMgr();
	virtual ~BufferCbMgr();

	virtual Cap getCap() = 0;

	virtual int getMaxNbBuffers(const FrameDim& frame_dim, 
				    int nb_concat_frames) = 0;
	virtual void allocBuffers(int nb_buffers, int nb_concat_frames, 
				  const FrameDim& frame_dim) = 0;
	virtual const FrameDim& getFrameDim() = 0;
	virtual void getNbBuffers(int& nb_buffers) = 0;
	virtual void getNbConcatFrames(int& nb_concat_frames) = 0;
	virtual void releaseBuffers() = 0;

	virtual void *getBufferPtr(int buffer_nb, int concat_frame_nb) = 0;

	virtual void clearBuffer(int buffer_nb);
	virtual void clearAllBuffers();

	virtual void setStartTimestamp(Timestamp  start_ts);
	virtual void getStartTimestamp(Timestamp& start_ts /Out/);

	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info /Out/) = 0;

	virtual void getBufferFrameDim(const FrameDim& single_frame_dim,
				       int nb_concat_frames, 
				       FrameDim& buffer_frame_dim /Out/);
	virtual void acqFrameNb2BufferNb(int acq_frame_nb, int& buffer_nb /Out/,
					 int& concat_frame_nb /Out/);
};

class StdBufferCbMgr : BufferCbMgr
{
%TypeHeaderCode
#include "lima/HwBufferMgr.h"
using namespace lima;
%End
 public:
	StdBufferCbMgr(BufferAllocMgr& alloc_mgr);
	virtual ~StdBufferCbMgr();

	virtual Cap getCap();

	virtual int getMaxNbBuffers(const FrameDim& frame_dim, 
				    int nb_concat_frames);
	virtual void allocBuffers(int nb_buffers, int nb_concat_frames, 
				  const FrameDim& frame_dim);
	virtual const FrameDim& getFrameDim();
	virtual void getNbBuffers(int& nb_buffers);
	virtual void getNbConcatFrames(int& nb_concat_frames);
	virtual void releaseBuffers();

	virtual void *getFrameBufferPtr(int frame_nb);
	virtual void *getBufferPtr(int buffer_nb, int concat_frame_nb);
	int getFrameBufferNb(void *ptr);

	virtual void clearBuffer(int buffer_nb);
	virtual void clearAllBuffers();

	void setKeepSidebandData(bool  keep_sideband_data);
	void getKeepSidebandData(bool& keep_sideband_data /Out/);

	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info /Out/);

	bool newFrameReady(HwFrameInfoType& frame_info);

 protected:
	virtual void setFrameCallbackActive(bool cb_active);
 public:
        void copy_data(int frame_nb, SIP_PYOBJECT arr);
%MethodCode
       PyArrayObject *tmpObject = (PyArrayObject *) PyArray_ContiguousFromObject(a1, NPY_NOTYPE,
						                    0, 0);
	if(tmpObject)
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
            if (tmpFrameDim.getMemSize() > sipCpp->getFrameDim().getMemSize())
	    {
              PyErr_SetString(PyExc_ValueError, "Frame size cannot be bigger than the allocated buffer frame size");
	      sipIsErr = 1;
	    }
	    else
	    {
	      void *frame_ptr = NULL;
	      frame_ptr = sipCpp->getFrameBufferPtr(a0);
	      if (!frame_ptr)
	      {
	        sipIsErr = 1;
	      }
	      else
	      {
	        memcpy(frame_ptr,PyArray_DATA(tmpObject),tmpFrameDim.getMemSize());
              }
	    }
          }
	clean:
	   Py_DECREF(tmpObject);
	}
	else
	{
	  sipIsErr = 1;
	}
%End

};

class BufferCtrlMgr
{
%TypeHeaderCode
#include "lima/HwBufferMgr.h"
using namespace lima;
%End
 public:
	enum AcqMode {
		Normal, Concat, Acc,
	};

	BufferCtrlMgr(BufferCbMgr& acq_buffer_mgr);
	~BufferCtrlMgr();

	void setFrameDim(const FrameDim& frame_dim);
	void getFrameDim(      FrameDim& frame_dim /Out/);

	void setNbConcatFrames(int  nb_concat_frames);
	void getNbConcatFrames(int& nb_concat_frames /Out/);

	void setNbBuffers(int  nb_buffers);
	void getNbBuffers(int& nb_buffers /Out/);

	void getMaxNbBuffers(int& max_nb_buffers /Out/);

	void *getBufferPtr(int buffer_nb, int concat_frame_nb = 0);
	void *getFramePtr(int acq_frame_nb);

	void setStartTimestamp(Timestamp  start_ts);
	void getStartTimestamp(Timestamp& start_ts /Out/);

	void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info /Out/);

	void   registerFrameCallback(HwFrameCallback& frame_cb);
	void unregisterFrameCallback(HwFrameCallback& frame_cb);

	BufferCbMgr& getAcqBufferMgr();
	AcqMode getAcqMode();
};


class SoftBufferCtrlObj : public HwBufferCtrlObj
{
%TypeHeaderCode
#include "lima/HwBufferMgr.h"
using namespace lima;
%End
 public:
	SoftBufferCtrlObj();

	virtual void setAllocParameters(const BufferHelper::Parameters& alloc_params);
	virtual void getAllocParameters(      BufferHelper::Parameters& alloc_params /Out/);
	virtual void setFrameDim(const FrameDim& frame_dim);
	virtual void getFrameDim(FrameDim& frame_dim /Out/);

	virtual void prepareAlloc(int nb_buffers);

	virtual void setNbBuffers(int  nb_buffers);
	virtual void getNbBuffers(int& nb_buffers /Out/);

	virtual void setNbConcatFrames(int nb_concat_frames);
	virtual void getNbConcatFrames(int& nb_concat_frames /Out/);

	virtual void getMaxNbBuffers(int& max_nb_buffers /Out/);

	virtual void *getBufferPtr(int buffer_nb, int concat_frame_nb = 0);
	virtual void *getFramePtr(int acq_frame_nb);

	virtual void getStartTimestamp(Timestamp& start_ts /Out/);
	virtual void getFrameInfo(int acq_frame_nb, HwFrameInfoType& info /Out/);

	virtual void   registerFrameCallback(HwFrameCallback& frame_cb);
	virtual void unregisterFrameCallback(HwFrameCallback& frame_cb);

	StdBufferCbMgr&  getBuffer();

	int getNbAcquiredFrames();

 private:
	SoftBufferCtrlObj(const SoftBufferCtrlObj& o);
};
