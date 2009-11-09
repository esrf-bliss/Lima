// This code was taken from the Debug.h file (Generig debug in C++)
// in: /segfs/dserver/classes++/extdevice/

#ifndef DEBUG_H
#define DEBUG_H

#include "StreamUtils.h"
#include "ThreadUtils.h"

#include <string>
#include <map>

namespace lima
{

// Do not forget to update the corresponding Type/Format/Module name in
// DebParams::checkInit (Debug.cpp) when you change these enums

enum DebType {
	DebFatal,	DebError,	DebWarning,	DebTrace, 
	DebFunct,	DebParam,	DebReturn,	DebAlways,
};

enum DebFormat {
	DebFmtDateTime,	DebFmtModule,	DebFmtObj,	DebFmtFunct,	
	DebFmtFileLine,	DebFmtType,
};

enum DebModule {
	DebModNone,	DebModCommon,	DebModHardware,	DebModControl,	
	DebModSimu,	DebModEspia,	DebModEspiaSerial, DebModFocla,	
	DebModFrelon,	DebModFrelonSerial, DebModMaxipix,
};

typedef const char *ConstStr;

/*------------------------------------------------------------------
 *  class DebStream
 *------------------------------------------------------------------*/

class DebStream : public std::ostream
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

class DebParams
{
 public:
	typedef long long Flags;

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

	static void setModuleFlags(Flags mod_flags);
	static Flags getModuleFlags();

	static void setFormatFlags(Flags fmt_flags);
	static Flags getFormatFlags();

	static void setTypeFlags(Flags type_flags);
	static Flags getTypeFlags();

	static DebStream& getDebStream();
	static AutoMutex lock();

	static ConstStr getTypeName(DebType type);
	static ConstStr getFormatName(DebFormat fmt);
	static ConstStr getModuleName(DebModule mod);

private:
	friend class DebProxy;
	friend class DebObj;

	static void checkInit();

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


/*------------------------------------------------------------------
 *  class DebProxy
 *------------------------------------------------------------------*/

class DebProxy
{
 public:
	DebProxy();
	DebProxy(DebObj *deb_obj, DebType type, ConstStr file_name, 
		 int line_nr);
	DebProxy(const DebProxy& p); // should not be called
	~DebProxy();

	template <class T> 
        const DebProxy& operator <<(const T& o) const;

	bool isActive() const;

 private:
	AutoMutex *m_lock;
};


/*------------------------------------------------------------------
 *  class DebObj
 *------------------------------------------------------------------*/

class DebObj
{
 public:
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

	void heading(DebType, ConstStr file_name, int line_nr);

	DebParams *m_deb_params;
	bool m_destructor;
	ConstStr m_funct_name;
	ConstStr m_obj_name;
	ConstStr m_file_name;
	int m_line_nr;
};


/*------------------------------------------------------------------
 *  global inline functions
 *------------------------------------------------------------------*/

inline DebParams::Flags DebFlag(int val)
{
	return (1LL << val);
}

inline bool DebHasFlag(DebParams::Flags flags, int val)
{
	return ((flags & DebFlag(val)) != 0);
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

inline DebProxy::DebProxy(DebObj *deb_obj, DebType type, ConstStr file_name, 
			  int line_nr)
{
	AutoMutex lock(*DebParams::s_mutex);

	deb_obj->heading(type, file_name, line_nr);

	m_lock = new AutoMutex(lock);
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
	return m_lock;
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
	write(DebFunct, m_file_name, m_line_nr) << "Enter";
}

inline DebObj::~DebObj()
{
	write(DebFunct, m_file_name, m_line_nr) << "Exit";
}

inline bool DebObj::checkOut(DebType type)
{
	return ((type == DebAlways) || (type == DebFatal) || 
		((type == DebError) && m_deb_params->checkType(DebError)) ||
		(m_deb_params->checkModule() && 
		 m_deb_params->checkType(type)));
}

inline bool DebObj::checkErr(DebType type)
{
	return ((type == DebFatal) || (type == DebError));
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
		return DebProxy(this, type, file_name, line_nr);
	else
		return DebProxy();
}

/*------------------------------------------------------------------
 *  debug macros
 *------------------------------------------------------------------*/

#define DEB_CLASS_NAMESPC(mod, class_name, name_space)			\
  private:								\
	static DebParams& getDebParams()				\
	{								\
		static DebParams *deb_params = NULL;			\
		if (!deb_params)					\
			deb_params = new DebParams(mod, class_name,	\
						   name_space);		\
		return *deb_params;					\
	}								\
									\
	void setDebObjName(const std::string& obj_name)			\
	{								\
		m_deb_obj_name = obj_name;				\
	}								\
									\
	ConstStr getDebObjName() const					\
	{								\
		if (m_deb_obj_name.empty())				\
			return NULL;					\
		return m_deb_obj_name.c_str();				\
	}								\
									\
	std::string m_deb_obj_name


#define DEB_MEMBER_FUNCT()						\
	DebObj deb(getDebParams(), false, __FUNCTION__,			\
		   getDebObjName(), __FILE__, __LINE__)

#define DEB_STATIC_FUNCT()						\
	DebObj deb(getDebParams(), false, __FUNCTION__,			\
		   NULL, __FILE__, __LINE__)

#define DEB_CONSTRUCTOR()						\
	DEB_MEMBER_FUNCT()

#define DEB_DESTRUCTOR()						\
	DebObj deb(getDebParams(), true, __FUNCTION__,			\
		   getDebObjName(), __FILE__, __LINE__)

#define DEB_SET_OBJ_NAME(n) \
	setDebObjName(n)

#define DEB_MSG(type)	deb.write(type, __FILE__, __LINE__)

#define DEB_MSG_VAR1(type, v1) \
	DEB_MSG(type) << #v1 "=" << v1

#define DEB_MSG_VAR2(type, v1, v2) \
	DEB_MSG_VAR1(type, v1) << ", " #v2 "=" << v2

#define DEB_MSG_VAR3(type, v1, v2, v3) \
	DEB_MSG_VAR2(type, v1, v2) << ", " #v3 "=" << v3

#define DEB_MSG_VAR4(type, v1, v2, v3, v4) \
	DEB_MSG_VAR3(type, v1, v2, v3) << ", " #v4 "=" << v4

#define DEB_MSG_VAR5(type, v1, v2, v3, v4, v5) \
	DEB_MSG_VAR4(type, v1, v2, v3, v4) << ", " #v5 "=" << v5

#define DEB_MSG_VAR6(type, v1, v2, v3, v4, v5, v6) \
	DEB_MSG_VAR5(type, v1, v2, v3, v4, v5) << ", " #v6 "=" << v6

#define DEB_FATAL()	DEB_MSG(DebFatal)
#define DEB_ERROR()	DEB_MSG(DebError)
#define DEB_WARNING()	DEB_MSG(DebWarning)
#define DEB_TRACE()	DEB_MSG(DebTrace)
#define DEB_PARAM()	DEB_MSG(DebParam)
#define DEB_RETURN()	DEB_MSG(DebReturn)
#define DEB_ALWAYS()	DEB_MSG(DebAlways)

#define DEB_PARAM_VAR1(v1) \
	DEB_MSG_VAR1(DebParam, v1)
#define DEB_PARAM_VAR2(v1, v2) \
	DEB_MSG_VAR2(DebParam, v1, v2)
#define DEB_PARAM_VAR3(v1, v2, v3) \
	DEB_MSG_VAR3(DebParam, v1, v2, v3)
#define DEB_PARAM_VAR4(v1, v2, v3, v4) \
	DEB_MSG_VAR4(DebParam, v1, v2, v3, v4)
#define DEB_PARAM_VAR5(v1, v2, v3, v4, v5) \
	DEB_MSG_VAR5(DebParam, v1, v2, v3, v4, v5)
#define DEB_PARAM_VAR6(v1, v2, v3, v4, v5, v6) \
	DEB_MSG_VAR6(DebParam, v1, v2, v3, v4, v5, v6)

#define DEB_TRACE_VAR1(v1) \
	DEB_MSG_VAR1(DebTrace, v1)
#define DEB_TRACE_VAR2(v1, v2) \
	DEB_MSG_VAR2(DebTrace, v1, v2)
#define DEB_TRACE_VAR3(v1, v2, v3) \
	DEB_MSG_VAR3(DebTrace, v1, v2, v3)
#define DEB_TRACE_VAR4(v1, v2, v3, v4) \
	DEB_MSG_VAR4(DebTrace, v1, v2, v3, v4)
#define DEB_TRACE_VAR5(v1, v2, v3, v4, v5) \
	DEB_MSG_VAR5(DebTrace, v1, v2, v3, v4, v5)
#define DEB_TRACE_VAR6(v1, v2, v3, v4, v5, v6) \
	DEB_MSG_VAR6(DebTrace, v1, v2, v3, v4, v5, v6)

#define DEB_RETURN_VAR1(v1) \
	DEB_MSG_VAR1(DebReturn, v1)
#define DEB_RETURN_VAR2(v1, v2) \
	DEB_MSG_VAR2(DebReturn, v1, v2)
#define DEB_RETURN_VAR3(v1, v2, v3) \
	DEB_MSG_VAR3(DebReturn, v1, v2, v3)
#define DEB_RETURN_VAR4(v1, v2, v3, v4) \
	DEB_MSG_VAR4(DebReturn, v1, v2, v3, v4)
#define DEB_RETURN_VAR5(v1, v2, v3, v4, v5) \
	DEB_MSG_VAR5(DebReturn, v1, v2, v3, v4, v5)
#define DEB_RETURN_VAR6(v1, v2, v3, v4, v5, v6) \
	DEB_MSG_VAR6(DebReturn, v1, v2, v3, v4, v5, v6)

} // namespace lima

#endif // DEBUG_H
