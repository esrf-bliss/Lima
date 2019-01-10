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
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include "lima/LimaCompatibility.h"

#include "lima/Debug.h"

#include <string.h>
#include <ostream>
#include <sstream>

namespace lima
{

enum Layer {
        Common, Control, Hardware, CameraPlugin,
};

enum ErrorType {
	InvalidValue, NotSupported, Error,
};


class LIMACORE_API ExcDebProxy
{
 public:
	ExcDebProxy(DebProxy *deb_proxy = NULL);
	ExcDebProxy(const ExcDebProxy& o);
	~ExcDebProxy();

	operator DebProxy *() const;

 private:
	struct Data {
		AutoCounter count;
		DebProxy *deb_proxy;
		Data(DebProxy *d = NULL);
	};

	struct Data *getData() const;
	bool putData() const;

	Data *m_d;
};


class LIMACORE_API Exception
{
 public:
	Exception(Layer layer, ErrorType err_type, const std::string& err_desc,
		  const std::string& file_name, const std::string& funct_name,
		  int line_nr, ExcDebProxy exc_deb_proxy);

	Layer       getLayer()     const;
	ErrorType   getErrType()   const;
	std::string getErrDesc()   const;
	std::string getFileName()  const;
	std::string getFunctName() const;

	std::string getErrMsg()    const;

	template <class T>
	Exception& operator <<(const T& o);

 private:
	Layer m_layer;
	ErrorType m_err_type;
	std::string m_err_desc;
	std::string m_file_name;
	std::string m_funct_name;
	int m_line_nr;
	ExcDebProxy m_exc_deb_proxy;
};

template <class T>
Exception& Exception::operator <<(const T& o)
{
	std::ostringstream os;
	os << o;
	m_err_desc += os.str();

	DebProxy *deb_proxy = m_exc_deb_proxy;
	if (deb_proxy)
		*deb_proxy << o;

	return *this;
}


LIMACORE_API std::ostream& operator <<(std::ostream& os, Layer layer);
LIMACORE_API std::ostream& operator <<(std::ostream& os, ErrorType err_type);
LIMACORE_API std::ostream& operator <<(std::ostream& os, const Exception& e);

#define LIMA_EXC(layer, err_type, err_desc) \
	Exception(layer, err_type, err_desc, __FILE__, __FUNCTION__, __LINE__,\
		  NULL)

#define LIMA_COM_EXC(err_type, err_desc) \
	LIMA_EXC(Common, err_type, err_desc)

#define LIMA_CTL_EXC(err_type, err_desc) \
	LIMA_EXC(Control, err_type, err_desc)

#define LIMA_HW_EXC(err_type, err_desc) \
	LIMA_EXC(Hardware, err_type, err_desc)

#ifndef LIMA_NO_DEBUG
#define LIMA_EXC_DEB(layer, err_type, deb_err_type) \
	Exception(layer, err_type, "", __FILE__, __FUNCTION__, __LINE__,\
		  new DebProxy(DEB_MSG(deb_err_type)))
#else
#define LIMA_EXC_DEB(layer, err_type, deb_err_type) \
  	LIMA_EXC(layer,err_type,"")
#endif


#define THROW_MSG(layer, err_type, deb_err_type) \
	throw LIMA_EXC_DEB(layer, err_type, deb_err_type)

#define THROW_FATAL(layer, err_type)				\
	THROW_MSG(layer, err_type, DebTypeFatal)
#define THROW_ERROR(layer, err_type)				\
	THROW_MSG(layer, err_type, DebTypeError)

#define THROW_COM_FATAL(err_type)	\
	THROW_FATAL(Common, err_type)
#define THROW_CTL_FATAL(err_type)		\
	THROW_FATAL(Control, err_type)
#define THROW_HW_FATAL(err_type)		\
	THROW_FATAL(Hardware, err_type)

#define THROW_COM_ERROR(err_type)		\
	THROW_ERROR(Common, err_type)
#define THROW_CTL_ERROR(err_type)		\
	THROW_ERROR(Control, err_type)
#define THROW_HW_ERROR(err_type)		\
	THROW_ERROR(Hardware, err_type)

} // namespace lima

#endif // EXCEPTIONS_H
