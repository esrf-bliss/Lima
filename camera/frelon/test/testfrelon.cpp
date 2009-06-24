#include "FrelonCamera.h"
#include "EspiaBufferMgr.h"

#include <iostream>

using namespace lima;
using namespace std;

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
	split_msg(frelon_ser_line, cmd);
	frelon_ser_line.write(cmd);

	Timestamp t0, t1;
	string ans;

	t0 = Timestamp::now();
	frelon_ser_line.readLine(ans);
	t1 = Timestamp::now();
	cout << "Elapsed " << (t1 - t0) << " sec" << endl;
	print_str("Ans", ans);
}

void frelon_read_reg(Frelon::Camera& frelon_cam, Frelon::Reg reg)
{
	Timestamp t0, t1;
	int val;

	print_str("Reg", Frelon::RegStrMap[reg]);
	t0 = Timestamp::now();
	frelon_cam.readRegister(reg, val);
	t1 = Timestamp::now();
	cout << "Elapsed " << (t1 - t0) << " sec" << endl;
	cout << "Val" << " " << val << endl;
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
public:
	FrelonFrameCb(int nb_frames, Cond& acq_finished) 
		: m_nb_frames(nb_frames), m_acq_finished(acq_finished) {}

protected:
	virtual bool newFrameReady(const HwFrameInfoType& frame_info)
	{
		cout << frame_info << "          \r" << flush;
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

void test_frelon(bool do_reset)
{
	Espia::Dev espia_dev(0);
	Espia::Acq espia_acq(espia_dev);
	Espia::BufferMgr espia_buffer_mgr(espia_acq);
	Espia::SerialLine espia_ser_line(espia_dev);
	Frelon::SerialLine frelon_ser_line(espia_ser_line);
	Frelon::Camera frelon_cam(espia_ser_line);

	string msg;

	if (do_reset) {
		cout << "Resetting camera ... " << endl;
		frelon_cam.hardReset();
		cout << "  Done!" << endl;
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
	cout << "Chan " << int(input_chan) << ": ";

	string sep = "";
	for (int i = 0; i < 4; i++) {
		Frelon::InputChan chan = Frelon::InputChan(1 << i);
		if (frelon_cam.isChanActive(chan)) {
			cout << sep << (i + 1);
			sep = "&";
		}
	}
	cout << endl;

	test_sleep();

	cout << "TopLeft:     " << TopLeft     << endl;
	cout << "TopRight:    " << TopRight    << endl;
	cout << "BottomLeft:  " << BottomLeft  << endl;
	cout << "BottomRight: " << BottomRight << endl;

	Bin bin;
	Roi roi;

	frelon_cam.getBin(bin);
	cout << "Bin " << bin << endl;
	bin = 1;
	cout << "Bin " << bin << endl;
	frelon_cam.setBin(bin);
	frelon_cam.getBin(bin);
	cout << "Bin " << bin << endl;
	
	roi = Roi(Point(515, 517), Size(1021, 521));
	frelon_set_roi(frelon_cam, roi);

	frelon_cam.getBin(bin);
	cout << "Bin " << bin << endl;
	bin = 2;
	cout << "Bin " << bin << endl;
	frelon_cam.setBin(bin);
	frelon_cam.getBin(bin);
	cout << "Bin " << bin << endl;
	
	roi = Roi(Point(257, 259), Size(509, 265));
	frelon_set_roi(frelon_cam, roi);

	roi = Roi(Point(57, 59), Size(71, 72));
	frelon_set_roi(frelon_cam, roi);

	bin = 1;
	cout << "Bin " << bin << endl;
	frelon_cam.setBin(bin);
	frelon_cam.getBin(bin);
	cout << "Bin " << bin << endl;

	roi.reset();
	frelon_set_roi(frelon_cam, roi);

	FrameDim frame_dim;
	frelon_cam.getFrameDim(frame_dim);
	frame_dim /= bin;
	int max_nb_buffers = espia_buffer_mgr.getMaxNbBuffers(frame_dim);
	cout << "MaxNbBuffers " << max_nb_buffers << endl;

	int nb_buffers = max_nb_buffers;
	int nb_concat_frames = 1;
	espia_buffer_mgr.allocBuffers(nb_buffers, nb_concat_frames, frame_dim);
	espia_buffer_mgr.getNbBuffers(nb_buffers);
	espia_buffer_mgr.getNbConcatFrames(nb_concat_frames);
	cout << "NbBuffers " << nb_buffers << ", "
	     << "NbConcatFrames " << nb_concat_frames << endl;

	TrigMode trig_mode;
	frelon_cam.getTriggerMode(trig_mode);
	cout << "TrigMode " << trig_mode << endl;
	trig_mode = IntTrig;
	frelon_cam.setTriggerMode(trig_mode);
	frelon_cam.getTriggerMode(trig_mode);
	cout << "TrigMode " << trig_mode << endl;
	
	int nb_frames;
	frelon_cam.getNbFrames(nb_frames);
	cout << "NbFrames " << nb_frames << endl;
	nb_frames = nb_buffers;
	frelon_cam.setNbFrames(nb_frames);
	frelon_cam.getNbFrames(nb_frames);
	cout << "NbFrames " << nb_frames << endl;

	Espia::Acq::StatusType acq_status;
	espia_acq.getStatus(acq_status);
	cout << "running " << acq_status.running << ", "
	     << "last_frame_nb " << acq_status.last_frame_nb << endl;

	espia_acq.setNbFrames(nb_frames);

	Cond acq_finished;

	FrelonFrameCb frame_cb(nb_frames, acq_finished);
	espia_buffer_mgr.registerFrameCallback(frame_cb);

	espia_acq.start();
	frelon_cam.start();
	acq_finished.wait();
	cout << "Acq. finished!" << endl;
}


int main(int argc, char *argv[])
{
	
	try {
		bool do_reset = (argc > 1) && (string(argv[1]) == "reset");
		test_frelon(do_reset);
	} catch (Exception e) {
		cerr << "LIMA Exception: " << e << endl;
	}

	return 0;
}
