#include "Simulator.h"

#include <iostream>

using namespace lima;
using namespace std;

class TestFrameCallback : public HwFrameCallback
{
public:
	TestFrameCallback(Simulator& simu) : m_simu(simu) {}
protected:
	virtual bool newFrameReady(const FrameInfoType& frame_info);
private:
	Simulator& m_simu;
};

bool TestFrameCallback::newFrameReady(const FrameInfoType& frame_info)
{
	cout << "acq_frame_nb=" << frame_info.acq_frame_nb  << ", "
	     << "ts=" << frame_info.frame_timestamp << ", "
	     << "simu=" << m_simu  << endl;
	return true;
}


int main(int argc, char *argv[])
{
	Simulator simu;
	TestFrameCallback cb(simu);

	FrameDim frame_dim;
	simu.getFrameDim(frame_dim);

	BufferCtrlMgr& buffer_mgr = simu.getBufferMgr();
	buffer_mgr.setFrameDim(frame_dim);
	buffer_mgr.setNbBuffers(1);
	buffer_mgr.registerFrameCallback(&cb);

	cout << "simu=" << simu << endl;
	simu.startAcq();
	cout << "simu=" << simu << endl;
	simu.stopAcq();
	cout << "simu=" << simu << endl;

	simu.setExpTime(5);

	cout << "simu=" << simu << endl;
	simu.startAcq();
	cout << "simu=" << simu << endl;
	simu.stopAcq();
	cout << "simu=" << simu << endl;

	simu.setExpTime(2);
	simu.setNbFrames(3);

	cout << "simu=" << simu << endl;
	simu.startAcq();
	cout << "simu=" << simu << endl;
	simu.stopAcq();
	cout << "simu=" << simu << endl;

	return 0;
}
