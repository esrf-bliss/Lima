#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "HwBufferMgr.h"
#include "FrameBuilder.h"
#include "ThreadUtils.h"

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

		
	protected:
		virtual void init();
		virtual void execCmd(int cmd);
	private:
		Simulator& m_simu;

	};
	friend class SimuThread;

	BufferCtrlMgr m_buffer_mgr;
	FrameBuilder m_frame_builder;
	double m_exp_time;
	int m_nb_frames;

	SimuThread m_thread;
};

} // namespace lima

#endif // SIMULATOR_H
