/*******************************************************************
 * @file testfoclahwinterface.cpp
 * @brief This file is to test Espia::Focla hardware interface
 *
 * This test is based on testfreloninterface.cpp by A.Homs
 *
 * @author A.Kirov
 * @date 15/07/2009
 *******************************************************************/

#include <iostream>

#include "FoclaHwInterface.h"
#include "EspiaBufferMgr.h"
#include "PoolThreadMgr.h"
#include "CtSaving.h"
#include "Data.h"

using namespace lima;
using namespace std;


class FoclaFrameCallback : public HwFrameCallback
{
public:
	FoclaFrameCallback( Espia::Focla::Interface &hw_inter, 
	                    CtSaving &buffer_save, Cond &acq_finished ) 
		: m_hw_inter(hw_inter), m_buffer_save(buffer_save), 
		  m_acq_finished(acq_finished) {}
protected:
	virtual bool newFrameReady(const HwFrameInfoType& frame_info);
private:
	Espia::Focla::Interface &m_hw_inter;
	CtSaving &m_buffer_save;
	Cond &m_acq_finished;
};


bool FoclaFrameCallback::newFrameReady( const HwFrameInfoType &frame_info )
{
	int nb_acq_frames = m_hw_inter.getNbAcquiredFrames();
	HwInterface::Status status;
	m_hw_inter.getStatus(status);

	cout << "In callback:" << endl
	     << "  frame_info=" << frame_info << endl
	     << "  nb_acq_frames=" << nb_acq_frames << endl
	     << "  status=" << status << endl;

	Data aNewData = Data();
	aNewData.frameNumber = frame_info.acq_frame_nb;
	const Size &aSize = frame_info.frame_dim->getSize();
	aNewData.width = aSize.getWidth();
	aNewData.height = aSize.getHeight();
	aNewData.type = Data::UINT16;
	
	Buffer *aNewBuffer = new Buffer();
	aNewBuffer->owner = Buffer::MAPPED;
	aNewBuffer->data = (void*)frame_info.frame_ptr;
	aNewData.setBuffer(aNewBuffer);
	aNewBuffer->unref();

	m_buffer_save.frameReady(aNewData);

	HwSyncCtrlObj *hw_sync;
	m_hw_inter.getHwCtrlObj(hw_sync);
	int nb_frames;
	hw_sync->getNbFrames(nb_frames);
	if (frame_info.acq_frame_nb == nb_frames - 1)
		m_acq_finished.signal();

	return true;
}


void print_status(Espia::Focla::Interface& hw_inter)
{
	HwInterface::Status status;

	hw_inter.getStatus(status);
	cout << "status=" << status << endl;
}


void test_focla_hw_interface()
{
	Espia::Dev espia_dev(0);

	Espia::Focla::Dev focla(espia_dev);

	Espia::Acq espia_acq(espia_dev);
	Espia::BufferMgr espia_buffer_mgr(espia_acq);
	BufferCtrlMgr buffer_mgr(espia_buffer_mgr);


	cout << "Creating Focla Hw Interface ... " << endl;
	Espia::Focla::Interface hw_inter( espia_acq, buffer_mgr, focla );
	cout << " Done!" << endl;


	HwBufferCtrlObj *hw_buffer;
	hw_inter.getHwCtrlObj(hw_buffer);

	FrameDim frame_dim(1024, 1024, Bpp16);
	hw_buffer->setFrameDim(frame_dim);
	int max_nb_buffers;
	hw_buffer->getMaxNbBuffers(max_nb_buffers);
	cout << "MaxNbBuffers " << max_nb_buffers << endl;
	int nb_concat_frames = 1;
	hw_buffer->setNbConcatFrames(nb_concat_frames);  // ???
	int nb_buffers = max_nb_buffers/2;
	hw_buffer->setNbBuffers(nb_buffers);
	cout << "Allocated " << nb_buffers << " buffers" << endl << flush;

	hw_buffer->getFramedim(frame_dim);
	hw_buffer->getNbBuffers(nb_buffers);
	hw_buffer->getNbConcatFrames(nb_concat_frames);
	cout << "FrameDim " << frame_dim << ", "
	     << "NbBuffers " << nb_buffers << ", "
	     << "NbConcatFrames " << nb_concat_frames << endl << flush;


	CtControl aControl(NULL);
	CtSaving buffer_save(aControl);
	CtSaving::Parameters saving_par;
	saving_par.fileFormat = CtSaving::EDF;
	saving_par.directory = ".";
	saving_par.prefix = "img";
	saving_par.suffix = ".edf";
	saving_par.nextNumber = 0;
	saving_par.savingMode = CtSaving::AutoFrame;
	saving_par.overwritePolicy = CtSaving::Overwrite;
	saving_par.framesPerFile = 1;
	buffer_save.setParameters(saving_par);


	Cond acq_finished;
	FoclaFrameCallback cb(hw_inter, buffer_save, acq_finished);
	hw_buffer->registerFrameCallback(cb);


	HwSyncCtrlObj *hw_sync;
	hw_inter.getHwCtrlObj(hw_sync);


#if 1
	hw_sync->setNbFrames(10);
#else
	espia_acq.setNbFrames(10);
#endif /* 0 */

	print_status(hw_inter);
	hw_inter.startAcq();

#if 1
	acq_finished.wait();
#else
	Sleep(10);
#endif /* 0 */

	print_status(hw_inter);
	hw_inter.stopAcq();
	print_status(hw_inter);

}


int main(int argc, char *argv[])
{

	try {
		test_focla_hw_interface();
	} catch (Exception e) {
		cerr << "LIMA Exception: " << e << endl;
		return -1;
	}

	return 0;
}
