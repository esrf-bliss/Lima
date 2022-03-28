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

#ifndef __HW_TEST_APP_H
#define __HW_TEST_APP_H

#include "lima/AppPars.h"
#include "lima/HwInterface.h"
#include "lima/AcqState.h"

#include <iomanip>
#include <fstream>

namespace lima 
{

class HwTestApp
{
	DEB_CLASS_NAMESPC(DebModTest, "HwTestApp", "Hardware");

 public:
	class Pars : public AppPars
	{
		DEB_CLASS_NAMESPC(DebModTest, "HwTestApp::Pars", "Hardware");
	public:
		static constexpr DebParams::Flags DefaultDebugTypeFlags =
			DebTypeWarning | DebTypeError | DebTypeFatal;

		DebParams::Flags debug_type_flags{DefaultDebugTypeFlags};
		int nb_frames{10};
		double exp_time{0.1};
		double latency_time{0.1};
		double wait_sleep_time{0.2};
		bool save_edf_format{true};
		std::string out_dir{"/tmp"};

		Pars();
	};

	class EdfHeaderKey
	{
	public:
	EdfHeaderKey(const std::string& key) : m_key(key)
		{}
	private:
		friend std::ostream& operator <<(std::ostream& os,
						 const EdfHeaderKey& h);
		std::string m_key;
	};

	HwTestApp(int argc, char *argv[]);
	virtual ~HwTestApp() {}

	virtual void run();

 protected:
	class FrameCallback : public HwFrameCallback
	{
		DEB_CLASS_NAMESPC(DebModTest, "HwTestApp::FrameCallback", 
				  "Hardware");
	public:
		FrameCallback(HwTestApp *app);
	protected:
		virtual bool newFrameReady(const HwFrameInfoType& frame_info);
	private:
		HwTestApp *m_app;
	};

	friend class FrameCallback;

	virtual void init();
	virtual Pars *getPars() = 0;
	virtual void parseArgs();
	virtual HwInterface *getHwInterface() = 0;

	bool newFrameReady(const HwFrameInfoType& frame_info);

	void save_raw_data(int start_frame, int nb_frames);
	void save_edf_data(int start_frame, int nb_frames);
	void save_edf_frame(std::ofstream& of, int acq_idx, int edf_idx);

	AppArgs m_args;
	Pars *m_pars;
	HwInterface *m_interface;
	HwDetInfoCtrlObj *m_det_info;
	HwSyncCtrlObj *m_sync;
	HwBufferCtrlObj *m_buffer;
	lima::AcqState m_state;
	FrameCallback m_cb;
	Timestamp m_last_msg_timestamp;
};

std::ostream& operator <<(std::ostream& os, const HwTestApp::EdfHeaderKey& h)
{
	using namespace std;
	return os << setiosflags(ios::left) << resetiosflags(ios::right)
		  << setw(14) << setfill(' ') << h.m_key << " = ";
}


} // namespace lima

#endif // __HW_TEST_APP_H
