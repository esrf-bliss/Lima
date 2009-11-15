class AcqState
{
%TypeHeaderCode
#include <AcqState.h>
using namespace lima;
%End
public:
	enum State {
		Idle, Running, Finished, Aborted,
	};

	AcqState();

	void set(State new_state);
	State get();
	void wait(State state);
	void waitNot(State state);
};

