#include "FrelonCamera.h"
#include "EspiaBufferMgr.h"

#include <iostream>

using namespace lima;
using namespace std;

DEB_GLOBAL(DebModTest);


void print_str(const string& desc, const string& str)
{
	cout << desc << " \"" << str << "\"" << endl;
}

void split_msg(const Frelon::SerialLine& frelon_ser_line, const string& msg)
{
	Frelon::SerialLine::MsgPartStrMapType msg_parts;

	frelon_ser_line.splitMsg(msg, msg_parts);

	print_str("Msg",  msg);
	print_str("Sync", msg_parts[Frelon::SerialLine::MsgSync]);
	print_str("Cmd ", msg_parts[Frelon::SerialLine::MsgCmd]);
	print_str("Val ", msg_parts[Frelon::SerialLine::MsgVal]);
	print_str("Req ", msg_parts[Frelon::SerialLine::MsgReq]);
	print_str("Term", msg_parts[Frelon::SerialLine::MsgTerm]);
	cout << endl;
}

void frelon_cmd(Frelon::SerialLine& frelon_ser_line, const string& cmd)
{
	DEB_GLOBAL_FUNCT();

	split_msg(frelon_ser_line, cmd);
	frelon_ser_line.write(cmd);

	Timestamp t0, t1;
	string ans;

	t0 = Timestamp::now();
	frelon_ser_line.readLine(ans);
	t1 = Timestamp::now();
	DEB_TRACE() << "Elapsed " << (t1 - t0) << " sec";
	print_str("Ans", ans);
}

void frelon_read_reg(Frelon::Camera& frelon_cam, Frelon::Reg reg)
{
	DEB_GLOBAL_FUNCT();

	Timestamp t0, t1;
	int val;

	print_str("Reg", Frelon::RegStrMap[reg]);
	t0 = Timestamp::now();
	frelon_cam.readRegister(reg, val);
	t1 = Timestamp::now();
	DEB_TRACE() << "Elapsed " << (t1 - t0) << " sec";
	DEB_TRACE() << "Val" << " " << val;
}

void frelon_set_roi(Frelon::Camera& frelon_cam, const Roi& set_roi)
{
	Roi hw_roi;
	frelon_cam.getRoi(hw_roi);
	cout << "HwRoi  " << hw_roi << endl;
	cout << "SetRoi " << set_roi << endl;
	frelon_cam.checkRoi(set_roi, hw_roi);
	cout << "HwRoi  " << hw_roi << endl;
	frelon_cam.setRoi(set_roi);
	frelon_cam.getRoi(hw_roi);
	cout << "HwRoi  " << hw_roi << endl;
}

void print_sleep(double sleep_time)
{
	cout << "Sleep(" << sleep_time << "): " << Sleep(sleep_time) << endl;
}

void test_sleep()
{
	print_sleep(0.1);
	print_sleep(1.5);
	print_sleep(0.1);
	print_sleep(0.01);
	print_sleep(0.001);
}


class FrelonFrameCb : public HwFrameCallback
{
	DEB_CLASS(DebModTest, "FrelonFrameCb");

public:
	FrelonFrameCb(int nb_frames, Cond& acq_finished) 
		: m_nb_frames(nb_frames), m_acq_finished(acq_finished) {}

protected:
	virtual bool newFrameReady(const HwFrameInfoType& frame_info)
	{
		DEB_MEMBER_FUNCT();
		DEB_PARAM() << DEB_VAR1(frame_info);
		if (frame_info.acq_frame_nb == m_nb_frames - 1) {
			cout << endl;
			m_acq_finished.signal();
		}
		return true;
	}

private:
	int m_nb_frames;
	Cond& m_acq_finished;
};

void test_frelon(int espia_nb, bool do_reset)
{
	DEB_GLOBAL_FUNCT();

	Espia::Dev espia_dev(espia_nb);
	Espia::Acq espia_acq(espia_dev);
	Espia::BufferMgr espia_buffer_mgr(espia_acq);
	Espia::SerialLine espia_ser_line(espia_dev);
	Frelon::SerialLine frelon_ser_line(espia_ser_line);
	Frelon::Camera frelon_cam(espia_ser_line);
	BufferCtrlMgr buffer_mgr(espia_buffer_mgr);

	string msg;

	if (do_reset) {
		DEB_TRACE() << "Resetting camera ... ";
		frelon_cam.hardReset();
		DEB_TRACE() << "  Done!";
	}

	string ver;
	frelon_cam.getVersion(ver);
	print_str("Ver", ver);

	msg = ">C\r\n";
	frelon_cmd(frelon_ser_line, msg);

	frelon_read_reg(frelon_cam, Frelon::ExpTime);
	frelon_cam.writeRegister(Frelon::ExpTime, 200);
	frelon_read_reg(frelon_cam, Frelon::ExpTime);

	frelon_cam.writeRegister(Frelon::ExpTime, 100);
	frelon_read_reg(frelon_cam, Frelon::ExpTime);

	Frelon::FrameTransferMode ftm;
	frelon_cam.getFrameTransferMode(ftm);
	string mode = (ftm == Frelon::FTM) ? "FTM" : "FFM";
	print_str("Mode", mode);

	Frelon::InputChan input_chan;
	frelon_cam.getInputChan(input_chan);
	DEB_TRACE() << "Chan " << int(input_chan) << ": ";

	string sep = "";
	for (int i = 0; i < 4; i++) {
		Frelon::InputChan chan = Frelon::InputChan(1 << i);
		if (frelon_cam.isChanActive(input_chan, chan)) {
			DEB_TRACE() << sep << (i + 1);
			sep = "&";
		}
	}
	DEB_TRACE();

	test_sleep();

	DEB_TRACE() << "TopLeft:     " << TopLeft    ;
	DEB_TRACE() << "TopRight:    " << TopRight   ;
	DEB_TRACE() << "BottomLeft:  " << BottomLeft ;
	DEB_TRACE() << "BottomRight: " << BottomRight;

	Bin bin;
	Roi roi;

	frelon_cam.getBin(bin);
	DEB_TRACE() << "Bin " << bin;
	bin = 1;
	DEB_TRACE() << "Bin " << bin;
	frelon_cam.setBin(bin);
	frelon_cam.getBin(bin);
	DEB_TRACE() << "Bin " << bin;
	
	roi = Roi(Point(515, 517), Size(1021, 521));
	frelon_set_roi(frelon_cam, roi);

	frelon_cam.getBin(bin);
	DEB_TRACE() << "Bin " << bin;
	bin = 2;
	DEB_TRACE() << "Bin " << bin;
	frelon_cam.setBin(bin);
	frelon_cam.getBin(bin);
	DEB_TRACE() << "Bin " << bin;
	
	roi = Roi(Point(257, 259), Size(509, 265));
	frelon_set_roi(frelon_cam, roi);

	roi = Roi(Point(57, 59), Size(71, 72));
	frelon_set_roi(frelon_cam, roi);

	bin = 1;
	DEB_TRACE() << "Bin " << bin;
	frelon_cam.setBin(bin);
	frelon_cam.getBin(bin);
	DEB_TRACE() << "Bin " << bin;

	roi.reset();
	frelon_set_roi(frelon_cam, roi);

	FrameDim frame_dim;
	frelon_cam.getFrameDim(frame_dim);
	frame_dim /= bin;

	buffer_mgr.setFrameDim(frame_dim);
	int max_nb_buffers;
	buffer_mgr.getMaxNbBuffers(max_nb_buffers);
	DEB_TRACE() << "MaxNbBuffers " << max_nb_buffers;
	int nb_concat_frames = 1;
	buffer_mgr.setNbConcatFrames(nb_concat_frames);
	int nb_buffers = max_nb_buffers;
	buffer_mgr.setNbBuffers(nb_buffers);

	buffer_mgr.getFrameDim(frame_dim);
	buffer_mgr.getNbBuffers(nb_buffers);
	buffer_mgr.getNbConcatFrames(nb_concat_frames);
	DEB_TRACE() << "FrameDim " << frame_dim << ", "
	     << "NbBuffers " << nb_buffers << ", "
	     << "NbConcatFrames " << nb_concat_frames;

	TrigMode trig_mode;
	frelon_cam.getTrigMode(trig_mode);
	DEB_TRACE() << "TrigMode " << trig_mode;
	trig_mode = IntTrig;
	frelon_cam.setTrigMode(trig_mode);
	frelon_cam.getTrigMode(trig_mode);
	DEB_TRACE() << "TrigMode " << trig_mode;
	
	int nb_frames;
	frelon_cam.getNbFrames(nb_frames);
	DEB_TRACE() << "NbFrames " << nb_frames;
	nb_frames = nb_buffers;
	frelon_cam.setNbFrames(nb_frames);
	frelon_cam.getNbFrames(nb_frames);
	DEB_TRACE() << "NbFrames " << nb_frames;

	Espia::Acq::StatusType acq_status;
	espia_acq.getStatus(acq_status);
	DEB_TRACE() << "running " << acq_status.running << ", "
	     << "last_frame_nb " << acq_status.last_frame_nb;

	espia_acq.setNbFrames(nb_frames);

	Cond acq_finished;

	FrelonFrameCb frame_cb(nb_frames, acq_finished);
	buffer_mgr.registerFrameCallback(frame_cb);

	espia_acq.start();
	frelon_cam.start();
	acq_finished.wait();
	DEB_TRACE() << "Acq. finished!";
}


int main(int argc, char *argv[])
{
	DEB_GLOBAL_FUNCT();

	try {
		char *endp;

		int espia_nb = 0;
		if ((argc > 1) && argv[1]) {
			espia_nb = strtol(argv[1], &endp, 0);
			if (*endp)
				THROW_HW_ERROR(InvalidValue) 
					<< "Invalid " << DEB_VAR1(argv[1]);
		}

		bool do_reset = (argc > 2) && (string(argv[2]) == "reset");

		if ((argc > 3) && argv[3]) {
			int deb_flags = strtol(argv[3], &endp, 0);
			if (*endp)
				THROW_HW_ERROR(InvalidValue) 
					<< "Invalid " << DEB_VAR1(argv[3]);
			DebParams::setTypeFlags(deb_flags);
		}

		test_frelon(espia_nb, do_reset);
	} catch (Exception e) {
		DEB_ERROR() << "LIMA Exception: " << e;
	} catch (...) {
		DEB_ERROR() << "Unkown exception!";
	}

	return 0;
}
