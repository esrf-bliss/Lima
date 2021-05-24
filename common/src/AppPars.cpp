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

#include "lima/AppPars.h"

using namespace std;
using namespace lima;

// AppArgs

AppArgs::AppArgs()
{
	DEB_CONSTRUCTOR();
	update_argc_argv();
}

AppArgs::AppArgs(unsigned int argc, char *argv[])
{
	DEB_CONSTRUCTOR();
	for (unsigned int i = 0; i < argc; ++i)
		m_arg_list.push_back(argv[i]);
	update_argc_argv();
}

AppArgs::AppArgs(const string& s)
{
	DEB_CONSTRUCTOR();
	set(s);
}

AppArgs::AppArgs(const AppArgs& o) : m_arg_list(o.m_arg_list)
{
	DEB_CONSTRUCTOR();
	update_argc_argv();
}

void AppArgs::set(const string& s)
{
	DEB_MEMBER_FUNCT();
	m_arg_list.clear();
	istringstream is(s);
	while (is) {
		string token;
		is >> token;
		m_arg_list.push_back(token);
	}
	update_argc_argv();
}

void AppArgs::clear()
{
	DEB_MEMBER_FUNCT();
	m_arg_list.clear();
	update_argc_argv();
}

AppArgs& AppArgs::operator =(const std::string& s)
{
	DEB_MEMBER_FUNCT();
	set(s);
	return *this;
}

string AppArgs::pop_front()
{
	DEB_MEMBER_FUNCT();
	string s = m_arg_list[0];
	erase(0);
	return s;
}

void AppArgs::erase(int pos)
{
	DEB_MEMBER_FUNCT();
	m_arg_list.erase(m_arg_list.begin() + pos);
	update_argc_argv();
}

void AppArgs::update_argc_argv()
{
	DEB_MEMBER_FUNCT();
	m_argc = m_arg_list.size();
	m_argv = new char *[m_argc + 1];
	for (unsigned int i = 0; i < m_argc; ++i)
		m_argv[i] = const_cast<char *>(m_arg_list[i].c_str());
	m_argv[m_argc] = NULL;
}



// AppPars

void AppPars::parseArgs(AppArgs& args)
{
	DEB_MEMBER_FUNCT();

	std::string n = args.pop_front();
	if (prog_name.empty())
		prog_name = n;

	while (args && (*args[0] == '-')) {
		OptList::const_iterator it, end = m_opt_list.cend();
		bool ok = false;
		for (it = m_opt_list.cbegin(); (it != end) && !ok; ++it)
			ok = ((*it)->check(args));
		if (!ok) {
			cerr << "Unknown option: " << args[0] << endl;
			exit(1);
		}
	}
}
