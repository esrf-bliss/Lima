#include "Simulator.h"

#include <iostream>

using namespace lima;
using namespace std;

int main(int argc, char *argv[])
{
	Simulator simu;

	cout << "simu=" << simu << endl;
	simu.startAcq();
	cout << "simu=" << simu << endl;
	simu.stopAcq();
	cout << "simu=" << simu << endl;

	simu.setExpTime(5);

	cout << "simu=" << simu << endl;
	simu.startAcq();
	cout << "simu=" << simu << endl;
	simu.stopAcq();
	cout << "simu=" << simu << endl;

	return 0;
}
