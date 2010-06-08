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
	DebFmtThread,		// = 1 << 1,
	DebFmtModule,		// = 1 << 2,
	DebFmtObj,		// = 1 << 3,
	DebFmtFunct,		// = 1 << 4,	
	DebFmtFileLine,		// = 1 << 5,
	DebFmtType,		// = 1 << 6,
	DebFmtIndent,		// = 1 << 7,
	DebFmtColor,		// = 1 << 8,
};

enum DebModule {
	DebModNone,		// = 1 << 0,
	DebModCommon,		// = 1 << 1,
	DebModHardware,		// = 1 << 2,
	DebModHardwareSerial,	// = 1 << 3,
	DebModControl,		// = 1 << 4,
	DebModEspia,		// = 1 << 5,
	DebModEspiaSerial,	// = 1 << 6,
	DebModFocla,		// = 1 << 7,
	DebModCamera,		// = 1 << 8,
	DebModCameraCom,	// = 1 << 9,
	DebModTest,		// = 1 << 10,
	DebModApplication,	// = 1 << 11,
};

typedef const char *ConstStr;


/*------------------------------------------------------------------
 *  class DebParams 
 *------------------------------------------------------------------*/

class DebParams
{
%TypeHeaderCode
#include <Debug.h>
#include <AutoObj.h>
using namespace lima;
%End

%TypeCode

static ConstStr getConstStr(const std::string *s)
{
	return (!s || s->empty()) ? NULL : s->c_str();
}

class DebParamsData
{
public:
	DebParamsData(std::string *cn, std::string *ns)
		: class_name(cn), name_space(ns)
	{}

	~DebParamsData()
	{
		delete class_name;
		delete name_space;
	}

	std::string *class_name;
	std::string *name_space;
};

static std::map<DebParams *, DebParamsData *> deb_params_data_map;

%End

 public:
	typedef long long Flags;
	typedef std::vector<std::string> NameList;

	static const Flags AllFlags;

	DebParams(DebModule mod = DebModNone, 
		  const std::string& class_name = "", 
		  const std::string& name_space = "");
%MethodCode
	std::string *cn = new std::string(*a1);
	std::string *ns = new std::string(*a2);
	sipCpp = new DebParams(a0, getConstStr(cn), getConstStr(ns));
	deb_params_data_map[sipCpp] = new DebParamsData(cn, ns);
%End

	~DebParams();
%MethodCode
	AutoPtr<DebParamsData> dpd = deb_params_data_map[sipCpp];
	deb_params_data_map.erase(sipCpp);
%End

	void setModule(DebModule mod);
	DebModule getModule() const;

	void setClassName(const std::string& class_name);
%MethodCode
	std::string *cn = deb_params_data_map[sipCpp]->class_name;
	*cn = *a0;
	sipCpp->setClassName(getConstStr(cn));
%End

	ConstStr getClassName() const;

	void setNameSpace(const std::string& name_space);
%MethodCode
	std::string *ns = deb_params_data_map[sipCpp]->name_space;
	*ns = *a0;
	sipCpp->setClassName(getConstStr(ns));
%End

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

/*------------------------------------------------------------------
 *  class DebObj
 *------------------------------------------------------------------*/

class DebObj
{
%TypeHeaderCode
#include <Debug.h>
#include <AutoObj.h>
using namespace lima;
%End

%TypeCode

static ConstStr getConstStr(const std::string *s)
{
	return (!s || s->empty()) ? NULL : s->c_str();
}

class DebObjData
{
public:
	DebObjData(DebParams *db, std::string *fn, std::string *on,
		   std::string *fin)
		: deb_params(db), funct_name(fn), obj_name(on), file_name(fin),
  		  next_type(DebTypeAlways)
	{}

	~DebObjData()
	{
		delete deb_params;
		delete funct_name;
		delete obj_name;
		delete file_name;
	}

	void setNextType(DebType nt)
	{
		next_type = nt;
	}

	DebType getNextType()
	{
		return next_type;
	}

	DebParams *deb_params;
	std::string *funct_name;
	std::string *obj_name;
	std::string *file_name;
        DebType next_type;
};

static std::map<DebObj *, DebObjData *> deb_obj_data_map;

static DebObj* setNextType(DebObj *deb_obj, DebType next_type)
{
	deb_obj_data_map[deb_obj]->setNextType(next_type);
	return deb_obj;
}

static void write(DebObj *deb_obj, const std::string *msg)
{
	if (*msg != "\n") {
		DebType next_type = deb_obj_data_map[deb_obj]->getNextType();
		deb_obj->write(next_type) << *msg;
	}
}

%End

 public:
	DebObj(DebParams& deb_params,
	       const std::string& funct_name = "", 
	       const std::string& obj_name = "",
	       const std::string& file_name = "", int line_nr = 0);
%MethodCode
	DebParams *dp    = new DebParams(*a0);
	std::string *fn  = new std::string(*a1);
	std::string *on  = new std::string(*a2);
	std::string *fin = new std::string(*a3);
	sipCpp = new DebObj(*dp, false, getConstStr(fn), getConstStr(on), 
			    getConstStr(fin), a4);
	deb_obj_data_map[sipCpp] = new DebObjData(dp, fn, on, fin);
%End

	~DebObj();
%MethodCode
	AutoPtr<DebObjData> dod = deb_obj_data_map[sipCpp];
	deb_obj_data_map.erase(sipCpp);
%End

	DebObj& Fatal();
%MethodCode
	sipRes = setNextType(sipCpp, DebTypeFatal);
%End

	DebObj& Error();
%MethodCode
	sipRes = setNextType(sipCpp, DebTypeError);
%End

	DebObj& Warning();
%MethodCode
	sipRes = setNextType(sipCpp, DebTypeWarning);
%End

	DebObj& Param();
%MethodCode
	sipRes = setNextType(sipCpp, DebTypeParam);
%End

	DebObj& Trace();
%MethodCode
	sipRes = setNextType(sipCpp, DebTypeTrace);
%End

	DebObj& Return();
%MethodCode
	sipRes = setNextType(sipCpp, DebTypeReturn);
%End

	DebObj& Always();
%MethodCode
	sipRes = setNextType(sipCpp, DebTypeAlways);
%End

	void write(const std::string& msg);
%MethodCode
	write(sipCpp, a0);
%End

	void __lshift__(const std::string& msg);
%MethodCode
	write(a0, a1);
%End

	void Fatal(const std::string& msg);
%MethodCode
	setNextType(sipCpp, DebTypeFatal);
	write(sipCpp, a0);
%End

	void Error(const std::string& msg);
%MethodCode
	setNextType(sipCpp, DebTypeError);
	write(sipCpp, a0);
%End

	void Warning(const std::string& msg);
%MethodCode
	setNextType(sipCpp, DebTypeWarning);
	write(sipCpp, a0);
%End

	void Param(const std::string& msg);
%MethodCode
	setNextType(sipCpp, DebTypeParam);
	write(sipCpp, a0);
%End

	void Trace(const std::string& msg);
%MethodCode
	setNextType(sipCpp, DebTypeTrace);
	write(sipCpp, a0);
%End

	void Return(const std::string& msg);
%MethodCode
	setNextType(sipCpp, DebTypeReturn);
	write(sipCpp, a0);
%End

	void Always(const std::string& msg);
%MethodCode
	setNextType(sipCpp, DebTypeAlways);
	write(sipCpp, a0);
%End

};
