class AcqState
{
%TypeHeaderCode
#include <AcqState.h>
using namespace lima;
%End
public:
	enum State {
		Idle,		// = 1 << 0, 
		Acquiring,	// = 1 << 1, 
		Saving,		// = 1 << 2,
		Finished,	// = 1 << 3, 
		Aborted,	// = 1 << 4,
	};

	AcqState();

	void set(State new_state);
	State get();
	void wait(State state);
	void waitNot(State state);
};

