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

#include "lima/CtTestApp.h"

#include <unistd.h>

using namespace std;
using namespace lima;

DEB_GLOBAL(DebModTest);

CtTestApp::Pars::Pars()
{
	DEB_CONSTRUCTOR();

#define AddOpt(var, opt, par) \
	m_opt_list.insert(MakeOpt(var, "", opt, par))

	AddOpt(debug_type_flags, "--debug-type-flags", "debug type flags");

	AddOpt(image_bin, "--image-bin", "image binning [HxV]");

	AddOpt(image_roi, "--image-roi", "image roi [T,L-WxH]");

	AddOpt(acq_expo_time, "--acq-expo-time", "exposure time");

	AddOpt(acq_lat_time, "--acq-latency-time", "latency time");

	AddOpt(acq_nb_frames, "--acq-nb-frames", "number of frames");

	AddOpt(saving_mode, "--saving-mode", "saving mode");

	AddOpt(saving_format, "--saving-format", "saving format");

	AddOpt(saving_directory, "--saving-directory", "saving dir");

	AddOpt(saving_prefix, "--saving-prefix", "saving prefix");

	AddOpt(saving_suffix, "--saving-suffix", "saving suffix");

	AddOpt(saving_next_number, "--saving-next-number",
	       "saving next number");

	AddOpt(saving_frames_per_file, "--saving-frames-per-file",
	       "saving frames per file");

	AddOpt(saving_overwrite_policy, "--saving-overwrite-policy",
	       "saving overwrite policy");

	AddOpt(video_active, "--video-active", "video active");

	AddOpt(video_source, "--video-source", "video source");

	AddOpt(buffer_max_memory, "--buffer-max-memory",
	       "buffer max memory [% of total RAM]");

	AddOpt(proc_nb_threads, "--proc-nb-threads",
	       "nb of processing threads");

	AddOpt(test_nb_seq, "--test-nb-seq", "number of sequences");

	AddOpt(test_seq_lat, "--test-seq-lat", "latency between sequences");

	AddOpt(test_rm_files_before_acq, "--test-rm-files-before-acq",
	       "remove files before each acq");

	AddOpt(test_image_status_callback, "--test-image-status-callback",
	       "activate ImageStatusCallback");

	AddOpt(test_pre_seq_cmd, "--test-pre-seq-cmd",
	       "exec cmd before each sequence");

	AddOpt(test_post_seq_cmd, "--test-post-seq-cmd",
	       "exec cmd after each sequence");

	AddOpt(test_acq_loop_wait_time, "--test-acq-loop-wait-time",
	       "acq loop wait time");

	AddOpt(test_acq_loop_display_time, "--test-acq-loop-display-time",
	       "acq loop display time");
}

CtTestApp::CtTestApp(int argc, char *argv[])
	: m_args(argc, argv), m_pars(NULL), m_ct(NULL)
{
	DEB_CONSTRUCTOR();
}

void CtTestApp::run()
{
	DEB_MEMBER_FUNCT();

	init();

	index_map indexes;
	for (int i = 0; i < m_pars->test_nb_seq; ++i) {
		indexes["acq"] = i;
		for (int j = 0; j < int(m_pars->acq_nb_frames.size()); ++j) {
			indexes["nb_frames"] = j;
			configureAcq(indexes);
			runAcq(indexes);
		}
	}
}

void CtTestApp::parseArgs()
{
	DEB_MEMBER_FUNCT();
	m_pars->parseArgs(m_args);
	DebParams::enableTypeFlags(m_pars->debug_type_flags);
}

void CtTestApp::init()
{
	DEB_MEMBER_FUNCT();
	m_pars = getPars();
	parseArgs();
	m_ct = getCtControl();
	CtAcquisition *acq = m_ct->acquisition();
	CtSaving *save = m_ct->saving();
	CtImage *image = m_ct->image();
	CtBuffer *buffer = m_ct->buffer();
	CtVideo *video = m_ct->video();

	if (m_pars->test_image_status_callback) {
		m_img_status_cb = new ImageStatusCallback(this);
		m_ct->registerImageStatusCallback(*m_img_status_cb);
	}

	if (m_pars->saving_suffix.empty()) {
		switch (m_pars->saving_format) {
		case CtSaving::EDF:
			m_pars->saving_suffix = ".edf"; break;
		case CtSaving::EDFGZ:
			m_pars->saving_suffix = ".edf.gz"; break;
		case CtSaving::EDFLZ4:
			m_pars->saving_suffix = ".edf.lz4"; break;
		default:
			m_pars->saving_suffix = ".h5";
		}
	}

	save->setSavingMode(m_pars->saving_mode);
	if (m_pars->saving_mode == CtSaving::AutoFrame) {
		save->setDirectory(m_pars->saving_directory);
		save->setPrefix(m_pars->saving_prefix);
		save->setNextNumber(m_pars->saving_next_number);
		save->setFormat(m_pars->saving_format);
		save->setSuffix(m_pars->saving_suffix);
		save->setOverwritePolicy(m_pars->saving_overwrite_policy);
		save->setFramesPerFile(m_pars->saving_frames_per_file);
	}

	image->setBin(m_pars->image_bin);
	image->setRoi(m_pars->image_roi);

	if (m_pars->acq_lat_time == 0)
		acq->getLatencyTime(m_pars->acq_lat_time);

	DEB_ALWAYS() << DEB_VAR2(m_pars->acq_expo_time, m_pars->acq_lat_time);

	acq->setAcqMode(Single);
	acq->setAcqExpoTime(m_pars->acq_expo_time);
	acq->setLatencyTime(m_pars->acq_lat_time);

	video->setVideoSource(m_pars->video_source);
	video->setActive(m_pars->video_active);

	buffer->setMaxMemory(m_pars->buffer_max_memory);

	PoolThreadMgr& mgr = PoolThreadMgr::get();
	mgr.setNumberOfThread(m_pars->proc_nb_threads);
}

void CtTestApp::execCmd(std::string cmd)
{
	DEB_MEMBER_FUNCT();
	enum Keyword { PID };
	typedef std::map<Keyword, std::string> M;
	typedef M::value_type V;
	static M KeyStrings = { V(PID, "PID") };
	while (true) {
		bool has_keys = false;
		M::const_iterator it, end = KeyStrings.end();
		for (it = KeyStrings.begin(); it != end; ++it) {
			std::string token = "%" + it->second + "%";
			std::size_t t = cmd.find(token);
			if (t == std::string::npos)
				continue;
			has_keys = true;
			std::string r;
			switch (it->first) {
			case PID:
				pid_t p = getpid();
				r = std::to_string(p);
				break;
			}
			cmd.replace(t, token.size(), r);
		}
		if (!has_keys)
			break;
	}
	DEB_ALWAYS() << "executing " << DEB_VAR1(cmd);
	if (system(cmd.c_str()) != 0)
		THROW_HW_ERROR(Error) << "Error executing " << DEB_VAR1(cmd);
}

void CtTestApp::runAcq(const index_map& indexes)
{
	DEB_MEMBER_FUNCT();

	if (m_pars->test_rm_files_before_acq) {
		std::ostringstream os;
		os << "rm -f " << m_pars->saving_directory << "/"
		   << m_pars->saving_prefix << "*" << m_pars->saving_suffix;
		execCmd(os.str());
	}

	if (!m_pars->test_pre_seq_cmd.empty())
		execCmd(m_pars->test_pre_seq_cmd);

	int acq_idx = indexes.at("acq");
	int nb_frames_idx = indexes.at("nb_frames");
	int acq_nb_frames = m_pars->acq_nb_frames[nb_frames_idx];
	DEB_ALWAYS() << DEB_VAR3(acq_idx, nb_frames_idx, acq_nb_frames);
	m_ct->acquisition()->setAcqNbFrames(acq_nb_frames);

	m_ct->prepareAcq();
	DEB_ALWAYS() << "acq prepared";

   	m_ct->startAcq();
	DEB_ALWAYS() << "acq started";
	Timestamp t0 = Timestamp::now();

	long acq_frame = -1, saved_frame = -1;
	long last_frame = acq_nb_frames - 1;
	bool all_acquired = false;
	bool acq_interrupted = false;
	bool all_saved = false;
	bool auto_frame = (m_pars->saving_mode == CtSaving::AutoFrame);
	Timestamp last_display_ts = Timestamp::now();
	const double& display_time = m_pars->test_acq_loop_display_time;
	while (true) {
		CtControl::Status control_status;
		m_ct->getStatus(control_status);
		auto acq_status = control_status.AcquisitionStatus;
		CtControl::ImageStatus img_status;
		m_ct->getImageStatus(img_status);
		Timestamp ts = Timestamp::now();
		bool changed = false;
		if ((ts - last_display_ts) > display_time) {
			changed = ((acq_frame != img_status.LastImageAcquired) ||
				   (saved_frame != img_status.LastImageSaved));
			last_display_ts = ts;
		}
		if (changed) {
			acq_frame = img_status.LastImageAcquired;
			saved_frame = img_status.LastImageSaved;
			DEB_ALWAYS() << acq_status << ", "
				     << "acquired:" << acq_frame << ", "
				     << "saved: " << saved_frame;
		}
		bool new_all_acquired = (acq_frame == last_frame);
		if (new_all_acquired && !all_acquired) {
			DEB_ALWAYS() << "all frames acquired";
			all_acquired = true;
		}
		bool acq_running = (acq_status == AcqRunning);
		bool new_interrupted = (!all_acquired && !acq_running);
		if (new_interrupted && !acq_interrupted) {
			DEB_ALWAYS() << "acq interrupted: " << acq_status;
			acq_interrupted = true;
		}
		bool hw_finished = (all_acquired || acq_interrupted);
		bool acq_finished;
		if (acq_running)
			acq_finished = false;
		else if (auto_frame) {
			long last_to_save = acq_interrupted ? acq_frame :
							      last_frame;
			bool new_all_saved = (img_status.LastImageSaved ==
					      last_to_save);
			if (new_all_saved && !all_saved) {
				DEB_ALWAYS() << "saving finished";
				all_saved = true;
			}
			acq_finished = all_saved;
		} else {
			acq_finished = hw_finished;
		}
		if (acq_finished) {
			DEB_ALWAYS() << "acq finished";
			break;
		}

		long sleep_us = int(m_pars->test_acq_loop_wait_time * 1e6);
		if (sleep_us > 0)
			usleep(sleep_us);
		else
			pthread_yield();
	}

	Timestamp t = Timestamp::now();
	double elapsed = t - t0;
	DEB_ALWAYS() << DEB_VAR1(elapsed);

	if (m_pars->test_seq_lat > 0) {
		DEB_ALWAYS() << "sleeping " << DEB_VAR1(m_pars->test_seq_lat);
		usleep(m_pars->test_seq_lat * 1e6);
	}

	if (!m_pars->test_post_seq_cmd.empty())
		execCmd(m_pars->test_post_seq_cmd);
}

void CtTestApp::imageStatusChanged(const CtControl::ImageStatus& status)
{
	DEB_MEMBER_FUNCT();
}
