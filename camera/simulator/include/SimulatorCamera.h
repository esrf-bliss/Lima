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

#include <ostream>

#include "lima/HwInterface.h"
#include "lima/HwBufferMgr.h"
#include "lima/ThreadUtils.h"
#include "lima/SizeUtils.h"

#include "SimulatorCompatibility.h"

namespace lima
{

namespace Simulator
{

// Forward definitions
struct FrameGetter;
class FrameBuilder;
class FrameLoader;
template <typename FrameGetter>
class FramePrefetcher;

class LIBSIMULATOR_API Camera
{
    DEB_CLASS_NAMESPC(DebModCamera, "Camera", "Simulator");
 public:
     enum Mode {
         MODE_GENERATOR,            //<! Generate frames using a super smart multi-gaussian algorithm in realtime (default)
         MODE_GENERATOR_PREFETCH,   //<! Prebuild frames using a super smart multi-gaussian algorithm
         MODE_LOADER,               //<! Load frames from files in realtime
         MODE_LOADER_PREFETCH,      //<! Preload frames from files
     };

     enum SimuShutterMode {
         FRAME,
         MANUAL
     };

     static const char* DetectorModel[4];

	Camera(const Mode& mode = Mode::MODE_GENERATOR);
	~Camera();

	HwBufferCtrlObj *getBufferCtrlObj() { return &m_buffer_ctrl_obj; }
	
    FrameGetter* getFrameGetter() { return m_frame_getter; }
    
    FrameBuilder* getFrameBuilder();
    FramePrefetcher<FrameBuilder>* getFrameBuilderPrefetched();
    FrameLoader* getFrameLoader();
    FramePrefetcher<FrameLoader>* getFrameLoaderPrefetched();

    /// Returns the detectof model (according to the current mode) 
    void getDetectorModel(std::string& det_model) const;

	void prepareAcq();
	void startAcq();
	void stopAcq();

    void setMode(const Mode& mode);
    void getMode(Mode& mode) const { mode = m_mode; }

	void setNbFrames(int  nb_frames);
	void getNbFrames(int& nb_frames);

	void setExpTime(double  exp_time);
	void getExpTime(double& exp_time);

	void setLatTime(double  lat_time);
	void getLatTime(double& lat_time);

	void setTrigMode(TrigMode trig_mode) {m_trig_mode = trig_mode;};
	void getTrigMode(TrigMode& trig_mode) {trig_mode = m_trig_mode;};
	
    void setFrameDim(const FrameDim& frame_dim);
    void getFrameDim(FrameDim& frame_dim);

    void getMaxImageSize(Size& max_image_size) const;

	HwInterface::StatusType::Basic getStatus();
	int getNbAcquiredFrames();

	void reset();

 private:
	class SimuThread : public CmdThread
	{
        DEB_CLASS_NAMESPC(DebModCamera, "Camera", "SimuThread");
	public:
		enum { // Status
			Ready = MaxThreadStatus,
			Prepared,
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
		void execPrepareAcq();
		void execStartAcq();
		Camera* m_simu;
	};
	friend class SimuThread;


    void setDefaultProperties();
    void constructFrameGetter();

	double m_exp_time;
	double m_lat_time;
	int m_nb_frames;
	
    TrigMode m_trig_mode;
	
    SoftBufferCtrlObj m_buffer_ctrl_obj;
    SimuThread m_thread;

	Mode m_mode;                        //<! The current mode of the simulateur
    FrameGetter *m_frame_getter;        //<! The current frame getter (according to the mode)

    //unsigned int m_nb_prebuilt_frames;  //<! In MODE_GENERATOR_PREBUILT mode, the number of frame to prebuilt
    //std::string m_file_pattern;         //<! In MODE_LOADER mode, the file pattern use to load the frames

	//bool m_mis_cb_act;	//<! Used by setMaxImageSizeCallbackActive	
};

LIBSIMULATOR_API std::ostream& operator <<(std::ostream& os, Camera& simu);

}

} // namespace lima


#endif // SIMULATOR_H
