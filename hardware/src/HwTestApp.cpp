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

#include "lima/HwTestApp.h"

using namespace std;
using namespace lima;

DEB_GLOBAL(DebModTest);

HwTestApp::Pars::Pars()
{
	DEB_CONSTRUCTOR();

#define AddOpt(var, sopt, lopt, par)	\
	m_opt_list.insert(MakeOpt(var, sopt, lopt, par))

	AddOpt(debug_type_flags, "-d", "--debug-type-flags",
		    "debug type flags");

	AddOpt(nb_frames, "-n", "--nb-frames", "number of frames");

	AddOpt(exp_time, "-e", "--exp-time", "exposure time");

	AddOpt(latency_time, "-l", "--latency-time", "latency time");

	AddOpt(save_edf_format, "-e", "--save_edf-format", "");

	AddOpt(out_dir, "-o", "--out-dir", "out_dir");
}

HwTestApp::FrameCallback::FrameCallback(HwTestApp *app)
	: m_app(app)
{
	DEB_CONSTRUCTOR();
}

bool HwTestApp::FrameCallback::newFrameReady(const HwFrameInfoType& frame_info)
{
	DEB_MEMBER_FUNCT();
	return m_app->newFrameReady(frame_info);
}

HwTestApp::HwTestApp(int argc, char *argv[])
	: m_args(argc, argv), m_pars(NULL), m_interface(NULL),
	  m_det_info(NULL), m_sync(NULL), m_buffer(NULL), m_cb(this)
{
	DEB_CONSTRUCTOR();
}

void HwTestApp::init()
{
	DEB_MEMBER_FUNCT();
	m_pars = getPars();
	parseArgs();
	m_interface = getHwInterface();
	m_interface->getHwCtrlObj(m_det_info);
	m_interface->getHwCtrlObj(m_sync);
	m_interface->getHwCtrlObj(m_buffer);

	m_buffer->registerFrameCallback(m_cb);
}

void HwTestApp::parseArgs()
{
	DEB_MEMBER_FUNCT();
	m_pars->parseArgs(m_args);
	DebParams::enableTypeFlags(m_pars->debug_type_flags);
}

void HwTestApp::run()
{
	DEB_MEMBER_FUNCT();

	init();

	try {
		m_sync->setNbFrames(m_pars->nb_frames);
		m_sync->setExpTime(m_pars->exp_time);
		m_sync->setLatTime(m_pars->latency_time);

		Size frame_size;
		m_det_info->getDetectorImageSize(frame_size);
		ImageType image_type;
		m_det_info->getCurrImageType(image_type);
		FrameDim frame_dim(frame_size, image_type);
		m_buffer->setFrameDim(frame_dim);
		int max_buffers;
		m_buffer->getMaxNbBuffers(max_buffers);
		int nb_buffers = min(m_pars->nb_frames, max_buffers);
		m_buffer->setNbBuffers(nb_buffers);

		m_interface->prepareAcq();
		m_last_msg_timestamp = Timestamp::now();

		m_state.set(lima::AcqState::Acquiring);
		m_interface->startAcq();
		m_state.waitNot(lima::AcqState::Acquiring);

		int first = max(0, m_pars->nb_frames - nb_buffers);
		int save_frames = min(m_pars->nb_frames, nb_buffers);
		if (m_pars->save_edf_format) {
			save_edf_data(first, save_frames);
		} else {
			save_raw_data(first, save_frames);
		}

		while (true) {
			HwInterface::Status status;
			m_interface->getStatus(status);
			if (status.acq != AcqRunning)
				break;
			Sleep(10e-3);
		}
	} catch (string s) {
		cerr << "Exception: " << s << endl;
		exit(1);
	} catch (...) {
		cerr << "Exception" << endl;
		exit(1);
	}
}

bool HwTestApp::newFrameReady(const HwFrameInfoType& frame_info)
{
	DEB_MEMBER_FUNCT();

	if (frame_info.acq_frame_nb == m_pars->nb_frames - 1)
		m_state.set(lima::AcqState::Finished);

	Timestamp timestamp = Timestamp::now();
	if (timestamp - m_last_msg_timestamp > m_pars->wait_sleep_time) {
		DEB_ALWAYS() << DEB_VAR1(frame_info.acq_frame_nb);
		m_last_msg_timestamp = timestamp;
	}

	return true;
}

void HwTestApp::save_raw_data(int start_frame, int nb_frames)
{
	DEB_MEMBER_FUNCT();
	FrameDim frame_dim;
	m_buffer->getFrameDim(frame_dim);

	for (int i = 0; i < nb_frames; ++i) {
		ostringstream os;
		os << m_pars->out_dir << "/slsdetector.bin." << i;
		DEB_TRACE() << "Saving raw to " << os.str();
		ofstream of(os.str().c_str());
		void *buffer = m_buffer->getFramePtr(start_frame + i);
		of.write(static_cast<char *>(buffer), frame_dim.getMemSize());
	}
}

void HwTestApp::save_edf_data(int start_frame, int nb_frames)
{
	DEB_MEMBER_FUNCT();
	ostringstream os;
	os << m_pars->out_dir << "/slsdetector.edf";
	DEB_TRACE() << "Saving EDF to " << os.str();
	ofstream of(os.str().c_str());
	for (int i = 0; i < nb_frames; ++i)
		save_edf_frame(of, start_frame + i, i);
}

void HwTestApp::save_edf_frame(ofstream& of, int acq_idx, int edf_idx)
{
	DEB_MEMBER_FUNCT();
	FrameDim frame_dim;
	m_buffer->getFrameDim(frame_dim);
	Size frame_size = frame_dim.getSize();
	int image_bytes = frame_dim.getMemSize();
	
	ostringstream os;
	os << "{" << endl;
	os << EdfHeaderKey("HeaderID") << setiosflags(ios::right) 
	   << "EH:" << setfill('0') << setw(6) << (edf_idx + 1) 
	   << ":" << setfill('0') << setw(6) << 0 
	   << ":" << setfill('0') << setw(6) << 0 << "; " << endl;
	os << EdfHeaderKey("ByteOrder") << "LowByteFirst" << "; " << endl;
	os << EdfHeaderKey("DataType") << "UnsignedShort" << "; " << endl;
	os << EdfHeaderKey("Size") << image_bytes << "; " << endl;
	os << EdfHeaderKey("Dim_1") << frame_size.getWidth() << "; " << endl;
	os << EdfHeaderKey("Dim_2") << frame_size.getHeight() << "; " << endl;
	os << EdfHeaderKey("Image") << edf_idx << "; " << endl;
	os << EdfHeaderKey("acq_frame_nb") << edf_idx << "; " << endl;

	const int HEADER_BLOCK = 1024;
	int rem = (HEADER_BLOCK - 2) - os.str().size() % HEADER_BLOCK;
	if (rem < 0)
		rem += HEADER_BLOCK;
	os << string(rem, '\n') << "}" << endl;
	of << os.str();

	void *buffer = m_buffer->getFramePtr(acq_idx);
	of.write(static_cast<char *>(buffer), image_bytes);
}

