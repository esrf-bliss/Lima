#ifndef ACQSTATE_H
#define ACQSTATE_H

#include "LimaCompatibility.h"
#include "ThreadUtils.h"

#include <ostream>

namespace lima
{

class LIMACORE_API AcqState
{
public:
	enum State {
		Idle		= 1 << 0, 
		Acquiring	= 1 << 1, 
		Saving		= 1 << 2,
		Finished	= 1 << 3, 
		Aborted		= 1 << 4,
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

inline AcqState::State operator |(AcqState::State s1, AcqState::State s2)
{
	return AcqState::State(int(s1) | int(s2));
}

inline AcqState::State operator &(AcqState::State s1, AcqState::State s2)
{
	return AcqState::State(int(s1) & int(s2));
}

std::ostream& operator <<(std::ostream& os, AcqState::State state);

} // namespace lima

#endif // ACQSTATE_H
