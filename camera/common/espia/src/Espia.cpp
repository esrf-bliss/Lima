#include "Espia.h"

using namespace lima;
using namespace std;

#define CHECK_CALL(ret)		ESPIA_CHECK_CALL(ret)

Espia::Espia(int dev_nr)
	: m_dev_nr(Invalid), m_dev(ESPIA_DEV_INVAL)
{
	open(dev_nr);
}

Espia::~Espia()
{
	close();
}

void Espia::open(int dev_nr)
{
	if (dev_nr == m_dev_nr)
		return;

	close();

	CHECK_CALL(espia_open(dev_nr, &m_dev));
	m_dev_nr = dev_nr;
}

void Espia::close()
{
	if (m_dev_nr == Invalid)
		return;

	CHECK_CALL(espia_close(m_dev));
	m_dev = ESPIA_DEV_INVAL;
	m_dev_nr = Invalid;
}


void Espia::throwError(int ret, string file, string func, int line)
{
	string err_desc = string("Espia: ") + espia_strerror(ret);
	throw Exception(Hardware, Error, err_desc, file, func, line);
}
