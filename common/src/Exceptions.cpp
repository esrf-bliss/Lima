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
#include "lima/Exceptions.h"

#include <sstream>

using namespace lima;
using namespace std;


ExcDebProxy::ExcDebProxy(DebProxy *deb_proxy)
{
	m_d = new Data(deb_proxy);
}

ExcDebProxy::ExcDebProxy(const ExcDebProxy& o)
{
	m_d = o.getData();
}

ExcDebProxy::~ExcDebProxy()
{
	if (m_d->deb_proxy) {
		*m_d->deb_proxy << " [thrown]";
		delete m_d->deb_proxy;
		m_d->deb_proxy = NULL;
	}

	if (putData())
		delete m_d;
}

struct ExcDebProxy::Data *ExcDebProxy::getData() const
{
	m_d->count.get();
	return m_d;
}

bool ExcDebProxy::putData() const
{
	return m_d->count.put();
}

ExcDebProxy::Data::Data(DebProxy *d)
	: deb_proxy(d)
{
}

ExcDebProxy::operator DebProxy *() const
{
	return m_d->deb_proxy;
}


Exception::Exception(Layer layer, ErrorType err_type, const string& err_desc,
		     const string& file_name, const string& funct_name, 
		     int line_nr, ExcDebProxy exc_deb_proxy)
	: m_layer(layer), m_err_type(err_type), m_err_desc(err_desc),
	  m_file_name(file_name), m_funct_name(funct_name), m_line_nr(line_nr),
	  m_exc_deb_proxy(exc_deb_proxy)
{
	DebProxy *deb_proxy = m_exc_deb_proxy;
	if (deb_proxy)
		*deb_proxy << "Exception(" << getErrType() << "): " 
			   << getErrDesc();
}

Layer Exception::getLayer() const
{
	return m_layer;
}

ErrorType Exception::getErrType() const
{
	return m_err_type;
}

string Exception::getErrDesc() const
{
	return m_err_desc;
}

string Exception::getFileName() const
{
	return m_file_name;
}

string Exception::getFunctName() const
{
	return m_funct_name;
}

string Exception::getErrMsg() const
{
	ostringstream os;
	os << m_layer << ": " 
	   << m_funct_name  << "(" << m_file_name << ", " 
	   << m_line_nr << "): " 
	   << m_err_type << ": " << m_err_desc;
	return os.str();
}

ostream& lima::operator <<(ostream& os, Layer layer)
{
	string name = "Unknown";
	switch (layer) {
	case Common:	name = "Common";	break;
	case Control:	name = "Control";	break;
	case Hardware:	name = "Hardware";	break;
	}
	return os << name;
}

ostream& lima::operator <<(ostream& os, ErrorType err_type)
{
	string name = "Unknown";
	switch (err_type) {
	case InvalidValue:	name = "InvalidValue";	break;
	case NotSupported:	name = "NotSupported";	break;
	case Error:		name = "Error";		break;
	}
	return os << name;
}

ostream& lima::operator <<(ostream& os, const Exception& e)
{
	return os << e.getErrMsg();
}

