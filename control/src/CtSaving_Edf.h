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
#ifndef CTSAVING_EDF_H
#define CTSAVING_EDF_H

#include "CtSaving.h"

namespace lima {

  class SaveContainerEdf : public CtSaving::SaveContainer
  {
    DEB_CLASS_NAMESPC(DebModControl,"Saving EDF Container","Control");
  public:
    SaveContainerEdf(CtSaving &ct_saving);
    virtual ~SaveContainerEdf();
  protected:
    virtual bool _open(const std::string &filename,
		       std::ios_base::openmode flags);
    virtual void _close();
    virtual void _writeFile(Data &data,
			    CtSaving::HeaderMap &aHeader,
			    CtSaving::FileFormat);
  private:
    void _writeEdfHeader(Data&,CtSaving::HeaderMap&);

    std::ofstream                m_fout;
  };

}
#endif // CTSAVING_EDF_H
