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
#ifndef CTSAVING_FITS_H
#define CTSAVING_FITS_H

#include "lima/CtSaving.h"

#include <memory>

namespace CCfits
{
class FITS;
}

namespace lima {

  class SaveContainerFits : public CtSaving::SaveContainer
  {
    DEB_CLASS_NAMESPC(DebModControl,"Saving FITS Container","Control");
  public:
    SaveContainerFits(CtSaving::Stream& stream);
    virtual ~SaveContainerFits();
  protected:
    virtual void* _open(const std::string &filename,
			std::ios_base::openmode flags,
			CtSaving::Parameters& pars);
    virtual void _close(void*);
    virtual long _writeFile(void*,Data &data,
			    CtSaving::HeaderMap &aHeader,
			    CtSaving::FileFormat);
  private:
    void writeHeader(std::auto_ptr<CCfits::FITS> &fitsFile, CtSaving::HeaderMap &header);
    void writeData(std::auto_ptr<CCfits::FITS> &fitsFile, Data &data, short dataType);
  };

}
#endif // CTSAVING_FITS_H
