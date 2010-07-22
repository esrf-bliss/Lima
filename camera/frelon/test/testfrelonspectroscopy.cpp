#include "FrelonInterface.h"
#include "BufferSave.h"
#include "AcqState.h"
#include "PoolThreadMgr.h"

using namespace lima;
using namespace std;

DEB_GLOBAL(DebModTest);


class TestFrameCallback : public HwFrameCallback
{
	DEB_CLASS(DebModTest, "TestFrameCallback");

public:
	TestFrameCallback(Frelon::Interface& hw_inter, 
			  BufferSave& buffer_save, AcqState& acq_state);
	~TestFrameCallback();

	const HwFrameInfoType& getBufferInfo();

protected:
	virtual bool newFrameReady(const HwFrameInfoType& frame_info);

private:
	Frelon::Interface& m_hw_inter;
	BufferSave& m_buffer_save;
	AcqState& m_acq_state;

	void *m_buffer_ptr;
	int m_nb_frames;
	int m_nb_stripes;
	FrameDim m_buffer_dim;
	HwFrameInfoType m_buffer_info;
};

TestFrameCallback::TestFrameCallback(Frelon::Interface& hw_inter, 
				     BufferSave& buffer_save, 
				     AcqState& acq_state)
	: m_hw_inter(hw_inter), m_buffer_save(buffer_save), 
	  m_acq_state(acq_state)
{
	DEB_CONSTRUCTOR();

	m_buffer_ptr = NULL;
	m_nb_frames = 1;
	m_nb_stripes = 1;
}

TestFrameCallback::~TestFrameCallback()
{
	DEB_DESTRUCTOR();
}

bool TestFrameCallback::newFrameReady(const HwFrameInfoType& frame_info)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(frame_info);

	if (!frame_info.isValid()) {
		DEB_ERROR() << "Acquisition aborted";
		m_acq_state.set(AcqState::Aborted);
		return false;
	}

	int acq_frame_nb = frame_info.acq_frame_nb;
	if (acq_frame_nb == 0) {
		HwBufferCtrlObj *hw_buffer;
		m_hw_inter.getHwCtrlObj(hw_buffer);
		hw_buffer->getNbConcatFrames(m_nb_stripes);

		HwSyncCtrlObj *hw_sync;
		m_hw_inter.getHwCtrlObj(hw_sync);
		hw_sync->getNbFrames(m_nb_frames);
	}

	if (acq_frame_nb % m_nb_stripes == 0)
		m_buffer_ptr = frame_info.frame_ptr;

	if (((acq_frame_nb + 1) % m_nb_stripes) == 0) {
		Point factor(1, m_nb_stripes);
		m_buffer_dim = frame_info.frame_dim * factor;
		m_buffer_info = frame_info;
		m_buffer_info.frame_ptr = m_buffer_ptr;
		m_buffer_info.frame_dim = m_buffer_dim;
		m_buffer_info.valid_pixels *= m_nb_stripes;

		DEB_ALWAYS() << "Buffer ready: " << m_buffer_info;
	}

	if (acq_frame_nb == m_nb_frames - 1) {
		DEB_ALWAYS() << "Acq finished!";
		m_acq_state.set(AcqState::Finished);
	}

	return true;
}

const HwFrameInfoType& TestFrameCallback::getBufferInfo()
{
	return m_buffer_info;
}

void test_frelon_spectroscopy()
{
	DEB_GLOBAL_FUNCT();

	DebParams::disableTypeFlags(DebParams::AllFlags);

	Espia::Dev dev(0);
	Espia::Acq acq(dev);
	Espia::BufferMgr buffer_cb_mgr(acq);
	Espia::SerialLine ser_line(dev);
	Frelon::Camera cam(ser_line);
	BufferCtrlMgr buffer_mgr(buffer_cb_mgr);

	DEB_ALWAYS() << "Creating the Hw Interface ...";
	Frelon::Interface hw_inter(acq, buffer_mgr, cam);
	DEB_TRACE() << "Done!";

	BufferSave buffer_save(BufferSave::EDF, "big_img", 0, ".edf", true, 1);
	AcqState acq_state;
	TestFrameCallback cb(hw_inter, buffer_save, acq_state);

	HwDetInfoCtrlObj *hw_det_info;
	hw_inter.getHwCtrlObj(hw_det_info);

	HwBufferCtrlObj *hw_buffer;
	hw_inter.getHwCtrlObj(hw_buffer);

	HwSyncCtrlObj *hw_sync;
	hw_inter.getHwCtrlObj(hw_sync);

	HwBinCtrlObj *hw_bin;
	hw_inter.getHwCtrlObj(hw_bin);

	HwRoiCtrlObj *hw_roi;
	hw_inter.getHwCtrlObj(hw_roi);

	HwFlipCtrlObj *hw_flip;
	hw_inter.getHwCtrlObj(hw_flip);

	hw_buffer->registerFrameCallback(cb);

	DEB_ALWAYS() << "Setting Chans 3 & 4";
	cam.setInputChan(Frelon::Chan34);

	Flip flip(true, false);
	DEB_ALWAYS() << "Setting flip mode to " << flip;
	hw_flip->setFlip(flip);

	Bin bin(1, 64);
	DEB_ALWAYS() << "Setting binning to " << bin;
	hw_bin->setBin(bin);

	Roi roi(Point(0, 30), Size(2048, 1));
	DEB_ALWAYS() << "Checking " << DEB_VAR1(roi);
	Roi real_roi;
	hw_roi->checkRoi(roi, real_roi);
	DEB_TRACE() << "Got " << DEB_VAR1(real_roi);

	DEB_ALWAYS() << "Setting " << DEB_VAR1(roi);
	hw_roi->setRoi(roi);
	hw_roi->getRoi(real_roi);
	DEB_TRACE() << "Got " << DEB_VAR1(real_roi);

	DEB_TRACE() << "Setting Kinetic roi";
	cam.setRoiMode(Frelon::Kinetic);

	int nb_stripes = 10000;
	int nb_buffers = 1;

	DEB_ALWAYS() << "Setting stripe concatenation (big buffer)";
	FrameDim fdim;
	hw_buffer->getFrameDim(fdim);
	fdim.setSize(real_roi.getSize());
	hw_buffer->setFrameDim(fdim);
	hw_buffer->setNbConcatFrames(nb_stripes);
	hw_buffer->setNbBuffers(nb_buffers);

	double exp_time = 1e-6;
	int nb_frames = nb_buffers * nb_stripes;
	hw_sync->setExpTime(exp_time);
	hw_sync->setNbFrames(nb_frames);

	Timestamp t0, t1;

	DEB_ALWAYS() << "Starting acquisition";
	t0 = Timestamp::now();
	acq_state.set(AcqState::Acquiring);
	hw_inter.startAcq();
	acq_state.waitNot(AcqState::Acquiring);
	t1 = Timestamp::now();
	if (acq_state.get() == AcqState::Aborted)
		THROW_HW_ERROR(Error) << "Acquisition aborted!";

	DEB_ALWAYS() << "Acquisition finished after " << (t1 - t0) << " sec";

	DEB_ALWAYS() << "Saving buffer";
	buffer_save.writeFrame(cb.getBufferInfo());
	DEB_TRACE() << "Done!";
}

int main(int argc, char *argv[])
{
	DEB_GLOBAL_FUNCT();

	try {
		test_frelon_spectroscopy();
	} catch (Exception& e) {
		DEB_ERROR() << "LIMA Exception: " << e;
	}
	return 0;
}
