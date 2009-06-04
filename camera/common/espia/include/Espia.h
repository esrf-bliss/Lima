#ifndef ESPIA_H
#define ESPIA_H

#include "Exceptions.h"
#include "espia_lib.h"
#include <string>

namespace lima
{

class Espia
{
 public:
	enum {
		Invalid = -1,
	};

	Espia(int dev_nr);
	~Espia();

	void serWrite(const std::string& buffer, 
		      int block_size = 0, double block_delay = 0, 
		      bool no_wait = false);
	void serRead(std::string& buffer, int& len, double timeout);
	void serReadStr(std::string& buffer, int& len, 
			const std::string& term, double timeout);

	static void throwError(int ret, std::string file, std::string func, 
			       int line);

 private:
	void open(int dev_nr);
	void close();
	
	int m_dev_nr;
	espia_t m_dev;

};

#define ESPIA_CHECK_CALL(ret)						\
	do {								\
		int aux_ret = (ret);					\
		if (aux_ret < 0)					\
			Espia::throwError(aux_ret, __FILE__,		\
					     __FUNCTION__, __LINE__);	\
	} while (0)


} // namespace lima

#endif // ESPIA_H
