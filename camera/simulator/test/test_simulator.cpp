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

#include <iostream>
#include <numeric>
#include <chrono>
#include <unistd.h>

#include "processlib/PoolThreadMgr.h"

#include "lima/CtControl.h"
#include "lima/CtAcquisition.h"
#include "lima/CtSaving.h"
#include "lima/CtImage.h"

#include "SimulatorInterface.h"
#include "SimulatorFrameBuilder.h"
#include "SimulatorFrameLoader.h"
#include "SimulatorFramePrefetcher.h"

using namespace lima;


void config_loader(Simulator::Camera& simu)
{
    simu.setMode(Simulator::Camera::Mode::MODE_LOADER);
    simu.getFrameLoader()->setFilePattern("input\\test_*.edf");
}

void config_loader_prefetched(Simulator::Camera& simu)
{
    simu.setMode(Simulator::Camera::MODE_LOADER_PREFETCH);    
    simu.getFrameLoaderPrefetched()->setNbPrefetchedFrames(20);
    simu.getFrameLoaderPrefetched()->setFilePattern("input\\test_*.edf");
}

void config_generator(Simulator::Camera& simu)
{
    simu.setMode(Simulator::Camera::MODE_GENERATOR);
    //simu.getFrameBuilder()->
}

void config_generator_prefetched(Simulator::Camera& simu)
{
    simu.setMode(Simulator::Camera::MODE_GENERATOR_PREFETCH);
    simu.getFrameBuilderPrefetched()->setNbPrefetchedFrames(20);    
}

void simulator_test(double expo, long nframe)
{
    PoolThreadMgr::get().setNumberOfThread(1);
    
    const int nb_prebuilt_frames = 10;

	//DebParams::setTypeFlags(DebParams::AllFlags);

	Simulator::Camera simu;

    //config_loader(simu);
    //config_loader_prefetched(simu);
    config_generator(simu);
    //config_generator_prefetched(simu);

	Simulator::Interface hw(simu);
	CtControl ct = CtControl(&hw);

	CtSaving *save = ct.saving();
	save->setDirectory("./data");
	save->setPrefix("test_");
	//save->setSuffix(".h5");
	save->setSuffix(".edf");
	save->setNextNumber(100);
	//save->setFormat(CtSaving::HDF5);
	save->setFormat(CtSaving::EDF);
	save->setSavingMode(CtSaving::AutoFrame);
    //save->setOverwritePolicy(CtSaving::Overwrite);
	save->setFramesPerFile(100);

	save->setStatisticHistorySize(nframe);

	Bin bin(2, 2);
	CtImage *image = ct.image();
	image->setBin(bin);

	std::cout << "SIMUTEST: " << expo << " sec / " << nframe << " frames" << std::endl;

	CtAcquisition *acq = ct.acquisition();
	acq->setAcqMode(Single);
	acq->setAcqExpoTime(expo);
	acq->setAcqNbFrames(nframe);

	ct.prepareAcq();

	ct.startAcq();
	std::cout << "SIMUTEST: acq started" << std::endl;

	long frame = -1;
	while (frame < (nframe - 1))
    {
		using namespace std::chrono;

		high_resolution_clock::time_point begin = high_resolution_clock::now();

		usleep(100000);
		
		CtControl::ImageStatus img_status;
		ct.getImageStatus(img_status);

		high_resolution_clock::time_point end = high_resolution_clock::now();

		auto duration = duration_cast<microseconds>(end - begin).count();

		std::cout << "SIMUTEST: acq frame nr " << img_status.LastImageAcquired
			<< " - saving frame nr " << img_status.LastImageSaved << std::endl;

		if (frame != img_status.LastImageAcquired) {
			unsigned int nb_frames = img_status.LastImageAcquired - frame;

			std::cout << "  " << duration << " usec for " << nb_frames << " frames\n";
			std::cout << "  " << 1e6 * nb_frames / duration << " fps" << std::endl;

			frame = img_status.LastImageAcquired;
		}
	}
	std::cout << "SIMUTEST: acq finished" << std::endl;

	//Get statistics
	double saving_speed;
	double compression_speed;
	double compression_ratio;
	double incoming_speed;
	save->getStatisticCounters(saving_speed, compression_speed, compression_ratio, incoming_speed);

	ct.stopAcq();
	std::cout << "SIMUTEST: acq stopped" << std::endl;
}


void simulator_test_int_trig_mult(double expo, long nframe)
{
	Simulator::Camera simu;

	Simulator::Interface hw(simu);
	CtControl ct = CtControl(&hw);
		std::cout << "SIMUTEST: " << expo << " sec / " << nframe << " frames" << std::endl;

	CtAcquisition *acq = ct.acquisition();
	acq->setAcqMode(Single);
	acq->setAcqExpoTime(expo);
	acq->setAcqNbFrames(nframe);
	acq->setTriggerMode(IntTrigMult);

	ct.prepareAcq();
	ct.startAcq();

	std::cout << "SIMUTEST: acq started" << std::endl;

	long frame = -1;
	while (frame < (nframe - 1))
	{
		usleep(100000);

		CtControl::ImageStatus img_status;
		ct.getImageStatus(img_status);

		CtControl::Status status;
		ct.getStatus(status);

		std::cout << "SIMUTEST: status " << status.AcquisitionStatus << std::endl;

		if (frame != img_status.LastImageAcquired) {
			if (frame < (nframe - 2))
				ct.startAcq();

			std::cout << "SIMUTEST: acq frame nr " << img_status.LastImageAcquired << std::endl;

			frame = img_status.LastImageAcquired;
		}
	}
	
	std::cout << "SIMUTEST: acq finished" << std::endl;

	CtControl::Status status;
	ct.getStatus(status);

	std::cout << "SIMUTEST: status " << status << std::endl;
}


int main(int argc, char *argv[])
{
	double expo;
	long nframe;

	if (argc != 3) {
		expo = 0.5;
		nframe = 5;
	}
	else {
		expo = atof(argv[1]);
		nframe = atoi(argv[2]);
	}

	try {
		simulator_test(expo, nframe);
		simulator_test_int_trig_mult(expo, nframe);
	}
	catch (Exception e) {
		std::cerr << "LIMA Exception:" << e.getErrMsg() << std::endl;
	}

	return 0;
}

