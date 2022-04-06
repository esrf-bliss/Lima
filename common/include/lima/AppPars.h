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

#ifndef __APP_PARS_H
#define __APP_PARS_H

#include "lima/LimaCompatibility.h"
#include "lima/Debug.h"
#include "lima/AutoObj.h"
#include "lima/Exceptions.h"

#include <iomanip>
#include <fstream>
#include <set>
#include <vector>

namespace std
{

std::ostream& operator <<(std::ostream& os, const std::vector<int>& v);
std::istream& operator >>(std::istream& is, std::vector<int>& v);

}

namespace lima 
{

class LIMACORE_API AppArgs
{
	DEB_CLASS(DebModCommon, "AppArgs");

public:
	AppArgs();
	AppArgs(unsigned int argc, char *argv[]);
	AppArgs(const std::string& s);
	AppArgs(const AppArgs& o);

	void set(const std::string& s);
	void clear();

	unsigned int size()
	{ return m_argc; }
	operator char **()
	{ return m_argv; }
	operator bool()
	{ return bool(m_argc); }
	char *operator[](int pos)
	{ return m_argv[pos]; }

	AppArgs& operator =(const std::string& s);

	std::string pop_front();
	void erase(int pos);

private:
	typedef std::vector<std::string> StringList;

	void update_argc_argv();
	
	StringList m_arg_list;
	unsigned int m_argc;
	AutoPtr<char *, true> m_argv;
};

class LIMACORE_API AppPars
{
	DEB_CLASS(DebModCommon, "AppPars");
public:
	std::string prog_name;

	AppPars();
	virtual ~AppPars() {}

	virtual void parseArgs(AppArgs& args);
	virtual void printHelp();

protected:
	class ArgOptBase
	{
	public:
		ArgOptBase(std::string sopt, std::string lopt,
			   std::string extra = "", std::string desc = "")
			: m_sopt(sopt), m_lopt(lopt), m_extra(extra),
			  m_desc(desc)
		{}
		virtual ~ArgOptBase()
		{}

		virtual bool check(AppArgs& args) const = 0;

		bool hasShortOpt() const
		{ return !m_sopt.empty(); }
		bool hasLongOpt() const
		{ return !m_lopt.empty(); }
		bool hasExtra() const
		{ return !m_extra.empty(); }
		bool hasDesc() const
		{ return !m_desc.empty(); }

	protected:
		friend class AppPars;

		std::string m_sopt;
		std::string m_lopt;
		std::string m_extra;
		std::string m_desc;
	};

	template <class T>
	static void OptDecoder(T& var, const std::string& val)
	{ std::istringstream(val) >> var; }

	template <class T>
	class ArgOpt : public ArgOptBase
	{
	public:
		ArgOpt(T& var, std::string sopt, std::string lopt,
		       std::string extra = "", std::string desc = "")
			: ArgOptBase(sopt, lopt, extra, desc), m_var(var)
		{
			if (!hasShortOpt() && !hasLongOpt())
				throw LIMA_HW_EXC(InvalidValue,
						  "ArgOpt: short & long empty");
			if (!hasExtra())
				std::istringstream("0") >> m_var;
		}

		virtual bool check(AppArgs& args) const
		{
			std::string s = args[0];
			if (!check(s, m_sopt) && !check(s, m_lopt))
				return false;
			args.pop_front();
			if (hasExtra() && s.empty() && !args) {
				std::cerr << "Missing " << m_extra
				     << std::endl;
				exit(1);
			}
			if (!hasExtra())
				s = std::string("1");
			else if (s.empty())
				s = args.pop_front();
			OptDecoder(m_var, s);
			return true;	
		}

	protected:
		bool check(std::string& s, const std::string &o) const
		{
			if (o.empty() || (s.find(o) != 0))
				return false;
			bool exact = (s.size() == o.size());
			if (!exact && ((o == m_sopt) || !hasExtra() ||
				       (s[o.size()] != '=')))
				return false;
			s.erase(0, o.size() + (!exact ? 1 : 0));
			return true;
		}

		T& m_var;
	};

	template <class T, class ... Args>
	static AutoPtr<ArgOptBase> MakeOpt(T& var, Args&& ... args)
	{ return new ArgOpt<T>(var, std::forward<Args>(args)...); }

	typedef std::set<AutoPtr<ArgOptBase>> OptList;

	bool m_print_help;
	OptList m_opt_list;
};

template <>
inline void AppPars::OptDecoder<std::string>(std::string& var,
					     const std::string& s)
{ var = s; }

template <>
inline void AppPars::OptDecoder<bool>(bool& var, const std::string& s)
{ var = ((s == "yes") || (s == "on") || (s == "1")); }

} // namespace lima

#endif // __APP_PARS_H
