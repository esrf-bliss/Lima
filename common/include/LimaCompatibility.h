#ifndef LIMACOMPATIBILITY_H
#define LIMACOMPATIBILITY_H

#ifdef WIN32
#ifdef LIMACORE_EXPORTS
#define LIMACORE_API __declspec(dllexport)
#else
#define LIMACORE_API __declspec(dllimport)
#endif
#else  /* Unix */
#define LIMACORE_API
#endif

#endif
