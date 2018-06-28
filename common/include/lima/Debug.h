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
// This code was taken from the Debug.h file (Generic debug in C++)
// in: /segfs/dserver/classes++/extdevice/

#ifndef DEBUG_H
#define DEBUG_H

#include "lima/LimaCompatibility.h"
#include "lima/StreamUtils.h"
#include "lima/ThreadUtils.h"

#include <string>
#include <map>
#ifndef __unix
#pragma warning(disable:4251)
#endif
namespace lima
{

// Do not forget to update the corresponding Type/Format/Module name in
// DebParams::checkInit (Debug.cpp) when you change these enums

enum DebType {
	DebTypeFatal		= 1 << 0,
	DebTypeError		= 1 << 1,
	DebTypeWarning		= 1 << 2,
	DebTypeTrace		= 1 << 3,
	DebTypeFunct		= 1 << 4,
	DebTypeParam		= 1 << 5,
	DebTypeReturn		= 1 << 6,
	DebTypeAlways		= 1 << 7,
};

enum DebFormat {
	DebFmtDateTime		= 1 << 0,
	DebFmtThread		= 1 << 1,
	DebFmtModule		= 1 << 2,
	DebFmtObj		= 1 << 3,
	DebFmtFunct		= 1 << 4,	
	DebFmtFileLine		= 1 << 5,
	DebFmtType		= 1 << 6,
	DebFmtIndent		= 1 << 7,
	DebFmtColor		= 1 << 8,
};

enum DebModule {
	DebModNone		= 1 << 0,
	DebModCommon		= 1 << 1,
	DebModHardware		= 1 << 2,
	DebModHardwareSerial	= 1 << 3,
	DebModControl		= 1 << 4,
	DebModEspia		= 1 << 5,
	DebModEspiaSerial	= 1 << 6,
	DebModFocla		= 1 << 7,
	DebModCamera		= 1 << 8,
	DebModCameraCom		= 1 << 9,
	DebModTest		= 1 << 10,
	DebModApplication	= 1 << 11,
};

typedef const char *ConstStr;

/*------------------------------------------------------------------
 *  class DebStream
 *------------------------------------------------------------------*/

class LIMACORE_API DebStream : public std::ostream
{
 public:
	typedef OCopyStream::StreamList StreamList;
	typedef StreamList::iterator ListIterator;
	enum Selector {
		None, Output, Error, Both
	};

	DebStream();
	
	DebStream& SetStream(Selector new_selector);
	
	bool FindStream(std::ostream *os, Selector buffer);
	void AddOutput(std::ostream *os);
	void RemoveOutput(std::ostream *os);
	void AddError(std::ostream *os);
	void RemoveError(std::ostream *os);

 protected:
	bool Find(std::ostream *os, StreamList& slist, ListIterator& it);

 private:
	StreamList m_out_list;
	StreamList m_err_list;
	StreamList m_all_list;
	
	NullStreamBuf m_null_buf;
	OCopyStream m_out_streams;
	OCopyStream m_err_streams;
	CopyStreamBuf m_all_buf;
	
	Selector m_current;
	std::streambuf *m_buffers[4];
};

inline DebStream& DebStream::SetStream(Selector new_selector)
{
	m_current = new_selector;
	rdbuf(m_buffers[m_current]);
	return *this;
}

/*------------------------------------------------------------------
 *  class DebParams 
 *------------------------------------------------------------------*/

class DebObj;
class DebProxy;

class LIMACORE_API DebParams
{
 public:
	typedef long long Flags;
	typedef std::vector<std::string> NameList;

	static const Flags AllFlags;

	DebParams(DebModule mod = DebModNone, 
		  ConstStr class_name = NULL, ConstStr name_space = NULL);

	void setModule(DebModule mod);
	DebModule getModule() const;

	void setClassName(ConstStr class_name);
	ConstStr getClassName() const;

	void setNameSpace(ConstStr name_space);
	ConstStr getNameSpace() const;

	bool checkModule() const;
	bool checkType(DebType type) const;

	static void checkTypeFlags(Flags& type_flags);
	static void setTypeFlags(Flags type_flags);
	static Flags getTypeFlags();

	static void enableTypeFlags(Flags type_flags);
	static void disableTypeFlags(Flags type_flags);

	static void setFormatFlags(Flags fmt_flags);
	static Flags getFormatFlags();

	static void enableFormatFlags(Flags fmt_flags);
	static void disableFormatFlags(Flags fmt_flags);

	static void setModuleFlags(Flags mod_flags);
	static Flags getModuleFlags();

	static void enableModuleFlags(Flags mod_flags);
	static void disableModuleFlags(Flags mod_flags);

	static void setTypeFlagsNameList(const NameList& type_name_list);
	static NameList getTypeFlagsNameList();

	static void setFormatFlagsNameList(const NameList& fmt_name_list);
	static NameList getFormatFlagsNameList();

	static void setModuleFlagsNameList(const NameList& mod_name_list);
	static NameList getModuleFlagsNameList();

	static DebStream& getDebStream();
	static AutoMutex lock();

	static ConstStr getTypeName(DebType type);
	static ConstStr getFormatName(DebFormat fmt);
	static ConstStr getModuleName(DebModule mod);

	static void checkInit();

private:
	friend class DebProxy;
	friend class DebObj;

	static void doInit();

	template <class T>
	static void setFlagsNameList(Flags& flags, 
				     const std::map<T, std::string>& name_map,
				     const NameList& name_list);
	template <class T>
	static void getFlagsNameList(Flags flags, 
				     const std::map<T, std::string>& name_map,
				     NameList& name_list);

	static Flags s_type_flags;
	static Flags s_fmt_flags;
	static Flags s_mod_flags;

	static DebStream *s_deb_stream;

	static std::map<DebType,   std::string> *s_type_name_map;
	static std::map<DebFormat, std::string> *s_fmt_name_map;
	static std::map<DebModule, std::string> *s_mod_name_map;

	static Mutex *s_mutex;

	DebModule m_mod;
	ConstStr m_class_name;
	ConstStr m_name_space;
};

std::ostream& operator <<(std::ostream& os, 
			  const DebParams::NameList& name_list);


/*------------------------------------------------------------------
 *  class DebProxy
 *------------------------------------------------------------------*/

class LIMACORE_API DebProxy
{
 public:
	DebProxy();
	DebProxy(DebObj *deb_obj, DebType type, ConstStr funct_name, 
		 ConstStr file_name, int line_nr);
	DebProxy(const DebProxy& p);
	~DebProxy();

	template <class T> 
        const DebProxy& operator <<(const T& o) const;

	bool isActive() const;

 private:
	mutable AutoMutex *m_lock;
};

/*------------------------------------------------------------------
 *  class DebObj
 *------------------------------------------------------------------*/

class LIMACORE_API DebObj
{
 public:
	enum {
		IndentSize = 4,
	};

	DebObj(DebParams& deb_params, bool destructor = false,
	       ConstStr funct_name = NULL, ConstStr obj_name = NULL, 
	       ConstStr file_name = NULL, int line_nr = 0);
	~DebObj();

	bool checkOut(DebType type);
	bool checkErr(DebType type);
	bool checkAny(DebType type);

	DebParams& getDebParams();

	DebProxy write(DebType type, 
		       ConstStr file_name = NULL, int line_nr = 0);

 private:
	friend class DebProxy;

	typedef struct ThreadData {
		int indent;
		ThreadData() : indent(-1) {}
	} ThreadData;
	
	static void heading(DebType type, ConstStr funct_name, 
			    ConstStr file_name, int line_nr,
			    DebObj *deb = NULL);

	static ThreadData *getThreadData();
	static void deleteThreadData(void *thread_data);

	DebParams *m_deb_params;
	bool m_destructor;
	ConstStr m_funct_name;
	ConstStr m_obj_name;
	ConstStr m_file_name;
	int m_line_nr;
};


/*------------------------------------------------------------------
 *  class DebHex
 *------------------------------------------------------------------*/

class LIMACORE_API DebHex
{
 public:
	DebHex(unsigned long val) : m_val(val) 
	{}

	unsigned long getVal() const
	{ return m_val; }

 private:
	unsigned long m_val;
};

inline std::ostream& operator <<(std::ostream& os, const DebHex& deb_hex)
{
	return os << std::hex << std::showbase << deb_hex.getVal()
		  << std::dec << std::noshowbase;
}


/*------------------------------------------------------------------
 *  global inline functions
 *------------------------------------------------------------------*/

inline bool DebHasFlag(DebParams::Flags flags, int val)
{
	return ((flags & val) != 0);
}

/*------------------------------------------------------------------
 *  class DebParams inline functions
 *------------------------------------------------------------------*/

inline DebParams::DebParams(DebModule mod, ConstStr class_name, 
			    ConstStr name_space)
{
	checkInit();
	m_mod = mod;
	m_class_name = class_name;
	m_name_space = name_space;
}

inline void DebParams::setModule(DebModule mod)
{
	m_mod = mod; 
}

inline DebModule DebParams::getModule() const
{ 
	return m_mod; 
}

inline void DebParams::setClassName(ConstStr class_name)
{ 
	m_class_name = class_name; 
}

inline ConstStr DebParams::getClassName() const
{ 
	return m_class_name; 
}

inline void DebParams::setNameSpace(ConstStr name_space)
{ 
	m_name_space = name_space; 
}

inline ConstStr DebParams::getNameSpace() const
{ 
	return m_name_space; 
}

inline bool DebParams::checkModule() const
{ 
	return DebHasFlag(s_mod_flags, m_mod); 
}

inline bool DebParams::checkType(DebType type) const
{ 
	return DebHasFlag(s_type_flags, type); 
}

/*------------------------------------------------------------------
 *  class DebProxy inline functions
 *------------------------------------------------------------------*/

inline DebProxy::DebProxy()
	: m_lock(NULL)
{
}

inline DebProxy::DebProxy(DebObj *deb_obj, DebType type, ConstStr funct_name,
			  ConstStr file_name, int line_nr)
{
#ifdef NO_LIMA_DEBUG
	DebParams::checkInit();
#endif

	AutoMutex lock(*DebParams::s_mutex);

	DebObj::heading(type, funct_name, file_name, line_nr, deb_obj);

	m_lock = new AutoMutex(lock);
}

inline DebProxy::DebProxy(const DebProxy& p)
	: m_lock(p.m_lock)
{
	p.m_lock = NULL;
}

inline DebProxy::~DebProxy()
{
	if (!m_lock)
		return;

	*DebParams::s_deb_stream << std::endl;
	delete m_lock;
}

inline bool DebProxy::isActive() const
{
	return !!m_lock;
}

template <class T>
inline const DebProxy& DebProxy::operator <<(const T& o) const
{
	if (isActive()) 
		*DebParams::s_deb_stream << o;
	return *this;
}


/*------------------------------------------------------------------
 *  class DebObj inline functions
 *------------------------------------------------------------------*/

inline DebObj::DebObj(DebParams& deb_params, bool destructor, 
		      ConstStr funct_name, ConstStr obj_name, 
		      ConstStr file_name, int line_nr)
	: m_deb_params(&deb_params), m_destructor(destructor), 
	  m_funct_name(funct_name), m_obj_name(obj_name), 
	  m_file_name(file_name), m_line_nr(line_nr)
{
	getThreadData()->indent++;
	write(DebTypeFunct, m_file_name, m_line_nr) << "Enter";
}

inline DebObj::~DebObj()
{
	write(DebTypeFunct, m_file_name, m_line_nr) << "Exit";
	getThreadData()->indent--;
}

inline bool DebObj::checkOut(DebType type)
{
	return ((type == DebTypeAlways) || (type == DebTypeFatal) || 
		((type == DebTypeError) && 
		 m_deb_params->checkType(DebTypeError)) ||
		(m_deb_params->checkModule() && 
		 m_deb_params->checkType(type)));
}

inline bool DebObj::checkErr(DebType type)
{
	return ((type == DebTypeFatal) || (type == DebTypeError));
}

inline bool DebObj::checkAny(DebType type)
{
	return checkOut(type) || checkErr(type);
}

inline DebParams& DebObj::getDebParams()
{
	return *m_deb_params;
}

inline DebProxy DebObj::write(DebType type, ConstStr file_name, int line_nr)
{
	if (checkAny(type))
		return DebProxy(this, type, m_funct_name, file_name, line_nr);
	else
		return DebProxy();
}

} // namespace lima

/*------------------------------------------------------------------
 *  debug macros
 *------------------------------------------------------------------*/

#define DEB_NOP()		do {} while (0)

#define DEB_GLOBAL_NAMESPC(mod, name_space)				\
	inline lima::DebParams& getDebParams()				\
	{								\
		static lima::DebParams *deb_params = NULL;		\
		EXEC_ONCE(deb_params = new lima::DebParams(lima::mod, NULL,\
							   name_space)); \
		return *deb_params;					\
	}

#define DEB_GLOBAL(mod)							\
	DEB_GLOBAL_NAMESPC(mod, NULL)

#define DEB_STRUCT_NAMESPC(mod, struct_name, name_space)		\
	static lima::DebParams& getDebParams()				\
	{								\
		static lima::DebParams *deb_params = NULL;		\
		EXEC_ONCE(deb_params = new lima::DebParams(lima::mod,	\
							   struct_name, \
							   name_space)); \
		return *deb_params;					\
	}								\
									\
	void setDebObjName(const std::string& obj_name)			\
	{								\
		m_deb_obj_name = obj_name;				\
	}								\
									\
	lima::ConstStr getDebObjName() const				\
	{								\
		if (m_deb_obj_name.empty())				\
			return NULL;					\
		return m_deb_obj_name.c_str();				\
	}								\
									\
	std::string m_deb_obj_name

#define DEB_CLASS_NAMESPC(mod, class_name, name_space)			\
  private:								\
	DEB_STRUCT_NAMESPC(mod, class_name, name_space)

#define DEB_CLASS(mod, class_name)					\
	DEB_CLASS_NAMESPC(mod, class_name, NULL)

#ifndef NO_LIMA_DEBUG

#define DEB_GLOBAL_FUNCT()						\
	lima::DebObj deb(getDebParams(), false, __FUNCTION__,		\
		   NULL, __FILE__, __LINE__)

#define DEB_CONSTRUCTOR()						\
	DEB_MEMBER_FUNCT()

#define DEB_DESTRUCTOR()						\
	lima::DebObj deb(getDebParams(), true, __FUNCTION__,		\
		   getDebObjName(), __FILE__, __LINE__)

#define DEB_MEMBER_FUNCT()						\
	lima::DebObj deb(getDebParams(), false, __FUNCTION__,		\
		   getDebObjName(), __FILE__, __LINE__)

#define DEB_PTR()							\
	(&deb)

#define DEB_FROM_PTR(deb_ptr)						\
	lima::DebObj& deb = *(deb_ptr)

#define DEB_STATIC_FUNCT()						\
	DEB_GLOBAL_FUNCT()

#define DEB_SET_OBJ_NAME(n)						\
	setDebObjName(n)


#define DEB_MSG(type)		deb.write(type, __FILE__, __LINE__)

#define DEB_FATAL()		DEB_MSG(lima::DebTypeFatal)
#define DEB_ERROR()		DEB_MSG(lima::DebTypeError)
#define DEB_WARNING()		DEB_MSG(lima::DebTypeWarning)
#define DEB_TRACE()		DEB_MSG(lima::DebTypeTrace)
#define DEB_PARAM()		DEB_MSG(lima::DebTypeParam)
#define DEB_RETURN()		DEB_MSG(lima::DebTypeReturn)
#define DEB_ALWAYS()		DEB_MSG(lima::DebTypeAlways)

#define DEB_OBJ_NAME(o)							\
	((o)->getDebObjName())

#define DEB_CHECK_ANY(type)	deb.checkAny(type)

#else // NO_LIMA_DEBUG

#define DEB_GLOBAL_FUNCT()	DEB_NOP()
#define DEB_CONSTRUCTOR()	DEB_NOP()
#define DEB_DESTRUCTOR()	DEB_NOP()
#define DEB_MEMBER_FUNCT()	DEB_NOP()

#define DEB_PTR()		NULL
#define DEB_FROM_PTR(deb_ptr)	DEB_NOP()
#define DEB_STATIC_FUNCT()	DEB_NOP()
#define DEB_SET_OBJ_NAME(n)	DEB_NOP()

#define DEB_OUT_MSG(type)						\
	DebProxy(NULL, type, __FUNCTION__, __FILE__, __LINE__)
#define DEB_NO_MSG()							\
	DebProxy()

#define DEB_FATAL()		DEB_OUT_MSG(lima::DebTypeFatal)
#define DEB_ERROR()		DEB_OUT_MSG(lima::DebTypeError)
#define DEB_WARNING()		DEB_OUT_MSG(lima::DebTypeWarning)
#define DEB_TRACE()		DEB_NO_MSG()
#define DEB_PARAM()		DEB_NO_MSG()
#define DEB_RETURN()		DEB_NO_MSG()
#define DEB_ALWAYS()		DEB_OUT_MSG(lima::DebTypeAlways)

#define DEB_OBJ_NAME(o)		NULL

#define DEB_IGNORED_TYPE_FLAGS						\
	(lima::DebTypeTrace | lima::DebTypeFunct | lima::DebTypeParam | \
	 lima::DebTypeReturn)

#define DEB_CHECK_ANY(type)						\
	(((type) == lima::DebTypeFatal) || ((type) == lima::DebTypeError) || \
	 ((type) == lima::DebTypeWarning) || ((type) == lima::DebTypeAlways))

#endif // NO_LIMA_DEBUG

#define DEB_HEX(x)	DebHex(x)

#define DEB_VAR1(v1)	\
	#v1 << "=" << v1
#define DEB_VAR2(v1, v2)	\
	DEB_VAR1(v1) << ", " << #v2 << "=" << v2
#define DEB_VAR3(v1, v2, v3)	\
	DEB_VAR2(v1, v2) << ", " << #v3 << "=" << v3
#define DEB_VAR4(v1, v2, v3, v4)	\
	DEB_VAR3(v1, v2, v3) << ", " << #v4 << "=" << v4
#define DEB_VAR5(v1, v2, v3, v4, v5)	\
	DEB_VAR4(v1, v2, v3, v4) << ", " << #v5 << "=" << v5
#define DEB_VAR6(v1, v2, v3, v4, v5, v6)	\
	DEB_VAR5(v1, v2, v3, v4, v5) << ", " << #v6 << "=" << v6
#define DEB_VAR7(v1, v2, v3, v4, v5, v6, v7)	\
	DEB_VAR6(v1, v2, v3, v4, v5, v6) << ", " << #v7 << "=" << v7


/*------------------------------------------------------------------
 *  DebParams checkTypeFlags
 *------------------------------------------------------------------*/

inline void lima::DebParams::checkTypeFlags(Flags& type_flags)
{
#ifdef NO_LIMA_DEBUG
	if (type_flags & DEB_IGNORED_TYPE_FLAGS) {
		DEB_WARNING() << "Ignored type flags: Trace|Funct|Param|Return";
		type_flags &= ~DEB_IGNORED_TYPE_FLAGS;
	}
#endif
}

#endif // DEBUG_H
