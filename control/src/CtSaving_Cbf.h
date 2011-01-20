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
#ifndef CTSAVING_CBF_H
#define CTSAVING_CBF_H

#include <cbf.h>

#include "CtSaving.h"

namespace lima {

  class SaveContainerCbf : public CtSaving::SaveContainer
  {
    DEB_CLASS_NAMESPC(DebModControl,"Saving CBF Container","Control");
    class Compression;
  public:
    SaveContainerCbf(CtSaving &ct_saving);
    virtual ~SaveContainerCbf();

    virtual bool needParralelCompression() const {return true;}
    virtual SinkTaskBase* getCompressionTask(const CtSaving::HeaderMap&);

  protected:
    virtual bool _open(const std::string &filename,
		       std::_Ios_Openmode flags);
    virtual void _close();
    virtual void _writeFile(Data &data,
			    CtSaving::HeaderMap &aHeader,
			    CtSaving::FileFormat);
    virtual void _clear();
  private:
    inline int _writeCbfHeader(Data&,CtSaving::HeaderMap&);
    inline int _writeCbfData(Data&);
    
    
    
    typedef std::map<int,cbf_handle> dataId2cbfHandle;
    void _setHandle(int dataId,cbf_handle);
    cbf_handle _takeHandle(int dataId);
    
    FILE* 		m_fout;
    dataId2cbfHandle 	m_cbfs;
    cbf_handle  	m_current_cbf;
    Mutex		m_lock;
  };

}
#endif // CTSAVING_CBF_H
