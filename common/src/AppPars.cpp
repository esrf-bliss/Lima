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

AppPars::AppPars()
	: m_print_help(false)
{
	m_opt_list.insert(MakeOpt(m_print_help, "", "--help", "",
				  "Print this help"));
}

void AppPars::parseArgs(AppArgs& args)
{
	DEB_MEMBER_FUNCT();

	std::string n = args.pop_front();
	if (prog_name.empty())
		prog_name = n;

	bool had_error = false;

	while (args && (*args[0] == '-')) {
		OptList::const_iterator it, end = m_opt_list.cend();
		bool ok = false;
		for (it = m_opt_list.cbegin(); (it != end) && !ok; ++it)
			ok = ((*it)->check(args));
		if (!ok) {
			cerr << "Unknown option: " << args[0] << endl << endl;
			had_error = true;
			break;
		}
	}

	if (had_error || m_print_help) {
		printHelp();
		exit(had_error ? 1 : 0);
	}
}

void AppPars::printHelp()
{
	DEB_MEMBER_FUNCT();
	const char *OptIndent = "   ";
	const char *DescIndent = "        ";
	cout << "Usage: " << prog_name << " [options]" << endl << endl
	     << "Options:" << endl;
	OptList::const_iterator it, end = m_opt_list.cend();
	for (it = m_opt_list.cbegin(); it != end; ++it) {
		cout << OptIndent;
		const ArgOptBase& o = **it;
		if (o.hasShortOpt())
			cout << o.m_sopt << (o.hasLongOpt() ? "," : " ");
		if (o.hasLongOpt())
			cout << o.m_lopt << (o.hasExtra() ? "=" : "");
		if (o.hasExtra())
			cout << "<" << o.m_extra << ">";
		cout << endl;
		if (o.hasDesc())
			cout << DescIndent << o.m_desc;
		cout << endl;
	}
	cout << endl;
}
