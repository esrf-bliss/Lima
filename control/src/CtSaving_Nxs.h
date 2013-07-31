//###########################################################################
//
// Copyright (C) 2012 Arafat Noureddine <arafat.noureddine@synchrotron-soleil.fr>
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
#ifndef CTSAVING_NXS_H
#define CTSAVING_NXS_H



#include <cstdio>
#include <iostream>
#include "CtSaving.h"

#include <nexuscpp/nexuscpp.h>


//--------------------------------------------------------------------------------------------------------------------
namespace lima
{

  class SaveContainerNxs : public CtSaving::SaveContainer
  {
	  DEB_CLASS_NAMESPC(DebModControl,"Saving NXS Container","Control");
	public:
	  SaveContainerNxs(CtSaving::Stream& stream);
	  virtual ~SaveContainerNxs();
	protected:
	  virtual bool _open(const std::string &filename, std::ios_base::openmode flags);
	  virtual void _close();
	  virtual void _writeFile(Data &data, CtSaving::HeaderMap &aHeader, CtSaving::FileFormat);
	private:
      nxcpp::DataStreamer* 	m_writer;
	  CtSaving::Parameters	m_pars;
  };

}
//--------------------------------------------------------------------------------------------------------------------
#endif // CTSAVING_NXS_H
