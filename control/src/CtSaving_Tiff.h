//###########################################################################
//
// Copyright (C) 2012 Alexander Lenz <alexander.lenz@frm2.tum.de>
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
#ifndef CTSAVING_TIFF_H
#define CTSAVING_TIFF_H

#include <tiffio.h>

#include "lima/CtSaving.h"

namespace lima {

  class SaveContainerTiff : public CtSaving::SaveContainer
  {
    DEB_CLASS_NAMESPC(DebModControl,"Saving TIFF Container","Control");
  public:
    SaveContainerTiff(CtSaving::Stream& stream);
    virtual ~SaveContainerTiff();
  protected:
    virtual void* _open(const std::string &filename,
			std::ios_base::openmode flags,
			CtSaving::Parameters& pars);
    virtual void _close(void*);
    virtual long _writeFile(void*,Data &data,
    			    CtSaving::HeaderMap &aHeader,
    			    CtSaving::FileFormat);
  private:

    CtSaving::FileFormat	 m_format;
    Mutex			 m_lock;
  };

}
#endif // CTSAVING_TIFF_H
