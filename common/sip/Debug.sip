%ExportedHeaderCode
#include <Debug.h>
using namespace lima;
%End

enum DebType {
	DebTypeFatal,		// = 1 << 0,
	DebTypeError,		// = 1 << 1,
	DebTypeWarning,		// = 1 << 2,
	DebTypeTrace,		// = 1 << 3,
	DebTypeFunct,		// = 1 << 4,
	DebTypeParam,		// = 1 << 5,
	DebTypeReturn,		// = 1 << 6,
	DebTypeAlways,		// = 1 << 7,
};

enum DebFormat {
	DebFmtDateTime,		// = 1 << 0,
	DebFmtModule,		// = 1 << 1,
	DebFmtObj,		// = 1 << 2,
	DebFmtFunct,		// = 1 << 3,	
	DebFmtFileLine,		// = 1 << 4,
	DebFmtType,		// = 1 << 5,
};

enum DebModule {
	DebModNone,		// = 1 << 0,
	DebModCommon,		// = 1 << 1,
	DebModHardware,		// = 1 << 2,
	DebModControl,		// = 1 << 3,
	DebModEspia,		// = 1 << 4,
	DebModEspiaSerial,	// = 1 << 5,
	DebModFocla,		// = 1 << 6,
	DebModCamera,		// = 1 << 7,
	DebModCameraCom,	// = 1 << 8,
};

typedef const char *ConstStr;


/*------------------------------------------------------------------
 *  class DebParams 
 *------------------------------------------------------------------*/

class DebParams
{
%TypeHeaderCode
#include <Debug.h>
using namespace lima;
%End
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

	static ConstStr getTypeName(DebType type);
	static ConstStr getFormatName(DebFormat fmt);
	static ConstStr getModuleName(DebModule mod);

};

/*------------------------------------------------------------------
 *  global inline functions
 *------------------------------------------------------------------*/

bool DebHasFlag(DebParams::Flags flags, int val);
