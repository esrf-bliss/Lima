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
#include "SinkTask.h"
#include "TaskMgr.h"

#include "CtSpsImage.h"

#include <sps.h>

using namespace lima;
//*********************************************************************
//* SPSImage
//*********************************************************************
class lima::_SpsImage
{
	DEB_CLASS_NAMESPC(DebModControl, "_SpsImage","Control");

public:
	_SpsImage();
	~_SpsImage();

	void setNames(const std::string& spec_name, 
		      const std::string& array_name);

	void setFrameDim(const FrameDim& frame_dim);

	void update(Data& data);

	FrameDim m_frame_dim;
private:
	void createSPS(const FrameDim& frame_dim);
	void deleteSPS();

	std::string m_spec_name;
	std::string m_array_name;
	void *m_shared_mem;
};
class lima::_SpsCBK: public TaskEventCallback
{
public:
	_SpsCBK(CtSpsImage &aCtSpsImage) : m_ct_sps(aCtSpsImage) {}
	virtual void finished(Data &aData)
	{
		m_ct_sps._update_finnished(aData);
	}
private:
	CtSpsImage &m_ct_sps;
};

class _UpdateTask : public SinkTaskBase
{
	DEB_CLASS_NAMESPC(DebModControl,"Sps update","Control");
public:
	_UpdateTask(_SpsImage &sps_image) : m_sps_image(sps_image) {}
	
	virtual void process(Data &aData)
	{
		m_sps_image.update(aData);
	}
private:
	_SpsImage &m_sps_image;
};

//Public class CtSpsImage
CtSpsImage::CtSpsImage() : m_ready_flag(true),m_active_flag(false)
{
	DEB_CONSTRUCTOR();

	m_sps_cbk = new _SpsCBK(*this);
	m_sps_cnt = new _SpsImage();
}

CtSpsImage::~CtSpsImage()
{
	DEB_DESTRUCTOR();
	m_sps_cbk->unref();
	delete m_sps_cnt;
	m_next_data.releaseBuffer();
}

void CtSpsImage::frameReady(Data &aData)
{
	DEB_MEMBER_FUNCT();
	
	AutoMutex l(m_cond.mutex());

	_check_data_size(aData);

	if(m_ready_flag)
		{
			
			m_ready_flag = false;
			_post_sps_task(aData);
		}
	else
		{
			m_next_data = aData;
		}
}

void CtSpsImage::_check_data_size(Data &data)
{
	DEB_MEMBER_FUNCT();

	ImageType image_type;
	switch (data.type) {

	case Data::UINT8:	image_type = Bpp8;  break;
	case Data::INT8:	image_type = Bpp8S;  break;
	case Data::UINT16: image_type = Bpp16; break;
	case Data::INT16: image_type = Bpp16S; break;
	case Data::UINT32: image_type = Bpp32; break;
	case Data::INT32: image_type = Bpp32S; break;
	default:
		THROW_CTL_ERROR(InvalidValue) << "Invalid " 
					      << DEB_VAR1(data.type);
	}
	
	FrameDim frame_dim(Size(data.width, data.height), image_type);
	if (frame_dim != m_sps_cnt->m_frame_dim)
		THROW_CTL_ERROR(InvalidValue) 
			<< "Data " << DEB_VAR1(frame_dim) << " does not match "
			<< DEB_VAR1(m_sps_cnt->m_frame_dim);
}
void CtSpsImage::_post_sps_task(Data &aData)
{
	DEB_MEMBER_FUNCT();

	_UpdateTask *newTaskPt = new _UpdateTask(*this->m_sps_cnt);
	newTaskPt->setEventCallback(m_sps_cbk);

	TaskMgr *aSpsTaskPt = new TaskMgr();
	aSpsTaskPt->addSinkTask(0,newTaskPt);
	aSpsTaskPt->setInputData(aData);

	PoolThreadMgr::get().addProcess(aSpsTaskPt);
}

void CtSpsImage::_update_finnished(Data &aData)
{
	DEB_MEMBER_FUNCT();

	AutoMutex l(m_cond.mutex());
	if((m_next_data.frameNumber >= 0) &&
	   (m_next_data.frameNumber != aData.frameNumber))
		_post_sps_task(m_next_data);
	else
		m_ready_flag = true;
	
	m_next_data = Data();
	
}
void CtSpsImage::setNames(const std::string& spec_name, 
			 const std::string& array_name)
{
	DEB_MEMBER_FUNCT();
	
	m_sps_cnt->setNames(spec_name,array_name);
}

void CtSpsImage::prepare(const FrameDim &frame_dim)
{
	DEB_MEMBER_FUNCT();

	m_sps_cnt->setFrameDim(frame_dim);
}

void CtSpsImage::reset()
{
	AutoMutex l(m_cond.mutex());
	
	m_next_data = Data();
	m_ready_flag = true;
	m_active_flag = false;
}
//*********************************************************************
//* SPSImage private
//*********************************************************************

_SpsImage::_SpsImage()
	: m_shared_mem(NULL)
{
	DEB_CONSTRUCTOR();
}

_SpsImage::~_SpsImage()
{
	DEB_DESTRUCTOR();
	deleteSPS();
}


void _SpsImage::setNames(const std::string& spec_name, 
			const std::string& array_name)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(spec_name, array_name);

	if ((spec_name == m_spec_name) && (array_name == m_array_name)) {
		DEB_TRACE() << "Nothing to do";
		return;
	}

	if (m_frame_dim.isValid())
		THROW_CTL_ERROR(InvalidValue) 
			<< "Cannot change the names of active array";

	m_spec_name = spec_name;
	m_array_name = array_name;
}

void _SpsImage::setFrameDim(const FrameDim& frame_dim)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(frame_dim, m_frame_dim);

	if (frame_dim == m_frame_dim) {
		DEB_TRACE() << "Nothing to do";
		return;
	}

	if (frame_dim.isValid())
		createSPS(frame_dim);
	else
		deleteSPS();
}

void _SpsImage::createSPS(const FrameDim& frame_dim)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(frame_dim, m_frame_dim);

	if (m_spec_name.empty() || m_array_name.empty())
		THROW_CTL_ERROR(InvalidValue) 
			<< "Must set the session/array names first";

	char *c_spec_name  = (char *) m_spec_name.c_str();
	char *c_array_name = (char *) m_array_name.c_str();

	const Size& size = frame_dim.getSize();

	ImageType image_type = frame_dim.getImageType();
	int sps_type;
	switch (image_type) {
	case Bpp8:
		sps_type = SPS_UCHAR;
		break;
	case Bpp8S:
		sps_type = SPS_CHAR;
		break;
	case Bpp10:
	case Bpp12:
	case Bpp14:
	case Bpp16:
		sps_type = SPS_USHORT;
		break;
	case Bpp16S:
		sps_type = SPS_SHORT;
		break;
	case Bpp32:
		sps_type = SPS_UINT;
		break;
	case Bpp32S:
		sps_type = SPS_INT;
		break;
	default:
		THROW_CTL_ERROR(InvalidValue) << "Unknown " 
					      << DEB_VAR1(image_type);
	}

	int ret = SPS_CreateArray(c_spec_name, c_array_name, 
				  size.getHeight(), size.getWidth(),
				  sps_type, SPS_IS_IMAGE);
	if (ret != 0)
		THROW_CTL_ERROR(Error) << "Error creating SPS array: " 
				       << DEB_VAR1(ret);

	m_shared_mem = SPS_GetDataPointer(c_spec_name, c_array_name, 1);
	if (m_shared_mem == NULL)
		THROW_CTL_ERROR(Error) << "Error getting SPS array pointer: "
			    << "is " << DEB_VAR1(frame_dim) << " too big?";

	DEB_TRACE() << DEB_VAR1(m_shared_mem);

	m_frame_dim = frame_dim;
}

void _SpsImage::deleteSPS()
{
	DEB_MEMBER_FUNCT();

	if (!m_frame_dim.isValid()) 
		return;

	DEB_TRACE() << "Returning pointer " << DEB_VAR1(m_shared_mem);
	SPS_ReturnDataPointer(m_shared_mem);
	m_frame_dim = FrameDim();
	m_shared_mem = NULL;
}

void _SpsImage::update(Data &data)
{
	DEB_MEMBER_FUNCT();
	void *mem_ptr = m_shared_mem;

	memcpy(mem_ptr, data.data(), data.size());
		
	char *c_spec_name  = (char *) m_spec_name.c_str();
	char *c_array_name = (char *) m_array_name.c_str();
	SPS_UpdateDone(c_spec_name, c_array_name);
}
