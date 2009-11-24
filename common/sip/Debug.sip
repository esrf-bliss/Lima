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

%TypeCode

static ConstStr getConstStr(const std::string *s)
{
	return (!s || s->empty()) ? NULL : s->c_str();
}

class DebParamsSip : public DebParams
{
public:
	DebParamsSip(DebModule mod, std::string *cn, std::string *ns)
		: DebParams(mod, getConstStr(cn), getConstStr(ns)),
		  class_name(cn), name_space(ns)
	{}

	~DebParamsSip()
	{}

	std::string *class_name;
	std::string *name_space;
};

%End

 public:
	typedef long long Flags;
	typedef std::vector<std::string> NameList;

	static const Flags AllFlags;

	DebParams(DebModule mod = DebModNone, 
		  const std::string& class_name = "", 
		  const std::string& name_space = "");
%MethodCode
	std::string *class_name = new std::string(*a1);
	std::string *name_space = new std::string(*a2);
	sipCpp = new DebParamsSip(a0, class_name, name_space);
%End

	~DebParams();
%MethodCode
	DebParamsSip *deb_params_sip = (DebParamsSip *) sipCpp;
	std::string *class_name = deb_params_sip->class_name;
	std::string *name_space = deb_params_sip->name_space;
	delete deb_params_sip;
	delete name_space;
	delete class_name;
%End

	void setModule(DebModule mod);
%MethodCode
	DebParamsSip *deb_params_sip = (DebParamsSip *) sipCpp;
	deb_params_sip->setModule(a0);
%End

	DebModule getModule() const;
%MethodCode
	DebParamsSip *deb_params_sip = (DebParamsSip *) sipCpp;
	sipRes = deb_params_sip->getModule();
%End

	void setClassName(const std::string& class_name);
%MethodCode
	DebParamsSip *deb_params_sip = (DebParamsSip *) sipCpp;
	*deb_params_sip->class_name = *a0;
	deb_params_sip->setClassName(getConstStr(deb_params_sip->class_name));
%End

	ConstStr getClassName() const;
%MethodCode
	DebParamsSip *deb_params_sip = (DebParamsSip *) sipCpp;
	sipRes = deb_params_sip->getClassName();
%End

	void setNameSpace(const std::string& name_space);
%MethodCode
	DebParamsSip *deb_params_sip = (DebParamsSip *) sipCpp;
	*deb_params_sip->name_space = *a0;
	deb_params_sip->setNameSpace(getConstStr(deb_params_sip->name_space));
%End

	ConstStr getNameSpace() const;
%MethodCode
	DebParamsSip *deb_params_sip = (DebParamsSip *) sipCpp;
	sipRes = deb_params_sip->getNameSpace();
%End

	bool checkModule() const;
%MethodCode
	DebParamsSip *deb_params_sip = (DebParamsSip *) sipCpp;
	sipRes = deb_params_sip->checkModule();
%End

	bool checkType(DebType type) const;
%MethodCode
	DebParamsSip *deb_params_sip = (DebParamsSip *) sipCpp;
	sipRes = deb_params_sip->checkType(a0);
%End

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
using namespace lima;
%End

%TypeCode

static ConstStr getConstStr(const std::string *s)
{
	return (!s || s->empty()) ? NULL : s->c_str();
}

class DebObjSip : public DebObj
{
public:
	DebObjSip(DebParams *db, std::string *fn, std::string *on) 
		: DebObj(*db, false, getConstStr(fn), getConstStr(on)),
		  deb_params(db), funct_name(fn), obj_name(on), 
		  next_type(DebTypeAlways)
	{}

	~DebObjSip()
	{}

	DebObj *Fatal()
	{ return setNextType(DebTypeFatal); }

	DebObj *Error()
	{ return setNextType(DebTypeError); }

	DebObj *Warning()
	{ return setNextType(DebTypeWarning); }

	DebObj *Param()
	{ return setNextType(DebTypeParam); }

	DebObj *Trace()
	{ return setNextType(DebTypeTrace); }

	DebObj *Return()
	{ return setNextType(DebTypeReturn); }

	DebObj *Always()
	{ return setNextType(DebTypeAlways); }

	void write(const std::string& msg)
	{
		if (msg != "\n")
			DebObj::write(next_type) << msg;
	}

	DebObj *setNextType(DebType nt)
	{
		next_type = nt;
		return this;
	}

	DebParams *deb_params;
	std::string *funct_name;
	std::string *obj_name;
        DebType next_type;
};

%End

 public:
	DebObj(DebParams& deb_params,
	       const std::string& funct_name = "", 
	       const std::string& obj_name = "");
%MethodCode
	DebParams *deb_params   = new DebParams(*a0);
	std::string *funct_name = new std::string(*a1);
	std::string *obj_name   = new std::string(*a2);
	sipCpp = new DebObjSip(deb_params, funct_name, obj_name);
%End

	~DebObj();
%MethodCode
	DebObjSip *deb_obj_sip = (DebObjSip *) sipCpp;
	DebParams *deb_params   = deb_obj_sip->deb_params;
	std::string *funct_name = deb_obj_sip->funct_name;
	std::string *obj_name   = deb_obj_sip->obj_name;
	delete deb_obj_sip;
	delete obj_name;
	delete funct_name;
	delete deb_params;
%End

	DebObj& Fatal();
%MethodCode
	DebObjSip *deb_obj_sip = (DebObjSip *) sipCpp;
	sipRes = deb_obj_sip->Fatal();
%End

	DebObj& Error();
%MethodCode
	DebObjSip *deb_obj_sip = (DebObjSip *) sipCpp;
	sipRes = deb_obj_sip->Fatal();
%End

	DebObj& Warning();
%MethodCode
	DebObjSip *deb_obj_sip = (DebObjSip *) sipCpp;
	sipRes = deb_obj_sip->Warning();
%End

	DebObj& Param();
%MethodCode
	DebObjSip *deb_obj_sip = (DebObjSip *) sipCpp;
	sipRes = deb_obj_sip->Param();
%End

	DebObj& Trace();
%MethodCode
	DebObjSip *deb_obj_sip = (DebObjSip *) sipCpp;
	sipRes = deb_obj_sip->Trace();
%End

	DebObj& Return();
%MethodCode
	DebObjSip *deb_obj_sip = (DebObjSip *) sipCpp;
	sipRes = deb_obj_sip->Return();
%End

	DebObj& Always();
%MethodCode
	DebObjSip *deb_obj_sip = (DebObjSip *) sipCpp;
	sipRes = deb_obj_sip->Always();
%End

	void write(const std::string& msg);
%MethodCode
	DebObjSip *deb_obj_sip = (DebObjSip *) sipCpp;
	deb_obj_sip->write(*a0);	
%End
};
