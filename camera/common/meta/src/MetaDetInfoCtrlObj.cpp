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
#include <cstdlib>
#include "MetaDetInfoCtrlObj.h"
#include "MetaInterface.h"

using namespace lima;
using namespace lima::Meta;

#define GET_LOCAL_DET_INFO						\
  Interface::TileCnt::iterator i = m_interface.m_tiles.begin();		\
  HwDetInfoCtrlObj* local_det_info;					\
  if(i == m_interface.m_tiles.end())					\
    THROW_HW_ERROR(Error) << "Meta doesn't have any sub detector";	\
  if(!i->second->getHwCtrlObj(local_det_info))				\
    THROW_HW_ERROR(Error) << "Cannot get hardware det info";

class DetInfoCtrlObj::_MaxImageSizeCallback : public HwMaxImageSizeCallback
{
public:
  _MaxImageSizeCallback(DetInfoCtrlObj &det) : m_det(det) {}
protected:
  virtual void maxImageSizeChanged(const Size& size,ImageType image_type)
  {
    m_det._maxImageSizeChanged();
  }
private:
  DetInfoCtrlObj& m_det;
};

DetInfoCtrlObj::DetInfoCtrlObj(Interface& interface):
  m_interface(interface)
{
}

DetInfoCtrlObj::~DetInfoCtrlObj()
{
  for(std::list<_MaxImageSizeCallback*>::iterator i = m_cbk.begin();
      i != m_cbk.end();++i)
    delete *i;
}

void DetInfoCtrlObj::getMaxImageSize(Size& max_image_size)
{
  DEB_MEMBER_FUNCT();

  max_image_size = Size(0,0);

  GET_LOCAL_DET_INFO;

  Size local_max_image_size;
  int prev_row = i->first.second;
  local_det_info->getMaxImageSize(local_max_image_size);
  int row_width = local_max_image_size.getWidth();
  int row_height = local_max_image_size.getHeight();
  for(++i;i != m_interface.m_tiles.end();++i)
    {
      if(!i->second->getHwCtrlObj(local_det_info))
	THROW_HW_ERROR(Error) << "Cannot get hardware det info";
      local_det_info->getMaxImageSize(local_max_image_size);

      if(prev_row == i->first.second) // new column, same row
	{
	  row_height = std::max(row_height,
				local_max_image_size.getHeight());
	  row_width += local_max_image_size.getWidth();
	}
      else			// new row
	{
	  prev_row = i->first.second;
	  max_image_size = Size(std::max(max_image_size.getWidth(),row_width),
				max_image_size.getHeight() + row_height);
	  row_height = local_max_image_size.getHeight();
	  row_width = local_max_image_size.getWidth();
	}
    }
  max_image_size = Size(std::max(max_image_size.getWidth(),row_width),
			max_image_size.getHeight() + row_height);

  DEB_RETURN() << DEB_VAR1(max_image_size);
}

void DetInfoCtrlObj::getDetectorImageSize(Size& det_image_size)
{
  DEB_MEMBER_FUNCT();

  getMaxImageSize(det_image_size);

}

void DetInfoCtrlObj::getDefImageType(ImageType& def_image_type)
{
  DEB_MEMBER_FUNCT();

  GET_LOCAL_DET_INFO;

  local_det_info->getDefImageType(def_image_type);

  DEB_RETURN() << DEB_VAR1(def_image_type);
}

void DetInfoCtrlObj::getCurrImageType(ImageType& curr_image_type)
{
  DEB_MEMBER_FUNCT();

  getDefImageType(curr_image_type);
}

void DetInfoCtrlObj::setCurrImageType(ImageType curr_image_type)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(curr_image_type);

  for(Interface::TileCnt::iterator i = m_interface.m_tiles.begin();
      i != m_interface.m_tiles.end();++i)
    {
      HwDetInfoCtrlObj* local_det_info;
      if(!i->second->getHwCtrlObj(local_det_info))
	THROW_HW_ERROR(Error) << "Cannot get hardware det info"; 
      local_det_info->setCurrImageType(curr_image_type);
    }
}
/** @brief get pixel size of detector
    assumed that all detector have the same resolution, it might be wrong in some cases.
*/
void DetInfoCtrlObj::getPixelSize(double& x_size,double& y_size)
{
  DEB_MEMBER_FUNCT();
  
  GET_LOCAL_DET_INFO;

  local_det_info->getPixelSize(x_size,y_size);

  DEB_RETURN() << DEB_VAR2(x_size,y_size);
}

void DetInfoCtrlObj::getDetectorType(std::string& det_type)
{
  DEB_MEMBER_FUNCT();

  GET_LOCAL_DET_INFO;
  std::string local_det_type;
  local_det_info->getDetectorType(local_det_type);

  bool allMatch = true;
  for(++i;allMatch && i != m_interface.m_tiles.end();++i)
    {
      std::string loop_det_type;
      if(!i->second->getHwCtrlObj(local_det_info))
	THROW_HW_ERROR(Error) << "Cannot get hardware det info"; 
      local_det_info->getDetectorType(loop_det_type);
      allMatch = loop_det_type == local_det_type;
    }

  if(allMatch)
    det_type = "Meta_" + local_det_type;
  else
    det_type = "Meta_Hybrid";

  DEB_RETURN() << DEB_VAR1(det_type);
}

void DetInfoCtrlObj::getDetectorModel(std::string& det_model)
{
  DEB_MEMBER_FUNCT();
  
  det_model = "";

  for(Interface::TileCnt::iterator i = m_interface.m_tiles.begin();
      i != m_interface.m_tiles.end();++i)
    {
      std::string local_det_model;
      HwDetInfoCtrlObj* local_det_info;
      if(!i->second->getHwCtrlObj(local_det_info))
	THROW_HW_ERROR(Error) << "Cannot get hardware det info"; 
      
      local_det_info->getDetectorModel(local_det_model);
      if(!det_model.size())
	det_model = local_det_model;
      else
	det_model += " / " + local_det_model;
    }
}

void DetInfoCtrlObj::registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb)
{
  this->HwMaxImageSizeCallbackGen::registerMaxImageSizeCallback(cb);
}

void DetInfoCtrlObj::unregisterMaxImageSizeCallback(HwMaxImageSizeCallback &cb)
{
  this->HwMaxImageSizeCallbackGen::unregisterMaxImageSizeCallback(cb);
}

void DetInfoCtrlObj::_maxImageSizeChanged()
{
  Size max_image_size;
  getMaxImageSize(max_image_size);

  ImageType def_image_type;
  getCurrImageType(def_image_type);

  maxImageSizeChanged(max_image_size,def_image_type);

  m_interface.m_dirty_geom_flag = true;
}

void DetInfoCtrlObj::_addInterface(HwInterface* i)
{
  DEB_MEMBER_FUNCT();

  _MaxImageSizeCallback *cbk = new _MaxImageSizeCallback(*this);
  HwDetInfoCtrlObj* local_det_info;
  if(!i->getHwCtrlObj(local_det_info))
    THROW_HW_ERROR(Error) << "Cannot get hardware det info"; 

  local_det_info->registerMaxImageSizeCallback(*cbk);
  m_cbk.push_back(cbk);
}
