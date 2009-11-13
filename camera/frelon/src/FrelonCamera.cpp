#include "FrelonCamera.h"
#include "RegEx.h"
#include "MiscUtils.h"
#include <sstream>

using namespace lima;
using namespace lima::Frelon;
using namespace std;

const double Camera::HorzBinChangeTime = 2.0;
const double Camera::MaxReadoutTime    = 0.7;


Camera::Camera(Espia::SerialLine& espia_ser_line)
	: m_ser_line(espia_ser_line)
{
	DEB_CONSTRUCTOR();

	m_trig_mode = IntTrig;

	string ver;
	getVersion(ver);
	int ser_nb;
	getSerialNb(ser_nb);
}

Camera::~Camera()
{
	DEB_DESTRUCTOR();

}

SerialLine& Camera::getSerialLine()
{
	return m_ser_line;
}

Espia::Dev& Camera::getEspiaDev()
{
	Espia::SerialLine& ser_line = m_ser_line.getEspiaSerialLine();
	Espia::Dev& dev = ser_line.getDev();
	return dev;
}

void Camera::sendCmd(Cmd cmd)
{
	DEB_MEMBER_FUNCT();
	const string& cmd_str = CmdStrMap[cmd];
	DEB_PARAM() << DEB_VAR2(cmd, cmd_str);
	if (cmd_str.empty()) {
		DEB_ERROR() << "Invalid command cmd=" << cmd;
		throw LIMA_HW_EXC(InvalidValue, "Invalid command");
	}

	string resp;
	m_ser_line.sendFmtCmd(cmd_str, resp);
}

void Camera::writeRegister(Reg reg, int val)
{
	DEB_MEMBER_FUNCT();
	const string& reg_str = RegStrMap[reg];
	DEB_PARAM() << DEB_VAR3(reg, reg_str, val);
	if (reg_str.empty()) {
		DEB_ERROR() << "Invalid register reg=" << reg;
		throw LIMA_HW_EXC(InvalidValue, "Invalid register");
	}

	ostringstream cmd;
	cmd << reg_str << val;
	string resp;
	m_ser_line.sendFmtCmd(cmd.str(), resp);
}

void Camera::readRegister(Reg reg, int& val)
{
	DEB_MEMBER_FUNCT();

	const string& reg_str = RegStrMap[reg];
	DEB_PARAM() << DEB_VAR2(reg, reg_str);
	if (reg_str.empty()) {
		DEB_ERROR() << "Invalid register reg=" << reg;
		throw LIMA_HW_EXC(InvalidValue, "Invalid register");
	}

	string resp;
	m_ser_line.sendFmtCmd(reg_str + "?", resp);
	istringstream is(resp);
	is >> val;

	DEB_RETURN() << DEB_VAR1(val);
}

void Camera::hardReset()
{
	DEB_MEMBER_FUNCT();
	Espia::Dev& dev = getEspiaDev();
	dev.resetLink();

	DEB_TRACE() << "Reseting the camera";
	sendCmd(Reset);
}

void Camera::getVersion(string& ver)
{
	DEB_MEMBER_FUNCT();
	string cmd = RegStrMap[Version] + "?";
	m_ser_line.sendFmtCmd(cmd, ver);
	DEB_RETURN() << DEB_VAR1(ver);
}

void Camera::getComplexSerialNb(int& complex_ser_nb)
{
	DEB_MEMBER_FUNCT();
	readRegister(CompSerNb, complex_ser_nb);
	DEB_RETURN() << DEB_VAR1(DEB_HEX(complex_ser_nb));
}

void Camera::getSerialNbParam(SerNbParam param, int& val)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(DEB_HEX(param));
	int complex_ser_nb;
	getComplexSerialNb(complex_ser_nb);
	val = complex_ser_nb & int(param);
}

void Camera::getSerialNb(int& ser_nb)
{
	DEB_MEMBER_FUNCT();
	getSerialNbParam(SerNb, ser_nb);
	DEB_RETURN() << DEB_VAR1(ser_nb);
}

void Camera::isFrelon2k16(bool& is_frelon_2k16)
{
	DEB_MEMBER_FUNCT();
	int frelon_2k16;
	getSerialNbParam(F2k16, frelon_2k16);
	is_frelon_2k16 = bool(frelon_2k16);
	DEB_RETURN() << DEB_VAR1(is_frelon_2k16);
}

void Camera::isFrelon4M(bool& is_frelon_4m)
{
	DEB_MEMBER_FUNCT();
	int f4m;
	getSerialNbParam(F4M, f4m);
	is_frelon_4m = bool(f4m);
	DEB_RETURN() << DEB_VAR1(is_frelon_4m);
}

void Camera::hasTaper(bool& has_taper)
{
	DEB_MEMBER_FUNCT();
	int taper;
	getSerialNbParam(Taper, taper);
	has_taper = bool(taper);
	DEB_RETURN() << DEB_VAR1(has_taper);
}

void Camera::setChanMode(int chan_mode)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(chan_mode);
	writeRegister(ChanMode, chan_mode);
}

void Camera::getChanMode(int& chan_mode)
{
	DEB_MEMBER_FUNCT();
	readRegister(ChanMode, chan_mode);
	DEB_RETURN() << DEB_VAR1(chan_mode);
}

void Camera::getBaseChanMode(FrameTransferMode ftm, int& base_chan_mode)
{
	DEB_MEMBER_FUNCT();
	base_chan_mode = FTMChanRangeMap[ftm].first;
	DEB_RETURN() << DEB_VAR1(base_chan_mode);
}

void Camera::getInputChanMode(FrameTransferMode ftm, InputChan input_chan,
			      int& chan_mode)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(ftm, DEB_HEX(input_chan));

	getBaseChanMode(ftm, chan_mode);
	const InputChanList& chan_list = FTMInputChanListMap[ftm];
	InputChanList::const_iterator it;
	it = find(chan_list.begin(), chan_list.end(), input_chan);
	if (it == chan_list.end())
		throw LIMA_HW_EXC(InvalidValue, "Invalid input channel");
	chan_mode += it - chan_list.begin();

	DEB_RETURN() << DEB_VAR1(chan_mode);
}

void Camera::setInputChan(InputChan input_chan)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(DEB_HEX(input_chan));

	FrameTransferMode ftm;
	getFrameTransferMode(ftm);
	int chan_mode;
	getInputChanMode(ftm, input_chan, chan_mode);
	setChanMode(chan_mode);
}

void Camera::getInputChan(InputChan& input_chan)
{
	DEB_MEMBER_FUNCT();

	FrameTransferMode ftm;
	getFrameTransferMode(ftm);
	int chan_mode, base_chan_mode;
	getChanMode(chan_mode);
	getBaseChanMode(ftm, base_chan_mode);
	input_chan = FTMInputChanListMap[ftm][chan_mode - base_chan_mode];

	DEB_RETURN() << DEB_VAR1(DEB_HEX(input_chan));
}

void Camera::setFrameTransferMode(FrameTransferMode ftm)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(ftm);
	
	InputChan input_chan;
	getInputChan(input_chan);
	int chan_mode;
	getInputChanMode(ftm, input_chan, chan_mode);
	setChanMode(chan_mode);
}

void Camera::getFrameTransferMode(FrameTransferMode& ftm)
{
	DEB_MEMBER_FUNCT();

	int chan_mode;
	getChanMode(chan_mode);

	FTMChanRangeMapType::const_iterator it, end = FTMChanRangeMap.end();
	for (it = FTMChanRangeMap.begin(); it != end; ++it) {
		ftm = it->first;
		const ChanRange& range = it->second;
		if ((chan_mode >= range.first) && (chan_mode < range.second)) {
			DEB_RETURN() << DEB_VAR1(ftm);
			return;
		}
	}

	throw LIMA_HW_EXC(Error, "Invalid chan mode");
}

void Camera::getFrameDim(FrameDim& frame_dim)
{
	DEB_MEMBER_FUNCT();

	frame_dim = MaxFrameDim;
	FrameTransferMode ftm;
	getFrameTransferMode(ftm);
	if (ftm == FTM)
		frame_dim /= Point(1, 2);

	DEB_RETURN() << DEB_VAR1(frame_dim);
}

void Camera::setFlipMode(int flip_mode)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(flip_mode);
	writeRegister(Flip, flip_mode);
}

void Camera::getFlipMode(int& flip_mode)
{
	DEB_MEMBER_FUNCT();
	readRegister(Flip, flip_mode);
	DEB_RETURN() << DEB_VAR1(flip_mode);
}

void Camera::setFlip(const Point& flip)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(flip);
	int flip_mode = (bool(flip.x) << 1) | (bool(flip.y) << 0);
	setFlipMode(flip_mode);
}

void Camera::getFlip(Point& flip)
{
	DEB_MEMBER_FUNCT();

	int flip_mode;
	getFlipMode(flip_mode);
	flip.x = (flip_mode >> 1) & 1;
	flip.y = (flip_mode >> 0) & 1;

	DEB_RETURN() << DEB_VAR1(flip);
}

void Camera::checkBin(Bin& bin)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(bin);
	
	int bin_x = min(bin.getX(), int(MaxBinX));
	int bin_y = min(bin.getY(), int(MaxBinY));
	bin = Bin(bin_x, bin_y);

	DEB_RETURN() << DEB_VAR1(bin);
}

void Camera::setBin(const Bin& bin)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(bin);
	
	if ((bin.getX() > 8) || (bin.getY() > 1024)) {
		DEB_ERROR() << "Invalid bin: " << bin << ". Must be <= 8x1024";
		throw LIMA_HW_EXC(InvalidValue, "Bin must be <= 8x1024");
	}

	Bin curr_bin;
	getBin(curr_bin);
	DEB_TRACE() << DEB_VAR1(curr_bin);
	if (bin == curr_bin) 
		return;

	DEB_TRACE() << "Reseting Roi";
	Roi roi;
	setRoi(roi);

	writeRegister(BinHorz, bin.getX());
	DEB_TRACE() << "Sleeping " << DEB_VAR1(HorzBinChangeTime);
	Sleep(HorzBinChangeTime);
	writeRegister(BinVert, bin.getY());
}

void Camera::getBin(Bin& bin)
{
	DEB_MEMBER_FUNCT();
	int bin_x, bin_y;
	readRegister(BinHorz, bin_x);
	readRegister(BinVert, bin_y);
	bin = Bin(bin_x, bin_y);
	DEB_RETURN() << DEB_VAR1(bin);
}

void Camera::setRoiMode(RoiMode roi_mode)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(roi_mode);

	bool roi_hw   = (roi_mode == Slow) || (roi_mode == Fast);
	bool roi_fast = (roi_mode == Fast) || (roi_mode == Kinetic);
	bool roi_kin  = (roi_mode == Kinetic);
	DEB_TRACE() << DEB_VAR3(roi_hw, roi_fast, roi_kin);

	writeRegister(RoiEnable,  roi_hw);
	writeRegister(RoiFast,    roi_fast);
	writeRegister(RoiKinetic, roi_kin);
}

void Camera::getRoiMode(RoiMode& roi_mode)
{
	DEB_MEMBER_FUNCT();

	int roi_hw, roi_fast, roi_kin;
	readRegister(RoiEnable,  roi_hw);
	readRegister(RoiFast,    roi_fast);
	readRegister(RoiKinetic, roi_kin);
	DEB_TRACE() << DEB_VAR3(roi_hw, roi_fast, roi_kin);

	if (roi_fast && roi_kin)
		roi_mode = Kinetic;
	else if (roi_fast && roi_hw)
		roi_mode = Fast;
	else if (roi_hw)
		roi_mode = Slow;
	else
		roi_mode = None;

	DEB_RETURN() << DEB_VAR1(roi_mode);
}

void Camera::getMirror(Point& mirror)
{
	DEB_MEMBER_FUNCT();
	mirror.x = isChanActive(Chan12) || isChanActive(Chan34);
	mirror.y = isChanActive(Chan13) || isChanActive(Chan24);
	DEB_RETURN() << DEB_VAR1(mirror);
}

void Camera::getNbChan(Point& nb_chan)
{
	DEB_MEMBER_FUNCT();
	getMirror(nb_chan);
	nb_chan += 1;
	DEB_RETURN() << DEB_VAR1(nb_chan);
}

void Camera::getCcdSize(Size& ccd_size)
{
	DEB_MEMBER_FUNCT();
	FrameDim frame_dim;
	getFrameDim(frame_dim);
	ccd_size = frame_dim.getSize();
	DEB_RETURN() << DEB_VAR1(ccd_size);
}

void Camera::getChanSize(Size& chan_size)
{
	DEB_MEMBER_FUNCT();
	getCcdSize(chan_size);
	Point nb_chan;
	getNbChan(nb_chan);
	chan_size /= nb_chan;
	DEB_RETURN() << DEB_VAR1(chan_size);
}

void Camera::xformChanCoords(const Point& point, Point& chan_point, 
			     Corner& ref_corner)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(point);

	Size chan_size;
	getChanSize(chan_size);

	bool good_xchan = isChanActive(Chan1) || isChanActive(Chan3);
	bool good_ychan = isChanActive(Chan1) || isChanActive(Chan2);
	Point flip;
	getFlip(flip);
	XBorder ref_xb = (bool(flip.x) == !good_xchan) ? Left : Right;
	YBorder ref_yb = (bool(flip.y) == !good_ychan) ? Top  : Bottom;

	Point mirror;
	getMirror(mirror);
	if (mirror.x)
		ref_xb = (point.x < chan_size.getWidth())  ? Left : Right;
	if (mirror.y)
		ref_yb = (point.y < chan_size.getHeight()) ? Top  : Bottom;

	ref_corner.set(ref_xb, ref_yb);

	Size ccd_size;
	getCcdSize(ccd_size);

	chan_point = ccd_size.getCornerCoords(point, ref_corner);
	DEB_RETURN() << DEB_VAR2(chan_point, ref_corner);
}

void Camera::getImageRoi(const Roi& chan_roi, Roi& image_roi)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(chan_roi);

	Point img_tl, chan_tl = chan_roi.getTopLeft();
	Point img_br, chan_br = chan_roi.getBottomRight();
	Corner c_tl, c_br;
	xformChanCoords(chan_tl, img_tl, c_tl);
	xformChanCoords(chan_br, img_br, c_br);

	Roi unbinned_roi(img_tl, img_br);
	Bin bin;
	getBin(bin);
	image_roi = unbinned_roi.getBinned(bin);

	DEB_RETURN() << DEB_VAR1(image_roi);
}

void Camera::getFinalRoi(const Roi& image_roi, const Point& roi_offset,
			 Roi& final_roi)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(image_roi, roi_offset);

	Point tl = image_roi.getTopLeft() + roi_offset;
	Point nb_chan;
	getNbChan(nb_chan);
	Size size = image_roi.getSize() * nb_chan;
	final_roi = Roi(tl, size);

	DEB_RETURN() << DEB_VAR1(final_roi);
}

void Camera::getChanRoi(const Roi& image_roi, Roi& chan_roi)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(image_roi);

	Bin bin;
	getBin(bin);
	Roi unbinned_roi = image_roi.getUnbinned(bin);
	Point chan_tl, img_tl = unbinned_roi.getTopLeft();
	Point chan_br, img_br = unbinned_roi.getBottomRight();
	Corner c_tl, c_br;
	xformChanCoords(img_tl, chan_tl, c_tl);
	xformChanCoords(img_br, chan_br, c_br);

	chan_roi.setCorners(chan_tl, chan_br);
	chan_tl = chan_roi.getTopLeft();
	chan_br = chan_roi.getBottomRight();

	bool two_xchan = (c_tl.getX() != c_br.getX());
	bool two_ychan = (c_tl.getY() != c_br.getY());
	DEB_TRACE() << DEB_VAR2(two_xchan, two_ychan);

	Size chan_size;
	getChanSize(chan_size);
	if (two_xchan)
		chan_br.x = chan_size.getWidth() - 1;
	if (two_ychan)
		chan_br.y = chan_size.getHeight() - 1;

	chan_roi.setCorners(chan_tl, chan_br);
	DEB_RETURN() << DEB_VAR1(chan_roi);
}

void Camera::getImageRoiOffset(const Roi& req_roi, const Roi& image_roi,
			       Point& roi_offset)
{
	Point virt_tl = image_roi.getTopLeft();

	Size ccd_size, image_size;
	getCcdSize(ccd_size);
	image_size = image_roi.getSize();
	Point image_br = image_roi.getBottomRight() + 1;

	Point mirror_tl = ccd_size - image_br - image_size;
	Point req_tl = req_roi.getTopLeft();
	if (req_tl.x >= image_br.x)
		virt_tl.x = mirror_tl.x;
	if (req_tl.y >= image_br.y)
		virt_tl.y = mirror_tl.y;

	roi_offset = virt_tl - image_roi.getTopLeft();
}

void Camera::checkRoiMode(const Roi& roi)
{
	RoiMode roi_mode;
	getRoiMode(roi_mode);
	if (!roi.isActive())
		roi_mode = None;
	else if (roi_mode == None)
		roi_mode = Slow;
	setRoiMode(roi_mode);
}

void Camera::checkRoi(const Roi& set_roi, Roi& hw_roi)
{
	if (!set_roi.isActive()) {
		hw_roi = set_roi;
		return;
	}

	Roi chan_roi;
	Point roi_offset;
	processSetRoi(set_roi, hw_roi, chan_roi, roi_offset);
}

void Camera::processSetRoi(const Roi& set_roi, Roi& hw_roi, 
			   Roi& chan_roi, Point& roi_offset)
{
	Roi aligned_roi = set_roi;
	aligned_roi.alignCornersTo(Point(32, 1), Ceil);
	getChanRoi(aligned_roi, chan_roi);
	Roi image_roi;
	getImageRoi(chan_roi, image_roi);
	getImageRoiOffset(set_roi, image_roi, roi_offset);
	getFinalRoi(image_roi, roi_offset, hw_roi);
}

void Camera::setRoi(const Roi& set_roi)
{
	checkRoiMode(set_roi);
	if (!set_roi.isActive())
		return;

	Roi hw_roi, chan_roi;
	Point roi_offset;
	processSetRoi(set_roi, hw_roi, chan_roi, roi_offset);

	Point tl  = chan_roi.getTopLeft();
	Size size = chan_roi.getSize();

	writeRegister(RoiPixelBegin, tl.x);
	writeRegister(RoiPixelWidth, size.getWidth());
	writeRegister(RoiLineBegin,  tl.y);
	writeRegister(RoiLineWidth,  size.getHeight());

	m_roi_offset = roi_offset;
}

void Camera::getRoi(Roi& hw_roi)
{
	hw_roi.reset();

	RoiMode roi_mode;
	getRoiMode(roi_mode);
	if (roi_mode == None)
		return;

	int rpb, rpw, rlb, rlw;
	readRegister(RoiPixelBegin, rpb);
	readRegister(RoiPixelWidth, rpw);
	readRegister(RoiLineBegin,  rlb);
	readRegister(RoiLineWidth,  rlw);

	Roi chan_roi(Point(rpb, rlb), Size(rpw, rlw));
	Roi image_roi;
	getImageRoi(chan_roi, image_roi);
	getFinalRoi(image_roi, m_roi_offset, hw_roi);
}

void Camera::setTrigMode(TrigMode trig_mode)
{
	m_trig_mode = trig_mode;
	setNbFrames(m_nb_frames);
}

void Camera::getTrigMode(TrigMode& trig_mode)
{
	trig_mode = m_trig_mode;
}

void Camera::setTimeUnitFactor(TimeUnitFactor time_unit_factor)
{
	int time_unit = int(time_unit_factor);
	writeRegister(TimeUnit, time_unit);
}

void Camera::getTimeUnitFactor(TimeUnitFactor& time_unit_factor)
{
	int time_unit;
	readRegister(TimeUnit, time_unit);
	time_unit_factor = TimeUnitFactor(time_unit);
}

void Camera::setExpTime(double exp_time)
{
	bool ok = false;
	int exp_val;
	TimeUnitFactor seq_clist[] = { Microseconds, Milliseconds };
	TimeUnitFactor *it, *end = C_LIST_END(seq_clist);
	for (it = seq_clist; it != end; ++it) {
		double factor = TimeUnitFactorMap[*it];
		exp_val = int(exp_time / factor + 0.1);
		ok = (exp_val <= MaxRegVal);
		if (ok)
			break;
	}
	if (!ok)
		throw LIMA_HW_EXC(InvalidValue, "Exposure time too high");

	TimeUnitFactor time_unit_factor = (exp_val == 0) ? Milliseconds : *it;
	setTimeUnitFactor(time_unit_factor);
	writeRegister(ExpTime, exp_val);
}

void Camera::getExpTime(double& exp_time)
{
	TimeUnitFactor time_unit_factor;
	getTimeUnitFactor(time_unit_factor);
	int exp_val;
	readRegister(ExpTime, exp_val);
	exp_time = exp_val * TimeUnitFactorMap[time_unit_factor];
}

void Camera::setLatTime(double lat_time)
{
	int lat_val = int(lat_time / TimeUnitFactorMap[Milliseconds] + 0.1);
	writeRegister(LatencyTime, lat_val);
}

void Camera::getLatTime(double& lat_time)
{
	int lat_val;
	readRegister(LatencyTime, lat_val);
	lat_time = lat_val * TimeUnitFactorMap[Milliseconds];
}

void Camera::setNbFrames(int nb_frames)
{
	TrigMode trig_mode;
	getTrigMode(trig_mode);
	int cam_nb_frames = (trig_mode == ExtTrigMult) ? 1 : nb_frames;
	writeRegister(NbFrames, cam_nb_frames);
	m_nb_frames = nb_frames;
}

void Camera::getNbFrames(int& nb_frames)
{
	nb_frames = m_nb_frames;
}

void Camera::getStatus(Status& status)
{
	Espia::Dev& dev = getEspiaDev();
	int ccd_status;
	dev.getCcdStatus(ccd_status);
	status = Status(ccd_status);
}

bool Camera::waitStatus(Status& status, double timeout)
{
	Timestamp end;
	if (timeout > 0)
		end = Timestamp::now() + Timestamp(timeout);

	bool good_status = false;
	Status curr_status;
	while (!good_status && !end.isSet() || (Timestamp::now() < end)) {
		getStatus(curr_status);
		good_status = ((curr_status & status) == status);
	}

	status = curr_status;
	return good_status;
}

void Camera::start()
{
	TrigMode trig_mode;
	getTrigMode(trig_mode);
	if (trig_mode == IntTrig)
		sendCmd(Start);
}

void Camera::stop()
{
	TrigMode trig_mode;
	getTrigMode(trig_mode);
	if (trig_mode != ExtGate)
		sendCmd(Stop);

	Status status = Wait;
	waitStatus(status);

	FrameTransferMode ftm;
	getFrameTransferMode(ftm);
	if (ftm == FTM)
		Sleep(MaxReadoutTime);
}

