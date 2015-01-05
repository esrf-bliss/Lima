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
#ifndef CTSPSIMAGE_H
#define CTSPSIMAGE_H

#include <string>

#include "lima/ThreadUtils.h"
#include "lima/Debug.h"
#include "lima/SizeUtils.h"

#include "processlib/Data.h"

namespace lima
{

class _SpsCBK;
class _SpsImage;

class CtSpsImage
{
	DEB_CLASS_NAMESPC(DebModControl,"CtSpsImage","Control");

 public:
	CtSpsImage();
	~CtSpsImage();
	
	void setNames(const std::string& spec_name, 
		      const std::string& array_name);
        void getNames(std::string &spec_name, std::string &array_name) const;
	void prepare(const FrameDim &frame_dim);
	void frameReady(Data&);
	void reset();
	void setActive(bool aFlag);
	bool isActive() const;

 private:
	friend class _SpsCBK;
	void _update_finnished(Data&);
	void _check_data_size(Data&);
	void _post_sps_task(Data&);

	Cond		m_cond;
	bool		m_ready_flag;
	bool 		m_active_flag;
	
	_SpsCBK 	*m_sps_cbk;
	_SpsImage	*m_sps_cnt;
	Data		m_next_data;
	
};

inline void CtSpsImage::setActive(bool aFlag) 
{
	m_active_flag = aFlag;
}

inline bool CtSpsImage::isActive() const 
{
	return m_active_flag;
}

} // namespace lima

#endif // CTSPSIMAGE_H
