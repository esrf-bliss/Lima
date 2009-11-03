
enum Layer {
	Common, Control, Hardware,
};

enum ErrorType {
	InvalidValue, NotSupported, Error,
};


%Exception Exception(SIP_Exception) /Default/
{
%TypeHeaderCode
#include <Exceptions.h>
using namespace lima;
%End
%RaiseCode
        const char *detail = sipExceptionRef.getErrMsg().c_str();
	PyErr_SetString(sipException_Exception, detail);
%End
};

