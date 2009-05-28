#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "HwBufferMgr.h"
#include "FrameBuilder.h"
#include "ThreadUtils.h"
#include "SizeUtils.h"
#include <ostream>

namespace lima
{

class Simulator
{
 public:
	enum Status {
		Ready, Exposure, Readout,
	};

	Simulator();
	~Simulator();

	BufferCtrlMgr& getBufferMgr();
	
	void startAcq();
	void stopAcq();

	void setNbFrames(int  nb_frames);
	void getNbFrames(int& nb_frames);

	void setExpTime(double  exp_time);
	void getExpTime(double& exp_time);

	void setBin(const Bin& bin);
	void getBin(Bin& bin);

	void setFrameDim(const FrameDim& frame_dim);
	void getFrameDim(FrameDim& frame_dim);
	
	Status getStatus();
	int getNbAcquiredFrames();

 private:
	class SimuThread : public CmdThread
	{
	public:
		enum { // Status
			Ready = MaxThreadStatus, Exposure, Readout,
		};

		enum { // Cmd 
			StartAcq = MaxThreadCmd, StopAcq,
		};
		
		SimuThread(Simulator& simu);

		virtual void start();
		
		int getNbAcquiredFrames();

	protected:
		virtual void init();
		virtual void execCmd(int cmd);
	private:
		void execStartAcq();

		Simulator& m_simu;
		int m_acq_frame_nb;
	};
	friend class SimuThread;

	void init();

	SoftBufferAllocMgr m_buffer_alloc_mgr;
	StdBufferCbMgr m_buffer_cb_mgr;
	BufferCtrlMgr m_buffer_ctrl_mgr;
	FrameBuilder m_frame_builder;
	double m_exp_time;
	int m_nb_frames;

	SimuThread m_thread;
};

std::ostream& operator <<(std::ostream& os, Simulator& simu);

} // namespace lima

#endif // SIMULATOR_H
