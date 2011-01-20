//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
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

