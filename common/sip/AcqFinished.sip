class AcqFinished
{
%TypeHeaderCode
#include <AcqFinished.h>
using namespace lima;
%End
public:
	AcqFinished();

	void signalFinished();
	bool hasFinished();
	void waitFinished();

	void resetFinished();
};


