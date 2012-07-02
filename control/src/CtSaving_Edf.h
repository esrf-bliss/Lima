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
    SaveContainerEdf(CtSaving::Stream& stream);
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
#ifdef WIN32
    class _OfStream
    {
    public:
      _OfStream();
      ~_OfStream();

      void clear();
      void exceptions(int exc);
      void open(const char* filename,
		std::ios_base::openmode openFlags = 
		std::ios_base::in | std::ios_base::out);
      bool is_open() const;
      void close();
      long tellp() const;
      inline _OfStream& write(const char* data,int size);
      inline _OfStream& operator<< (const char *data);
      inline _OfStream& operator<< (const std::string& data);
      inline _OfStream& operator<< (const int data);
      inline _OfStream& operator<< (const long data);
    private:
      FILE* m_fout;
      int m_exc_flag;
    };
    _OfStream			 m_fout;
#else
    std::ofstream                m_fout;
#endif
  };

}
#endif // CTSAVING_EDF_H
