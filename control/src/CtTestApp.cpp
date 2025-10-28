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

#include <iostream>
#include <fstream>
#include <cstring>

#ifdef __unix
#include <unistd.h>
#include <sys/time.h>
#else
#include <processlib/win/unistd.h>
#include <windows.h> // For Sleep()
#include <process.h> // For _getpid()
#include <sysinfoapi.h> // For GetTickCount64
#endif

#include <math.h>
#include <stdlib.h>

#include "lima/RegExUtils.h"
#include "lima/SoftOpExternalMgr.h"

using namespace std;
using namespace lima;

DEB_GLOBAL(DebModTest);

CtTestApp::Pars::Pars()
{
	DEB_CONSTRUCTOR();

	buffer_alloc_params.initMem = true;
	buffer_alloc_params.reqMemSizePercent = 70.0;

#define AddOpt(var, opt, par) \
	m_opt_list.insert(MakeOpt(var, "", opt, par))

	AddOpt(debug_type_flags, "--debug-type-flags", "debug type flags");

	AddOpt(image_bin, "--image-bin", "image binning [HxV]");

	AddOpt(image_roi, "--image-roi", "image roi [T,L-WxH]");

	AddOpt(acq_mode, "--acq-mode", "acquisition mode");

	AddOpt(acq_expo_time, "--acq-expo-time", "exposure time");

	AddOpt(acq_lat_time, "--acq-latency-time", "latency time");

	AddOpt(acq_nb_frames, "--acq-nb-frames", "number of frames");

	AddOpt(acc_time_mode, "--acc-time-mode", "accumulation time mode");

	AddOpt(acc_max_expo_time, "--acc-max-expo-time",
	       "accumulation max exposure time");

	AddOpt(saving_managed_mode, "--saving-managed-mode",
	       "saving managed mode");

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

	AddOpt(saving_statistics_history, "--saving-statistics-history",
	       "saving statistics history size");

	AddOpt(video_active, "--video-active", "video active");

	AddOpt(video_source, "--video-source", "video source");

	AddOpt(buffer_alloc_params, "--buffer-alloc-params",
	       "buffer allocation parameters");

	AddOpt(acc_buffer_params, "--acc-buffer-params",
	       "accumulation buffer allocation parameters");

	AddOpt(saving_zbuffer_params, "--saving-zbuffer-params",
	       "saving zbuffer allocation parameters");

	AddOpt(proc_nb_threads, "--proc-nb-threads",
	       "number of processing threads");

	AddOpt(proc_mask_file_name, "--proc-mask-file-name",
	       "name of EDF file with mask image to apply");

	AddOpt(test_attach_debugger, "--test-attach-debugger",
	       "prompt at start to allow attaching debugger");

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

	AddOpt(test_acq_stop_policy, "--test-acq-stop-policy",
	       "acq stop policy: Never | Random | Fixed");

	AddOpt(test_acq_stop_nb_frames, "--test-acq-stop-nb-frames",
	       "number of frames limit before acq stop");

	AddOpt(test_nb_exec_threads, "--test-nb-exec-threads",
	       "number of threads executing acq commands");
}

CtTestApp::ExecThread::ExecThread(CtTestApp *app)
	: m_app(app)
{
	DEB_CONSTRUCTOR();
	m_run = true;
	start();
	AutoMutex l(m_cond.mutex());
	while (m_run)
		m_cond.wait();
}

CtTestApp::ExecThread::~ExecThread()
{
	DEB_DESTRUCTOR();
	{
		AutoMutex l(m_cond.mutex());
		m_end = true;
		m_cond.signal();
	}
	join();
}

void CtTestApp::ExecThread::runAcq(const index_map& indexes)
{
	DEB_MEMBER_FUNCT();
	AutoMutex l(m_cond.mutex());
	m_indexes = &indexes;
	m_run = true;
	m_cond.signal();
	while (m_run)
		m_cond.wait();
}

void CtTestApp::ExecThread::threadFunction()
{
	DEB_MEMBER_FUNCT();
	AutoMutex l(m_cond.mutex());
	while (true) {
		m_run = false;
		m_cond.signal();
		while (!m_run && !m_end)
			m_cond.wait();
		if (m_end)
			break;
		{
			AutoMutexUnlock u(l);
			m_app->runAcq(*m_indexes);
		}
		m_indexes = nullptr;
	}
}

CtTestApp::CtTestApp(int argc, char *argv[])
	: m_args(argc, argv), m_pars(NULL), m_ct(NULL)
{
	DEB_CONSTRUCTOR();
}

CtTestApp::~CtTestApp()
{
	DEB_DESTRUCTOR();
}

void CtTestApp::run()
{
	DEB_MEMBER_FUNCT();

	init();

	int thread_i = 0;
	int nb_exec_threads = m_pars->test_nb_exec_threads;

	index_map indexes;
	for (int i = 0; i < m_pars->test_nb_seq; ++i) {
		indexes["acq"] = i;
		for (int j = 0; j < int(m_pars->acq_nb_frames.size()); ++j) {
			indexes["nb_frames"] = j;
			configureAcq(indexes);
			if (nb_exec_threads) {
				int t = thread_i++ % nb_exec_threads;
				m_exec_thread_list[t]->runAcq(indexes);
			} else {
				runAcq(indexes);
			}
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

	if (m_pars->test_attach_debugger) {
		char buffer[1024];
		DEB_ALWAYS() << "Press enter to continue";
		fgets(buffer, sizeof(buffer), stdin);
	}

	m_ct = getCtControl();
	CtAcquisition *acq = m_ct->acquisition();
	CtAccumulation *acc = m_ct->accumulation();
	CtSaving *save = m_ct->saving();
	CtImage *image = m_ct->image();
	CtBuffer *buffer = m_ct->buffer();
	CtVideo *video = m_ct->video();
	SoftOpExternalMgr* ext_op = m_ct->externalOperation();

	bool endless_acq = false;
	for (int i = 0; i < int(m_pars->acq_nb_frames.size()); ++i)
		if (m_pars->acq_nb_frames[i] == 0)
			endless_acq = true;

	if (m_pars->saving_managed_mode != CtSaving::Software)
		THROW_CTL_ERROR(NotSupported)
			<< "Only Software saving managed mode is supported";

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

	save->setManagedMode(m_pars->saving_managed_mode);
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

	DEB_ALWAYS() << DEB_VAR3(m_pars->acq_mode, m_pars->acq_expo_time,
				 m_pars->acq_lat_time);
	acq->setAcqMode(m_pars->acq_mode);
	if (m_pars->acq_mode == Accumulation) {
		DEB_ALWAYS() << DEB_VAR2(m_pars->acc_time_mode,
					 m_pars->acc_max_expo_time);
		acq->setAccTimeMode(m_pars->acc_time_mode);
		acq->setAccMaxExpoTime(m_pars->acc_max_expo_time);
	}
	acq->setAcqExpoTime(m_pars->acq_expo_time);
	acq->setLatencyTime(m_pars->acq_lat_time);

	video->setVideoSource(m_pars->video_source);
	video->setActive(m_pars->video_active);

	// buffer management
	DEB_ALWAYS() << DEB_VAR1(m_pars->buffer_alloc_params);
	buffer->setAllocParameters(m_pars->buffer_alloc_params);
	DEB_ALWAYS() << DEB_VAR1(m_pars->acc_buffer_params);
	acc->setBufferParameters(m_pars->acc_buffer_params);
	DEB_ALWAYS() << DEB_VAR1(m_pars->saving_zbuffer_params);
	save->setZBufferParameters(m_pars->saving_zbuffer_params);

	PoolThreadMgr& mgr = PoolThreadMgr::get();
	mgr.setNumberOfThread(m_pars->proc_nb_threads);

	// mask
	if (!m_pars->proc_mask_file_name.empty()) {
		DEB_ALWAYS() << DEB_VAR1(m_pars->proc_mask_file_name);
		m_mask_data = readEDFFileImage(m_pars->proc_mask_file_name);
		DEB_ALWAYS() << DEB_VAR1(m_mask_data);
		SoftOpInstance op;
		ext_op->addOp(MASK, "MaskTask", 0, op);
		SoftOpMask *mask_task = static_cast<SoftOpMask*>(op.m_opt);
		mask_task->setMaskImage(m_mask_data);
	}

	if (m_pars->test_acq_stop_policy != Never) {
		if (m_pars->test_acq_stop_nb_frames <= 0)
			THROW_CTL_ERROR(InvalidValue)
				<< "Invalid acq stop_nb_frames";
		if (m_pars->test_acq_stop_policy == Random) {
			unsigned seed = 0;
#ifdef __unix
			struct timeval tv;
			if (gettimeofday(&tv, NULL) != 0)
				THROW_CTL_ERROR(Error)
				<< "Error getting time-of-day";
			seed = (unsigned) tv.tv_usec;
#else
			seed = (unsigned) GetTickCount64();
#endif
			srand(seed);
		}
	} else if (endless_acq)
		THROW_CTL_ERROR(NotSupported)
			<< "Must specify an AcqStopPolicy with endless acqs";

	if (m_pars->test_nb_exec_threads == -1)
		m_pars->test_nb_exec_threads = m_pars->test_nb_seq;
	int nb_exec_threads = m_pars->test_nb_exec_threads;
	DEB_ALWAYS() << DEB_VAR1(nb_exec_threads);
	for (int i = 0; i < nb_exec_threads; ++i)
		m_exec_thread_list.emplace_back(new ExecThread(this));
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
#if defined(_WIN32)
				pid_t p = _getpid();
#else
				pid_t p = getpid();
#endif
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
		THROW_CTL_ERROR(Error) << "Error executing " << DEB_VAR1(cmd);
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

	PoolThreadMgr& mgr = PoolThreadMgr::get();

	int acq_idx = indexes.at("acq");
	int nb_frames_idx = indexes.at("nb_frames");
	int acq_nb_frames = m_pars->acq_nb_frames[nb_frames_idx];
	DEB_ALWAYS() << DEB_VAR3(acq_idx, nb_frames_idx, acq_nb_frames);
	m_ct->acquisition()->setAcqNbFrames(acq_nb_frames);

	bool endless_acq = (acq_nb_frames == 0);
	AcqStopPolicy stop_policy = m_pars->test_acq_stop_policy;
	bool do_stop = (stop_policy != Never);
	int stop_nb_frames = m_pars->test_acq_stop_nb_frames;
	DEB_ALWAYS() << "Requested: " << DEB_VAR2(stop_policy, stop_nb_frames);
	if (stop_policy == Random) {
		long nb_random = 1L << 32;
		double factor = double(stop_nb_frames + 1) / nb_random;
		stop_nb_frames = int(round(rand() * factor));
	}
	if (do_stop)
		DEB_ALWAYS() << "Effective: " << DEB_VAR2(stop_policy, stop_nb_frames);

	bool show_statistics = false;
	CtSaving *save = m_ct->saving();
	if (m_pars->saving_mode == CtSaving::AutoFrame) {
		int statistics_size = m_pars->saving_statistics_history;
		if (statistics_size == -1)
			statistics_size = acq_nb_frames;
		save->setStatisticHistorySize(statistics_size);
		show_statistics = true;
	}

	m_ct->prepareAcq();
	DEB_ALWAYS() << "acq prepared";

	long max_nb_buffers;
	m_ct->buffer()->getMaxNumber(max_nb_buffers);
	DEB_ALWAYS() << DEB_VAR1(max_nb_buffers);

   	m_ct->startAcq();
	DEB_ALWAYS() << "acq started";
	Timestamp t0 = Timestamp::now();

	bool stopped = false;
	long acq_frame = -1;
	long ready_frame = -1;
	long saved_frame = -1;
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
				   (ready_frame != img_status.LastImageReady) ||
				   (saved_frame != img_status.LastImageSaved));
			last_display_ts = ts;
		}
		if (changed) {
			acq_frame = img_status.LastImageAcquired;
			ready_frame = img_status.LastImageReady;
			saved_frame = img_status.LastImageSaved;
			DEB_ALWAYS() << acq_status << ", "
				     << "acquired:" << acq_frame << ", "
				     << "ready:" << ready_frame << ", "
				     << "saved: " << saved_frame;
		}
		bool new_all_acquired = (acq_frame == last_frame);
		if (new_all_acquired && !all_acquired) {
			Timestamp t = Timestamp::now();
			double elapsed = t - t0;
			double frame_rate = (acq_frame + 1) / elapsed;
			DEB_ALWAYS() << "all frames acquired: "
				     << DEB_VAR2(elapsed, frame_rate);
			all_acquired = true;
		}
		bool acq_running = (acq_status == AcqRunning);
		auto acq_error = control_status.Error;
		bool saving_error = false;
		if (acq_status == AcqFault) {
			switch (acq_error) {
			case CtControl::SaveUnknownError:
			case CtControl::SaveOpenError:
			case CtControl::SaveCloseError:
			case CtControl::SaveAccessError:
			case CtControl::SaveOverwriteError:
			case CtControl::SaveDiskFull:
			case CtControl::SaveOverun:
				saving_error = true;
				break;
			}
		}

		bool new_interrupted = (!all_acquired && !acq_running);
		if (new_interrupted && !acq_interrupted) {
			DEB_ALWAYS() << "acq interrupted: " << acq_status;
			DEB_ALWAYS() << DEB_VAR2(acq_error, saving_error);
			acq_interrupted = true;
		}
		bool hw_finished = (all_acquired || acq_interrupted);
		bool acq_finished;
		if (acq_running)
			acq_finished = false;
		else if (acq_error != CtControl::NoError)
			acq_finished = mgr.wait(0);
		else if (auto_frame) {
			long last_to_save = acq_interrupted ? ready_frame :
							      last_frame;
			bool new_all_saved = (img_status.LastImageSaved ==
					      last_to_save);
			if (new_all_saved && !all_saved) {
				DEB_ALWAYS() << "saving finished";
				all_saved = true;
			}
			acq_finished = all_saved;
		} else
			acq_finished = hw_finished;
		if (acq_finished) {
			DEB_ALWAYS() << "acq finished";
			break;
		}
		if (!stopped && do_stop && (acq_frame >= stop_nb_frames)) {
			DEB_ALWAYS() << "stopping acquisition ...";
			m_ct->stopAcq();
			stopped = true;
		}
		long sleep_us = int(m_pars->test_acq_loop_wait_time * 1e6);
		if (sleep_us > 0)
			usleep(sleep_us);
		else
#if defined(_WIN32)
			Sleep(0);
#else
			sched_yield();
#endif
	}

	Timestamp t = Timestamp::now();
	double elapsed = t - t0;
	DEB_ALWAYS() << DEB_VAR1(elapsed);

	if (show_statistics && (saved_frame >= 0)) {
		int statistics_size = save->getStatisticHistorySize();
		double incoming_speed;
		double compression_speed;
		double compression_ratio;
		double saving_speed;
		save->getStatisticCounters(saving_speed, compression_speed,
					   compression_ratio, incoming_speed);
		incoming_speed /= 1e6;
		compression_speed /= 1e6;
		saving_speed /= 1e6;
		DEB_ALWAYS() << "Saving statistics (MByte/s): "
			     << DEB_VAR5(statistics_size, incoming_speed,
					 compression_speed, compression_ratio,
					 saving_speed);
	}

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

Data CtTestApp::readEDFFileImage(std::string filename)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(filename);

	std::string error_suffix(" EDF file: " + filename);

	std::ifstream ifs(filename, std::ios::binary);
	if (!ifs)
		THROW_CTL_ERROR(Error) << "Error opening" << error_suffix;

	EDFHeaderMap header = readEDFHeader(ifs, filename);
	std::vector<std::string> mandatory_keys = {
		"ByteOrder", "DataType", "Size", "Dim_1", "Dim_2"
	};
	for (auto& key : mandatory_keys)
		if (header.find(key) == header.end())
			THROW_CTL_ERROR(Error) << "Key " << key << " missing in"
					       << error_suffix;
	if (header["ByteOrder"] != "LowByteFirst")
		THROW_CTL_ERROR(Error) << "Only Little-Endian supported in"
				       << error_suffix;
	Data::TYPE data_type;
	std::string edf_data_type = header["DataType"];
	if (edf_data_type == "UnsignedByte")
		data_type = Data::UINT8;
	else if (edf_data_type == "SignedByte")
		data_type = Data::INT8;
	else if (edf_data_type == "UnsignedShort")
		data_type = Data::UINT16;
	else if (edf_data_type == "SignedShort")
		data_type = Data::INT16;
	else if (edf_data_type == "UnsignedInteger")
		data_type = Data::UINT32;
	else if (edf_data_type == "SignedInteger")
		data_type = Data::INT32;
	else if (edf_data_type == "Unsigned64")
		data_type = Data::UINT64;
	else if (edf_data_type == "Signed64")
		data_type = Data::INT64;
	else if (edf_data_type == "FloatValue")
		data_type = Data::FLOAT;
	else if (edf_data_type == "DoubleValue")
		data_type = Data::DOUBLE;
	else
		THROW_CTL_ERROR(Error) << "Unsupported data type "
				       << edf_data_type << " in"
				       << error_suffix;
	auto to_long = [&](std::string s) {
		try {
			std::string::size_type end_pos;
			long v = std::stoi(s, &end_pos);
			if (end_pos != s.size())
				throw std::exception();
			return v;
		} catch (const std::exception& e) {
			THROW_CTL_ERROR(Error) << "Error reading number \""
					       << s << "\" in" << error_suffix;
		}				       
	};

	auto size = to_long(header["Size"]);
	auto width = to_long(header["Dim_1"]);
	auto height = to_long(header["Dim_2"]);

	Data res;
	res.type = data_type;
	res.dimensions = {int(width), int(height)};
	if (res.size() != size)
		THROW_CTL_ERROR(Error) << "Data size mismatch: expected="
				       << res.size() << ", got=" << size
				       << " in" << error_suffix;
	res.buffer = new Buffer(size);
	if (!ifs.read(static_cast<char*>(res.data()), size))
		THROW_CTL_ERROR(Error) << "Error reading data from" << error_suffix;

	DEB_RETURN() << DEB_VAR1(res);
	return res;
}

CtTestApp::EDFHeaderMap CtTestApp::readEDFHeader(std::ifstream& edf_file, std::string filename)
{
	DEB_MEMBER_FUNCT();
	DEB_TRACE() << DEB_VAR1(edf_file.tellg());

	std::string error_suffix(" EDF file: " + filename);

	const int HEADER_BLOCK_LEN = 512;
	const int MAX_HEADER_LEN = 16 * 1024;
	AutoPtr<char, true> hbuffer = new char [MAX_HEADER_LEN];
	char *hptr = hbuffer.getPtr();
	int hlen = 0;
	auto header_ok = [&]() {
		if ((hlen < 4) || (hlen % HEADER_BLOCK_LEN != 0))
			return false;
		return (std::strncmp(hptr + hlen - 2, "}\n", 2) == 0);
	};
	while (!header_ok()) {
		int to_read = HEADER_BLOCK_LEN;
		if (hlen + to_read >= MAX_HEADER_LEN)
			THROW_CTL_ERROR(Error) << "Header too large in"
					       << error_suffix;
		if (!edf_file.read(hptr + hlen, to_read))
			THROW_CTL_ERROR(Error) << "Error reading header in"
					       << error_suffix;
		hlen += edf_file.gcount();
	}
	if (std::strncmp(hptr, "{\n", 2) != 0)
		THROW_CTL_ERROR(Error) << "Invalid header in" << error_suffix;
	std::string hstr(hptr + 2, hlen - 4);
	DEB_TRACE() << DEB_VAR2(hstr, edf_file.tellg());

	std::string ws_char = " \t";
	std::string ws_str = "[" + ws_char + "]*";
	RegEx hline_re("(?P<key>[^" + ws_char + "]+)" + ws_str + "=" +
		       ws_str + "(?P<value>[^;]+);\n");
	RegEx::NameMatchListType name_match_list;
	hline_re.multiSearchName(hstr, name_match_list);
	if (name_match_list.size() == 0)
		THROW_CTL_ERROR(Error) << "Could not decode header in"
				       << error_suffix;

	EDFHeaderMap res;
	for (auto& full_match: name_match_list) {
		std::string value = full_match["value"];
		auto last = value.find_last_not_of(" \t");
		if (last != std::string::npos)
			value.resize(last + 1);
		res.emplace(full_match["key"], value);
	}
	
	DEB_RETURN() << DEB_VAR1(res);
	return res;
}

std::istream& lima::operator >>(std::istream& is,
				CtTestApp::AcqStopPolicy& stop_policy)
{
	typedef CtTestApp::AcqStopPolicy AcqStopPolicy;

	std::string policy;
	is >> policy;
	std::transform(policy.begin(), policy.end(), policy.begin(),
		       [](unsigned char c){ return ::tolower(c); });
	if (policy == "never")
		stop_policy = AcqStopPolicy::Never;
	else if (policy == "random")
		stop_policy = AcqStopPolicy::Random;
	else if (policy == "fixed")
		stop_policy = AcqStopPolicy::Fixed;
	else {
		std::ostringstream msg;
		msg << "AcqStopPolicy can't be: " << DEB_VAR1(policy);
		throw LIMA_EXC(Control, InvalidValue, msg.str());
	}
	return is;
}

std::ostream& lima::operator <<(std::ostream& os,
				const CtTestApp::AcqStopPolicy& stop_policy)
{
	typedef CtTestApp::AcqStopPolicy AcqStopPolicy;

	const char *policy = "Unknown";
	switch (stop_policy) {
	case AcqStopPolicy::Never:  policy = "Never";  break;
	case AcqStopPolicy::Random: policy = "Random"; break;
	case AcqStopPolicy::Fixed:  policy = "Fixed";  break;
	}
	return os << policy;
}

