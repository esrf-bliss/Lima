#ifndef ACQFINISHED_H
#define ACQFINISHED_H

#include "ThreadUtils.h"

namespace lima
{

class AcqFinished
{
public:
	AcqFinished();

	void signalFinished();
	bool hasFinished();
	void waitFinished();

	void resetFinished();

private:
	AutoMutex lock();
	Cond m_cond;
	bool m_finished;
};

} // namespace lima

#endif // ACQFINISHED_H
