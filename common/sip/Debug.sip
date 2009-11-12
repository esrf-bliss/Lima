%ExportedHeaderCode
#include <Debug.h>
using namespace lima;
%End

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

	static ConstStr getTypeName(DebType type);
	static ConstStr getFormatName(DebFormat fmt);
	static ConstStr getModuleName(DebModule mod);
};

/*------------------------------------------------------------------
 *  global inline functions
 *------------------------------------------------------------------*/

DebParams::Flags DebFlag(int val);
bool DebHasFlag(DebParams::Flags flags, int val);
