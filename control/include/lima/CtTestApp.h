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

#ifndef __CT_TEST_APP_H
#define __CT_TEST_APP_H

#include "lima/AppPars.h"

#include "lima/LimaCompatibility.h"
#include "lima/CtControl.h"
#include "lima/CtAcquisition.h"
#include "lima/CtSaving.h"
#include "lima/CtBuffer.h"
#include "lima/CtImage.h"
#include "lima/CtVideo.h"
#include "processlib/PoolThreadMgr.h"

namespace lima 
{

class LIMACORE_API CtTestApp
{
	DEB_CLASS_NAMESPC(DebModTest, "CtTestApp", "Control");

 public:
	typedef std::vector<std::string> string_array;
	typedef std::vector<int> int_array;
	typedef std::map<std::string, int> index_map;

	class LIMACORE_API Pars : public AppPars
	{
		DEB_CLASS_NAMESPC(DebModTest, "CtTestApp::Pars", "Control");
	public:
		static constexpr DebParams::Flags DefaultDebugTypeFlags =
			DebTypeWarning | DebTypeError | DebTypeFatal;

		DebParams::Flags debug_type_flags{DefaultDebugTypeFlags};

		Bin image_bin;
		Roi image_roi;
	
		AcqMode acq_mode{Single};
		double acq_expo_time{0.5};
		double acq_lat_time{0};
		int_array acq_nb_frames{{5}};
		CtAcquisition::AccTimeMode acc_time_mode{CtAcquisition::Live};
		double acc_max_expo_time{0};
	
		CtSaving::ManagedMode saving_managed_mode{CtSaving::Software};
		CtSaving::SavingMode saving_mode{CtSaving::AutoFrame};
		CtSaving::FileFormat saving_format{CtSaving::EDF};
		std::string saving_directory{"./data"};
		std::string saving_prefix{"test_"};
		std::string saving_suffix;
		int saving_next_number{100};
		int saving_frames_per_file{100};
		CtSaving::OverwritePolicy saving_overwrite_policy{
			CtSaving::Overwrite
		};
	
		bool video_active{false};
		CtVideo::VideoSource video_source{CtVideo::BASE_IMAGE};
	
		int buffer_max_memory{70};
		int proc_nb_threads{2};

		int test_nb_seq{1};
		double test_seq_lat{0};
		bool test_rm_files_before_acq{false};
		bool test_image_status_callback{false};
		std::string test_pre_seq_cmd;
		std::string test_post_seq_cmd;
		double test_acq_loop_wait_time{0.1};
		double test_acq_loop_display_time{0.1};
		int test_nb_exec_threads{0};

		Pars();
	};

	CtTestApp(int argc, char *argv[]);
	virtual ~CtTestApp() {}

	virtual void run();

 protected:
	class ImageStatusCallback : public CtControl::ImageStatusCallback
	{
		DEB_CLASS_NAMESPC(DebModTest, "CtTestApp::ImageStatusCallback", 
				  "Control");
	public:
		ImageStatusCallback(CtTestApp *app) : m_app(app) {}
	protected:
		virtual void imageStatusChanged(
				const CtControl::ImageStatus& img_status) {
			m_app->imageStatusChanged(img_status);
		}
		CtTestApp *m_app;
	};

	class ExecThread : public Thread
	{
		DEB_CLASS_NAMESPC(DebModTest, "CtTestApp::ExecThread", 
				  "Control");
	public:
		ExecThread(CtTestApp *app);
		~ExecThread();
		void runAcq(const index_map& indexes);
	protected:
		void threadFunction();
		CtTestApp *m_app;
		bool m_run;
		bool m_end{false};
		Cond m_cond;
		const index_map *m_indexes{nullptr};
	};

	virtual void init();
	virtual Pars *getPars() = 0;
	virtual void parseArgs();
	virtual index_map getIndexMap() = 0;
	virtual CtControl *getCtControl() = 0;
	virtual void configureAcq(const index_map& /*indexes*/) {}
	virtual void imageStatusChanged(const CtControl::ImageStatus& status);

	void execCmd(std::string cmd);

	void runAcq(const index_map& indexes);

	AppArgs m_args;
	Pars *m_pars;
	CtControl *m_ct;
	AutoPtr<ImageStatusCallback> m_img_status_cb;
	std::vector<AutoPtr<ExecThread>> m_exec_thread_list;
};

} // namespace lima

#endif // __CT_TEST_APP_H
