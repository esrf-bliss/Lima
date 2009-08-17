#include "SimuHwInterface.h"
#include "CtControl.h"
#include "CtAcquisition.h"
#include "CtSaving.h"
#include "CtImage.h"

#include <iostream>

using namespace lima;
using namespace std;

void simulator_test(double expo, long nframe)
{
	Simulator simu;
	HwInterface *hw;
	CtControl *ct;
	CtAcquisition *acq;
	CtSaving *save;
	CtImage *image;
	CtControl::ImageStatus img_status;
	long frame= -1;

	hw= new SimuHwInterface(simu);
	ct= new CtControl(hw);
	ct->setDebug(0xff);

	save= ct->saving();
	save->setDirectory("./data");
 	save->setPrefix("test_");
	save->setSuffix(".edf");
	save->setNextNumber(100);
	save->setFormat(CtSaving::EDF);
	save->setSavingMode(CtSaving::AutoFrame);
	save->setFramesPerFile(100);

	Bin bin(2,2);
	image= ct->image();
	image->setBin(bin);

	cout << "SIMUTEST: " << expo <<" sec / " << nframe << " frames" << endl;

	acq= ct->acquisition();
	acq->setAcqMode(Single);
	acq->setAcqExpoTime(expo);
	acq->setAcqNbFrames(nframe);

	ct->prepareAcq();
   	ct->startAcq();
	cout << "SIMUTEST: acq started" << endl;

	while (frame < (nframe-1)) {
	    usleep(100000);
	    ct->getImageStatus(img_status);
	    if (frame!=img_status.LastImageAcquired) {
		frame= img_status.LastImageAcquired;
	    	cout << "SIMUTEST: frame nr " << frame << endl;
	    }
	}
	cout << "SIMUTEST: acq finished" << endl;
	
	ct->stopAcq();
	cout << "SIMUTEST: acq stopped" << endl;
}

int main(int argc, char *argv[])
{
	double expo;
	long nframe;

	if (argc != 3) {
		expo= 0.5;
		nframe= 5;
	} else {
		expo= atof(argv[1]);
		nframe= atoi(argv[2]);
	}
        try {
                simulator_test(expo, nframe);
        } catch (Exception e) {
                cerr << "LIMA Exception:" << e << endl;
        }

        return 0;
}

