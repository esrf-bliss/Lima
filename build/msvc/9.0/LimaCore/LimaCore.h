// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIMACORE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIMACORE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LIMACORE_EXPORTS
#define LIMACORE_API __declspec(dllexport)
#else
#define LIMACORE_API __declspec(dllimport)
#endif

// This class is exported from the LimaCore.dll
class LIMACORE_API CLimaCore {
public:
	CLimaCore(void);
	// TODO: add your methods here.
};

extern LIMACORE_API int nLimaCore;

LIMACORE_API int fnLimaCore(void);
