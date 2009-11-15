#ifndef ACQSTATE_H
#define ACQSTATE_H

#include "ThreadUtils.h"

namespace lima
{

class AcqState
{
public:
	enum State {
		Idle, Running, Finished, Aborted,
	};

	AcqState();

	void set(State new_state);
	State get();
	void wait(State state);
	void waitNot(State state);

private:
	AutoMutex lock();
	Cond m_cond;
	State m_state;
};

std::ostream& operator <<(std::ostream& os, AcqState::State state);

} // namespace lima

#endif // ACQSTATE_H
