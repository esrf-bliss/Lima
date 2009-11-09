#include "Debug.h"
#include "MiscUtils.h"

#include <ctime>
#include <iomanip>
#include <unistd.h>
#include <sys/time.h>

using namespace lima;
using namespace std;


/*------------------------------------------------------------------
 *  DebStream functions 
 *------------------------------------------------------------------*/

DebStream::DebStream() 
	: ostream(&m_all_buf), m_out_streams(&m_out_list), 
	  m_err_streams(&m_err_list), m_all_buf(&m_all_list)
{
	m_out_list.push_back(&cout);
	m_all_list.push_back(&m_out_streams);
	m_all_list.push_back(&m_err_streams);
	
	m_buffers[None]   = &m_null_buf;
	m_buffers[Output] = m_out_streams.rdbuf();
	m_buffers[Error]  = m_err_streams.rdbuf();
	m_buffers[Both]   = &m_all_buf;
	
	SetStream(Output);
}

bool DebStream::Find(ostream *os, StreamList& slist, 
		       ListIterator& it)
{
	ListIterator end = slist.end();
	for (it = slist.begin(); it < end; ++it)
		if (*it == os)
			return true;
	
	return false;
}

bool DebStream::FindStream(ostream *os, Selector buffer)
{
	ListIterator it;
	
	switch (buffer) {
	case Output:
		return Find(os, m_out_list, it);
	case Error:
		return Find(os, m_err_list, it);
	case Both:
		return FindStream(os, Output) || FindStream(os, Error);
	default:
		return 0;
	}
}

void DebStream::AddOutput(ostream *os)
{
	if (!FindStream(os, Output))
		m_out_list.push_back(os);
}

void DebStream::RemoveOutput(ostream *os)
{
	ListIterator it;
	if (Find(os, m_out_list, it))
		m_out_list.erase(it);
}

void DebStream::AddError(ostream *os)
{
	if (!FindStream(os, Error))
		m_err_list.push_back(os);
}

void DebStream::RemoveError(ostream *os)
{
	ListIterator it;
	if (Find(os, m_err_list, it))
		m_err_list.erase(it);
}


/*------------------------------------------------------------------
 *  DebParams functions 
 *------------------------------------------------------------------*/

DebParams::Flags DebParams::s_type_flags;
DebParams::Flags DebParams::s_fmt_flags;
DebParams::Flags DebParams::s_mod_flags;

DebStream *DebParams::s_deb_stream = NULL;

map<DebType,   string> *DebParams::s_type_name_map = NULL;
map<DebFormat, string> *DebParams::s_fmt_name_map  = NULL;
map<DebModule, string> *DebParams::s_mod_name_map  = NULL;

Mutex *DebParams::s_mutex = NULL;

void DebParams::setTypeFlags(Flags type_flags)
{ 
	checkInit();
	s_type_flags = type_flags; 
}

DebParams::Flags DebParams::getTypeFlags()
{ 
	checkInit();
	return s_type_flags; 
}

void DebParams::setFormatFlags(Flags fmt_flags)
{ 
	checkInit();
	s_fmt_flags = fmt_flags; 
}

DebParams::Flags DebParams::getFormatFlags()
{ 
	checkInit();
	return s_fmt_flags; 
}

void DebParams::setModuleFlags(Flags mod_flags)
{
	checkInit();
	s_mod_flags = mod_flags; 
}

DebParams::Flags DebParams::getModuleFlags()
{ 
	checkInit();
	return s_mod_flags; 
}

DebStream& DebParams::getDebStream()
{ 
	checkInit();
	return *s_deb_stream; 
}

AutoMutex DebParams::lock()
{
	checkInit();
	return AutoMutex(*s_mutex);
}

ConstStr DebParams::getTypeName(DebType type)
{
	checkInit();

	string name = (*s_type_name_map)[type];
	if (name.empty())
		name = "Unknown";
	return name.c_str();
}

ConstStr DebParams::getFormatName(DebFormat fmt)
{
	checkInit();

	string name = (*s_fmt_name_map)[fmt];
	if (name.empty())
		name = "Unknown";
	return name.c_str();
}

ConstStr DebParams::getModuleName(DebModule mod)
{
	checkInit();

	string name = (*s_mod_name_map)[mod];
	if (name.empty())
		name = "Unknown";
	return name.c_str();
}

void DebParams::checkInit()
{
	if (s_deb_stream != NULL)
		return;

	s_type_flags = s_fmt_flags = s_mod_flags = 0xffffffff;

	s_deb_stream = new DebStream();

	typedef pair<DebType, string> TypeNamePair;
#define TYPE_NAME(x) TypeNamePair(Deb##x, #x)
	TypeNamePair type_names[] = {
		TYPE_NAME(Fatal),
		TYPE_NAME(Error),
		TYPE_NAME(Warning),
		TYPE_NAME(Trace),
		TYPE_NAME(Funct),
		TYPE_NAME(Param),
		TYPE_NAME(Return),
		TYPE_NAME(Always),
	};
	s_type_name_map = new map<DebType, string>(C_LIST_ITERS(type_names));

	typedef pair<DebFormat, string> FormatNamePair;
#define FORMAT_NAME(x) FormatNamePair(DebFmt##x, #x)
	FormatNamePair fmt_names[] = {
		FORMAT_NAME(DateTime),
		FORMAT_NAME(Module),
		FORMAT_NAME(Obj),
		FORMAT_NAME(Funct),
		FORMAT_NAME(FileLine),
		FORMAT_NAME(Type),
	};
	s_fmt_name_map = new map<DebFormat, string>(C_LIST_ITERS(fmt_names));

	typedef pair<DebModule, string> ModuleNamePair;
#define MODULE_NAME(x) ModuleNamePair(DebMod##x, #x)
	ModuleNamePair mod_names[] = {
		MODULE_NAME(None),
		MODULE_NAME(Common),
		MODULE_NAME(Hardware),
		MODULE_NAME(Control),
		MODULE_NAME(Simu),
		MODULE_NAME(Espia),
		MODULE_NAME(EspiaSerial),
		MODULE_NAME(Focla),
		MODULE_NAME(Frelon),
		MODULE_NAME(FrelonSerial),
		MODULE_NAME(Maxipix),
	};
	s_mod_name_map = new map<DebModule, string>(C_LIST_ITERS(mod_names));

	s_mutex = new Mutex();
}


/*------------------------------------------------------------------
 *  DebObj functions 
 *------------------------------------------------------------------*/

void DebObj::heading(DebType type, ConstStr file_name, int line_nr)
{
	std::ostream& os = *DebParams::s_deb_stream;
	DebParams::Flags& flags = DebParams::s_fmt_flags;

	ConstStr m, sep = "";

	if (DebHasFlag(flags, DebFmtDateTime)) {
		struct timeval tod;
		gettimeofday(&tod, NULL);

		time_t raw_time = tod.tv_sec;
		struct tm *tm_info = localtime(&raw_time);

		char buffer[256];
		strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", tm_info);

		os << sep << "[" << buffer << "." 
		   << setw(6) << setfill('0') << tod.tv_usec 
		   << setw(0) << setfill(' ') << "]";
		sep = " ";
	}

	if (DebHasFlag(flags, DebFmtModule)) {
		os << "*" << DebParams::getModuleName(m_deb_params->m_mod)
		   << "*";
		sep = "";
	}

	if (DebHasFlag(flags, DebFmtObj) && ((m = m_obj_name))) {
		os << sep << m;
		sep = " ";
	}

	if (DebHasFlag(flags, DebFmtFunct)) {
		ConstStr csep = sep;
		if ((m = m_deb_params->m_name_space)) {
			os << csep << m;
			csep = "::";
		}
		if ((m = m_deb_params->m_class_name)) {
			os << csep << m;
			csep = "::";
		}
		if ((m = m_funct_name)) {
			ConstStr destruct = m_destructor ? "~" : "";
			os << csep << destruct << m;
			csep = "::";
		}
		if (string(csep) == "::")
			sep = " ";
	}

	if (DebHasFlag(flags, DebFmtFileLine) && ((m = file_name))) {
		os << sep << "(" << m << ":" << line_nr << ")";
		sep = " ";
	}

	if (DebHasFlag(flags, DebFmtType)) {
		if (strlen(sep) > 0)
			sep = "-";
		os << sep << DebParams::getTypeName(type);
		sep = " ";
	}

	if (strlen(sep) > 0)
		os << ": ";
}

