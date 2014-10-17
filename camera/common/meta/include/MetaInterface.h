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
#ifndef METAINTERFACE_H
#define METAINTERFACE_H
#include <map>

#include "HwInterface.h"

#include "Data.h"

namespace lima
{
  namespace Meta
  {
    class DetInfoCtrlObj;
    class SyncCtrlObj;
    class Interface : public HwInterface
    {
      DEB_CLASS_NAMESPC(DebModCamera, "MetaInterface", "Meta");
      friend class DetInfoCtrlObj;
      friend class SyncCtrlObj;
    public:
      enum Geometry {TILE};
      Interface(Geometry = TILE);
      virtual ~Interface();

      void addInterface(int row,int column,HwInterface*);
      
      //- From HwInterface
      virtual void	getCapList(CapList&) const;
      virtual void	reset(ResetLevel reset_level);
      virtual void	prepareAcq();
      virtual void	startAcq();
      virtual void	stopAcq();
      virtual void	getStatus(StatusType& status);
      virtual int	getNbHwAcquiredFrames();

    private:
      class _BufferFrameCBK;
      friend class _BufferFrameCBK;
      typedef std::pair<int,int> ColumnRow;
      struct ltColumnRow
      {
	bool operator()(const ColumnRow& a,const ColumnRow& b)
	{
	  return a.second == b.second ? 
	    a.first < b.first : a.second < b.second;
	}
      };
      typedef std::map<ColumnRow,HwInterface*,ltColumnRow> TileCnt;
      typedef std::map<ColumnRow,int> TileOffset;
      typedef std::map<int,int> PendingFrames;
      typedef std::vector<_BufferFrameCBK*> BufferFrameCBKs;

      bool _newFrameReady(int row,int column,const Data&);

      TileCnt		m_tiles;
      Geometry		m_geometry;

      DetInfoCtrlObj*	m_det_info;
      SyncCtrlObj*	m_sync;
      SoftBufferCtrlObj m_buffer;

      bool		m_dirty_geom_flag;
      int		m_width_step;
      ImageType		m_curr_type;
      TileOffset	m_tiles_offset;
      bool		m_continue_acq;
      Mutex		m_lock;
      PendingFrames	m_pending_frames;
      BufferFrameCBKs   m_buffer_cbks;
    };
  }
}
#endif
