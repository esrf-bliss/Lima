#ifndef SIMUCOMPATIBILITY_H
#define SIMUCOMPATIBILITY_H

#ifdef WIN32
#pragma warning (disable:4290)
#ifdef LIBSIMULATOR_EXPORTS
#define LIBSIMULATOR_API __declspec(dllexport)
#else
#define LIBSIMULATOR_API __declspec(dllimport)
#endif
#else  /* Unix */
#define LIBSIMULATOR_API
#endif

#endif
