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
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "lima/HwInterface.h"
#include "lima/HwBufferMgr.h"
#include "SimulatorFrameBuilder.h"
#include "lima/ThreadUtils.h"
#include "lima/SizeUtils.h"
#include <ostream>

namespace lima
{

namespace Simulator
{

class LIBSIMULATOR_API Camera
{
	DEB_CLASS_NAMESPC(DebModCamera, "Camera", "Simulator");
 public:
	Camera();
	~Camera();

	HwBufferCtrlObj *getBufferCtrlObj();
	FrameBuilder *getFrameBuilder();

	void prepareAcq();
	void startAcq();
	void stopAcq();

	void setNbFrames(int  nb_frames);
	void getNbFrames(int& nb_frames);

	void setExpTime(double  exp_time);
	void getExpTime(double& exp_time);

	void setLatTime(double  lat_time);
	void getLatTime(double& lat_time);

	void setTrigMode(TrigMode trig_mode) {m_trig_mode = trig_mode;};
	void getTrigMode(TrigMode& trig_mode) {trig_mode = m_trig_mode;};
	void setBin(const Bin& bin);
	void getBin(Bin& bin);
	void checkBin(Bin& bin);

	void setFrameDim(const FrameDim& frame_dim);
	void getFrameDim(FrameDim& frame_dim);
	
	HwInterface::StatusType::Basic getStatus();
	int getNbAcquiredFrames();

	void getMaxImageSize(Size& max_image_size);

	void reset();

	enum SimuShutterMode {
		FRAME,
		MANUAL
	};

 private:
	class SimuThread : public CmdThread
	{
        DEB_CLASS_NAMESPC(DebModCamera, "Camera", "SimuThread");
	public:
		enum { // Status
			Ready = MaxThreadStatus,
			Prepare,
			Exposure,
			Readout,
			Latency,
		};

		enum { // Cmd 
			PrepareAcq = MaxThreadCmd,
			StartAcq,
			StopAcq,
		};

		SimuThread(Camera& simu);
		~SimuThread();

		virtual void start();
		
		int m_acq_frame_nb;
	protected:
		virtual void init();
		virtual void execCmd(int cmd);
	private:
		void execStartAcq();
		Camera* m_simu;
	};
	friend class SimuThread;

	void init();

	SoftBufferCtrlObj m_buffer_ctrl_obj;
	FrameBuilder m_frame_builder;
	double m_exp_time;
	double m_lat_time;
	int m_nb_frames;
	TrigMode m_trig_mode;
	SimuThread m_thread;
};

LIBSIMULATOR_API std::ostream& operator <<(std::ostream& os, Camera& simu);

}

} // namespace lima


#endif // SIMULATOR_H
