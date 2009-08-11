#include "SimuHwInterface.h"
#include "CtControl.h"
#include "CtAcquisition.h"

#include <iostream>

using namespace lima;
using namespace std;

void simulator_test()
{
	Simulator simu;
	HwInterface *hw;
	CtControl *ct;
	CtAcquisition *acq;

	hw= new SimuHwInterface(simu);
	ct= new CtControl(hw);
	ct->setDebug(0xff);
	acq= ct->acquisition();

	cout << "ACQ: 0.5 sec / 5 frames" << endl;

	acq->setAcqMode(Single);
	acq->setAcqExpoTime(0.5);
	acq->setAcqNbFrames(5);

	ct->prepareAcq();
   	ct->startAcq();
	sleep(2);
	ct->stopAcq();
}

int main(int argc, char *argv[])
{
        try {
                simulator_test();
        } catch (Exception e) {
                cerr << "LIMA Exception:" << e << endl;
        }

        return 0;
}

