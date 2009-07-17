#include <iostream>

#include "FoclaHwInterface.h"
#include "EspiaBufferMgr.h"
#include "PoolThreadMgr.h"
#include "CtSaving.h"
#include "Data.h"

using namespace lima;
using namespace std;


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
	saving_par.directory = ".";
	saving_par.prefix = "img";
	saving_par.suffix = ".edf";
	saving_par.nextNumber = 0;
	saving_par.savingMode = CtSaving::AutoFrame;
	saving_par.overwritePolicy = CtSaving::Overwrite;
	saving_par.framesPerFile = 1;
	buffer_save.setParameters(saving_par);



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
