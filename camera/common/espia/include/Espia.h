#ifndef ESPIA_H
#define ESPIA_H

#include "espia_lib.h"
#include "Exceptions.h"

#include <string>

namespace lima
{

class Espia
{
 public:
	enum {
		Invalid = -1,
		NoBlock = 0,
		BlockForever = -1,
		MetaDev = SCDXIPCI_META_DEV,
	};

	unsigned long sec2usec(double sec);
	double usec2sec(unsigned long usec);

	static void throwError(int ret, std::string file, std::string func, 
			       int line);

};

#define ESPIA_CHECK_CALL(ret)						\
	do {								\
		int aux_ret = (ret);					\
		if (aux_ret < 0)					\
			Espia::throwError(aux_ret, __FILE__,		\
					  __FUNCTION__, __LINE__);	\
	} while (0)


inline unsigned long Espia::sec2usec(double sec)
{
	return (unsigned long) (sec * 1e6);
}

inline double Espia::usec2sec(unsigned long  usec)
{
	return usec * 1e-6;
}


} // namespace lima


#endif // ESPIA_H
