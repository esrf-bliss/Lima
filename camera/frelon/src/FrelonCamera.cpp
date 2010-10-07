#include "FrelonCamera.h"
#include "MiscUtils.h"
#include <sstream>

using namespace lima;
using namespace lima::Frelon;
using namespace std;

const double Camera::UpdateCcdStatusTime = 0.1;
const double Camera::MaxIdleWaitTime = 1.5;


Camera::Camera(Espia::SerialLine& espia_ser_line)
	: m_ser_line(espia_ser_line), m_timing_ctrl(m_model, m_ser_line),
	  m_mis_cb_act(false)
{
	DEB_CONSTRUCTOR();

	sync();
}

Camera::~Camera()
{
	DEB_DESTRUCTOR();

}

void Camera::sync()
{
	DEB_MEMBER_FUNCT();

	DEB_TRACE() << "Forcing a link reset";
	Espia::Dev& dev = getEspiaDev();
	dev.resetLink();

	DEB_TRACE() << "Synchronizing with the camera";

	m_model.reset();
	m_roi_offset = 0;

	try {
		syncRegs();
	} catch (Exception e) {
		string err_msg = e.getErrMsg();
		DEB_TRACE() << "Error in sync: " << DEB_VAR1(err_msg);
		string timeout_msg = Espia::StrError(SCDXIPCI_ERR_TIMEOUT);
		bool timeout = (err_msg.find(timeout_msg) != string::npos);
		if (!timeout)
			throw;

		THROW_HW_ERROR(Error) << "Serial connection timeout: "
					 "is camera ON and connected?";
	}

	string ver;
	m_model.getFirmware().getVersionStr(ver);
	DEB_ALWAYS() << "Found Frelon " << m_model.getName() 
		     << " #" << m_model.getSerialNb() << ", FW:" << ver;
}

void Camera::syncRegs()
{
	DEB_MEMBER_FUNCT();

	m_ser_line.clearCache();

	string ver;
	getVersionStr(ver);
	m_model.setVersionStr(ver);

	int complex_ser_nb;
	getComplexSerialNb(complex_ser_nb);
	m_model.setComplexSerialNb(complex_ser_nb);

	double exp_time;
	getExpTime(exp_time);
	m_trig_mode = (exp_time == 0) ? ExtGate : IntTrig;

	// A sequencer cmd will send the CCD status byte to the Espia
	m_nb_frames = 1;
	writeRegister(NbFrames, m_nb_frames);
	DEB_TRACE() << "Sleeping " << UpdateCcdStatusTime << " s to allow "
		    << "CCD status byte get updated";
	Sleep(UpdateCcdStatusTime);
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
	if (cmd_str.empty())
		THROW_HW_ERROR(InvalidValue) << "Invalid " 
					     << DEB_VAR1(cmd);

	string resp;
	m_ser_line.sendFmtCmd(cmd_str, resp);
}

void Camera::writeRegister(Reg reg, int val)
{
	DEB_MEMBER_FUNCT();
	m_ser_line.writeRegister(reg, val);
}

void Camera::readRegister(Reg reg, int& val)
{
	DEB_MEMBER_FUNCT();
	m_ser_line.readRegister(reg, val);
}

void Camera::hardReset()
{
	DEB_MEMBER_FUNCT();

	DEB_TRACE() << "Reseting the camera";
	sendCmd(Reset);

	sync();
}

void Camera::getVersionStr(string& ver)
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

Model& Camera::getModel()
{
	return m_model;
}

TimingCtrl& Camera::getTimingCtrl()
{
	return m_timing_ctrl;
}

int Camera::getModesAvail()
{
	DEB_MEMBER_FUNCT();

	int modes_avail;
	if (m_model.hasModesAvail())
		readRegister(CcdModesAvail, modes_avail);
	else if (m_model.getChipType() == Kodak)
		modes_avail = KodakModesAvail;
	else
		modes_avail = AtmelModesAvail;
		
	DEB_RETURN() << DEB_VAR1(modes_avail);
	return modes_avail;
}

string Camera::getInputChanModeName(FrameTransferMode ftm, 
				    InputChan input_chan)
{
	DEB_MEMBER_FUNCT();

	ostringstream os;
	os << ((ftm == FTM) ? "FTM" : "FFM") << "-";
	string sep;
	for (int chan = 1; chan <= 4; chan++) {
		int chan_bit = 1 << (chan - 1);
		if ((input_chan & chan_bit) != 0) {
			os << sep << chan;
			sep = "&";
		}
	}
	string mode_name = os.str();
	DEB_RETURN() << DEB_VAR1(mode_name);
	return mode_name;
}

void Camera::setChanMode(int chan_mode)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(chan_mode);

	int mode_bit = 1 << (chan_mode - 1);
	bool valid_mode = ((getModesAvail() & mode_bit) != 0);
	if (!valid_mode) {
		FrameTransferMode ftm;
		InputChan input_chan;
		calcFTMInputChan(chan_mode, ftm, input_chan);
		string mode_name = getInputChanModeName(ftm, input_chan);

		THROW_HW_ERROR(InvalidValue) 
			<< "Channel mode " << mode_name 
			<< " [" << DEB_VAR1(chan_mode) << "] "
			<< "not supported in " << m_model.getName();
	}
	writeRegister(ChanMode, chan_mode);
}

void Camera::getChanMode(int& chan_mode)
{
	DEB_MEMBER_FUNCT();
	readRegister(ChanMode, chan_mode);
	DEB_RETURN() << DEB_VAR1(chan_mode);
}

void Camera::calcBaseChanMode(FrameTransferMode ftm, int& base_chan_mode)
{
	DEB_MEMBER_FUNCT();
	base_chan_mode = FTMChanRangeMap[ftm].first;
	DEB_RETURN() << DEB_VAR1(base_chan_mode);
}

void Camera::calcChanMode(FrameTransferMode ftm, InputChan input_chan,
			  int& chan_mode)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(ftm, DEB_HEX(input_chan));

	calcBaseChanMode(ftm, chan_mode);
	const InputChanList& chan_list = FTMInputChanListMap[ftm];
	InputChanList::const_iterator it;
	it = find(chan_list.begin(), chan_list.end(), input_chan);
	if (it == chan_list.end())
		THROW_HW_ERROR(InvalidValue) << "Invalid " 
					     << DEB_VAR1(input_chan);
	chan_mode += it - chan_list.begin();

	DEB_RETURN() << DEB_VAR1(chan_mode);
}

void Camera::calcFTMInputChan(int chan_mode, FrameTransferMode& ftm, 
			      InputChan& input_chan)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(chan_mode);

	bool ftm_found = false;
	FTMChanRangeMapType::const_iterator it, end = FTMChanRangeMap.end();
	for (it = FTMChanRangeMap.begin(); it != end; ++it) {
		ftm = it->first;
		const ChanRange& range = it->second;
		if ((chan_mode >= range.first) && (chan_mode < range.second)) {
			ftm_found = true;
			break;
		}
	}
	if (!ftm_found)
		THROW_HW_ERROR(Error) << "Invalid " << DEB_VAR1(chan_mode);

	int base_chan_mode;
	calcBaseChanMode(ftm, base_chan_mode);
	input_chan = FTMInputChanListMap[ftm][chan_mode - base_chan_mode];
	DEB_RETURN() << DEB_VAR2(ftm, DEB_HEX(input_chan));
}

void Camera::getDefInputChan(FrameTransferMode ftm, InputChan& input_chan)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(ftm);

	int modes_avail = getModesAvail();

	bool valid_mode = false;
	InputChanList::const_iterator it, end = DefInputChanList.end();
	for (it = DefInputChanList.begin(); it != end; ++it) {
		input_chan = *it;
		int chan_mode;
		try {
			calcChanMode(ftm, input_chan, chan_mode);
		} catch (...) {
			continue;
		}
		int mode_bit = 1 << (chan_mode - 1);
		valid_mode = ((modes_avail & mode_bit) != 0);
		if (valid_mode)
			break;
	}

	if (!valid_mode) 
		THROW_HW_ERROR(Error) << "Could not find valid " << ftm 
				      << "mode: " << DEB_VAR1(modes_avail);

	DEB_RETURN() << DEB_VAR1(DEB_HEX(input_chan)) 
		     << " [" << getInputChanModeName(ftm, input_chan) << "]";
}

void Camera::setInputChan(InputChan input_chan)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(DEB_HEX(input_chan));

	FrameTransferMode ftm;
	getFrameTransferMode(ftm);
	int chan_mode;
	calcChanMode(ftm, input_chan, chan_mode);
	setChanMode(chan_mode);
}

void Camera::getInputChan(InputChan& input_chan)
{
	DEB_MEMBER_FUNCT();

	FrameTransferMode ftm;
	int chan_mode;
	getChanMode(chan_mode);
	calcFTMInputChan(chan_mode, ftm, input_chan);

	DEB_RETURN() << DEB_VAR1(DEB_HEX(input_chan));
}

void Camera::setFrameTransferMode(FrameTransferMode ftm)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(ftm);
	
	FrameTransferMode prev_ftm;
	getFrameTransferMode(prev_ftm);
	if (ftm == prev_ftm) {
		DEB_TRACE() << "Nothing to do";
		return;
	}

	InputChan input_chan;
	getInputChan(input_chan);
	int chan_mode;
	try {
		calcChanMode(ftm, input_chan, chan_mode);
		setChanMode(chan_mode);
	} catch (...) {
		DEB_TRACE() << DEB_VAR1(DEB_HEX(input_chan)) 
			    << " not available in " << DEB_VAR1(ftm);
		DEB_TRACE() << "  Trying default input channel";
		getDefInputChan(ftm, input_chan);
		calcChanMode(ftm, input_chan, chan_mode);
		setChanMode(chan_mode);
	}

	if (!m_mis_cb_act) 
		return;

	FrameDim frame_dim;
	getFrameDim(frame_dim);
	DEB_TRACE() << "MaxImageSizeChanged: " << DEB_VAR1(frame_dim);
	maxImageSizeChanged(frame_dim.getSize(), frame_dim.getImageType());
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

	THROW_HW_ERROR(Error) << "Invalid " << DEB_VAR1(chan_mode);
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
	writeRegister(FlipMode, flip_mode);
}

void Camera::getFlipMode(int& flip_mode)
{
	DEB_MEMBER_FUNCT();
	readRegister(FlipMode, flip_mode);
	DEB_RETURN() << DEB_VAR1(flip_mode);
}

void Camera::checkFlip(Flip& flip)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(flip);
	DEB_TRACE() << "All standard flip modes are supported";
	DEB_RETURN() << DEB_VAR1(flip);
}

void Camera::setFlip(const Flip& flip)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(flip);
	int flip_mode = (flip.x << 1) | (flip.y << 0);
	setFlipMode(flip_mode);
}

void Camera::getFlip(Flip& flip)
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
	
	if ((bin.getX() > 8) || (bin.getY() > 1024))
		THROW_HW_ERROR(InvalidValue) << "Invalid " << DEB_VAR1(bin)
					     << ". Must be <= 8x1024";

	Bin curr_bin;
	getBin(curr_bin);
	DEB_TRACE() << DEB_VAR1(curr_bin);
	if (bin == curr_bin) 
		return;

	DEB_TRACE() << "Reseting Roi";
	Roi roi;
	setRoi(roi);

	writeRegister(BinHorz, bin.getX());
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

Flip Camera::getMirror()
{
	DEB_MEMBER_FUNCT();

	InputChan curr;
	getInputChan(curr);
	Flip mirror;
	mirror.x = isChanActive(curr, Chan12) || isChanActive(curr, Chan34);
	mirror.y = isChanActive(curr, Chan13) || isChanActive(curr, Chan24);

	DEB_RETURN() << DEB_VAR1(mirror);
	return mirror;
}

Point Camera::getNbChan()
{
	DEB_MEMBER_FUNCT();
	Point nb_chan = Point(getMirror()) + 1;
	DEB_RETURN() << DEB_VAR1(nb_chan);
	return nb_chan;
}

Size Camera::getCcdSize()
{
	DEB_MEMBER_FUNCT();
	FrameDim frame_dim;
	getFrameDim(frame_dim);
	Size ccd_size = frame_dim.getSize();
	DEB_RETURN() << DEB_VAR1(ccd_size);
	return ccd_size;
}

Size Camera::getChanSize()
{
	DEB_MEMBER_FUNCT();
	Size chan_size = getCcdSize() / getNbChan();
	DEB_RETURN() << DEB_VAR1(chan_size);
	return chan_size;
}

Flip Camera::getRoiInsideMirror()
{
	DEB_MEMBER_FUNCT();
	Flip roi_inside_mirror(m_roi_offset.x > 0, m_roi_offset.y > 0);
	DEB_RETURN() << DEB_VAR1(roi_inside_mirror);
	return roi_inside_mirror;
}

void Camera::xformChanCoords(const Point& point, Point& xform_point, 
			     Corner& ref_corner)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(point);

	Flip chan_flip;
	getFlip(chan_flip);
	Flip mirror = getMirror();
	Size chan_size = getChanSize();

	InputChan curr;
	getInputChan(curr);
	bool right  = !isChanActive(curr, Chan1) && !isChanActive(curr, Chan3);
	bool bottom = !isChanActive(curr, Chan1) && !isChanActive(curr, Chan2);
	Flip readout_flip(right, bottom);
	DEB_TRACE() << DEB_VAR2(chan_flip, readout_flip);

	Flip effect_flip = chan_flip & readout_flip;
	DEB_TRACE() << "After flip: " << DEB_VAR1(effect_flip);

	if (mirror.x)
		effect_flip.x = (point.x >= chan_size.getWidth());
	if (mirror.y)
		effect_flip.y = (point.y >= chan_size.getHeight());
	DEB_TRACE() << "After mirror: " << DEB_VAR1(effect_flip);

	ref_corner = effect_flip.getRefCorner();

	Size ccd_size = getCcdSize();
	xform_point = ccd_size.getCornerCoords(point, ref_corner);
	DEB_RETURN() << DEB_VAR2(xform_point, ref_corner);
}

void Camera::calcImageRoi(const Roi& chan_roi, const Flip& roi_inside_mirror,
			  Roi& image_roi, Point& roi_bin_offset)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(chan_roi);

	Point image_tl, chan_tl = chan_roi.getTopLeft();
	Point image_br, chan_br = chan_roi.getBottomRight();
	Corner c_tl, c_br;
	xformChanCoords(chan_tl, image_tl, c_tl);
	xformChanCoords(chan_br, image_br, c_br);
	Roi unbinned_roi(image_tl, image_br);
	DEB_TRACE() << "Before mirror shift " << DEB_VAR1(unbinned_roi);

	Bin bin;
	getBin(bin);
	Size bin_size = Point(bin);
	Point mirr_shift = roi_inside_mirror * (bin_size - 1);
	DEB_TRACE() << DEB_VAR1(mirr_shift);

	image_tl = unbinned_roi.getTopLeft() + mirr_shift;
	unbinned_roi.setTopLeft(image_tl);
	DEB_TRACE() << "Before mirror shift " << DEB_VAR1(unbinned_roi);

	image_roi = unbinned_roi.getBinned(bin);

	image_tl %= bin_size;
	c_tl = roi_inside_mirror.getRefCorner();
	DEB_TRACE() << DEB_VAR2(image_tl, c_tl);

	roi_bin_offset = bin_size.getCornerCoords(image_tl, c_tl) % bin_size;

	DEB_RETURN() << DEB_VAR2(image_roi, roi_bin_offset);
}

void Camera::calcFinalRoi(const Roi& image_roi, const Point& roi_offset,
			  Roi& final_roi)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(image_roi, roi_offset);

	Point tl = image_roi.getTopLeft() + roi_offset;
	Point nb_chan = getNbChan();
	Size size = image_roi.getSize() * nb_chan;
	final_roi = Roi(tl, size);

	DEB_RETURN() << DEB_VAR1(final_roi);
}

void Camera::calcChanRoi(const Roi& image_roi, Roi& chan_roi,
			 Flip& roi_inside_mirror)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(image_roi);

	Bin bin;
	getBin(bin);
	Roi unbinned_roi = image_roi.getUnbinned(bin);
	DEB_TRACE() << DEB_VAR1(unbinned_roi);

	Point chan_tl, image_tl = unbinned_roi.getTopLeft();
	Point chan_br, image_br = unbinned_roi.getBottomRight();
	Corner c_tl, c_br;
	xformChanCoords(image_tl, chan_tl, c_tl);
	xformChanCoords(image_br, chan_br, c_br);

	chan_roi.setCorners(chan_tl, chan_br);
	DEB_TRACE() << "xformChanCoords: " << DEB_VAR3(chan_roi, c_tl, c_br);

	chan_tl = chan_roi.getTopLeft();
	chan_br = chan_roi.getBottomRight();

	bool two_xchan = (c_tl.getX() != c_br.getX());
	bool two_ychan = (c_tl.getY() != c_br.getY());
	DEB_TRACE() << DEB_VAR2(two_xchan, two_ychan);

	Size chan_size = getChanSize();
	if (two_xchan)
		chan_br.x = chan_size.getWidth() - 1;
	if (two_ychan)
		chan_br.y = chan_size.getHeight() - 1;

	chan_roi.setCorners(chan_tl, chan_br);

	roi_inside_mirror.x = (image_tl.x > chan_br.x);
	roi_inside_mirror.y = (image_tl.y > chan_br.y);

	DEB_RETURN() << DEB_VAR2(chan_roi, roi_inside_mirror);
}

void Camera::calcImageRoiOffset(const Roi& req_roi, const Roi& image_roi,
				Point& roi_offset)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(req_roi, image_roi);

	Point virt_tl = image_roi.getTopLeft();

	Size image_size, ccd_size = getCcdSize();
	Bin bin;
	getBin(bin);
	ccd_size /= bin;

	image_size = image_roi.getSize();
	Point image_br = image_roi.getBottomRight();

	Point mirror_tl = ccd_size - (image_br + 1) - image_size;
	Point req_tl = req_roi.getTopLeft();
	if (req_tl.x > image_br.x)
		virt_tl.x = mirror_tl.x;
	if (req_tl.y > image_br.y)
		virt_tl.y = mirror_tl.y;

	roi_offset = virt_tl - image_roi.getTopLeft();
	DEB_RETURN() << DEB_VAR1(roi_offset);
}

void Camera::checkRoiMode(const Roi& roi)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(roi);

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
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(set_roi);

	if (set_roi.isActive()) {
		Roi chan_roi;
		Point roi_offset;
		processSetRoi(set_roi, hw_roi, chan_roi, roi_offset);
	} else 
		hw_roi = set_roi;

	DEB_RETURN() << DEB_VAR1(hw_roi);
}

void Camera::processSetRoi(const Roi& set_roi, Roi& hw_roi, 
			   Roi& chan_roi, Point& roi_offset)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(set_roi);

	Roi aligned_roi = set_roi;
	aligned_roi.alignCornersTo(Point(32, 1), Ceil);
	Flip roi_inside_mirror;
	calcChanRoi(aligned_roi, chan_roi, roi_inside_mirror);
	Roi image_roi;
	Point roi_bin_offset;
	calcImageRoi(chan_roi, roi_inside_mirror, image_roi, roi_bin_offset);
	calcImageRoiOffset(set_roi, image_roi, roi_offset);
	calcFinalRoi(image_roi, roi_offset, hw_roi);

	DEB_RETURN() << DEB_VAR3(hw_roi, chan_roi, roi_offset);
}

void Camera::setRoi(const Roi& set_roi)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(set_roi);

	checkRoiMode(set_roi);
	if (!set_roi.isActive()) {
		DEB_TRACE() << "Roi deactivated";
		return;
	}

	Roi hw_roi, chan_roi;
	Point roi_offset;
	processSetRoi(set_roi, hw_roi, chan_roi, roi_offset);

	writeChanRoi(chan_roi);
	m_roi_offset = roi_offset;
}

void Camera::getRoi(Roi& hw_roi)
{
	DEB_MEMBER_FUNCT();

	hw_roi.reset();

	RoiMode roi_mode;
	getRoiMode(roi_mode);
	if (roi_mode == None)
		return;

	Roi chan_roi, image_roi;
	readChanRoi(chan_roi);

	Flip roi_inside_mirror = getRoiInsideMirror();
	Point roi_bin_offset;
	calcImageRoi(chan_roi, roi_inside_mirror, image_roi, roi_bin_offset);

	calcFinalRoi(image_roi, m_roi_offset, hw_roi);
	DEB_RETURN() << DEB_VAR1(hw_roi);
}

void Camera::writeChanRoi(const Roi& chan_roi)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(chan_roi);
	
	Point tl  = chan_roi.getTopLeft();
	Size size = chan_roi.getSize();
	
	writeRegister(RoiPixelBegin, tl.x);
	writeRegister(RoiPixelWidth, size.getWidth());
	writeRegister(RoiLineBegin,  tl.y);
	writeRegister(RoiLineWidth,  size.getHeight());
}

void Camera::readChanRoi(Roi& chan_roi)
{
	DEB_MEMBER_FUNCT();

	int rpb, rpw, rlb, rlw;
	readRegister(RoiPixelBegin, rpb);
	readRegister(RoiPixelWidth, rpw);
	readRegister(RoiLineBegin,  rlb);
	readRegister(RoiLineWidth,  rlw);

	chan_roi = Roi(Point(rpb, rlb), Size(rpw, rlw));
	DEB_RETURN() << DEB_VAR1(chan_roi);
}


void Camera::setRoiBinOffset(const Point& roi_bin_offset)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(roi_bin_offset);

	RoiMode roi_mode;
	getRoiMode(roi_mode);
	if (roi_mode == None) {
		if (!roi_bin_offset.isNull())
			THROW_HW_ERROR(InvalidValue) << "HW Roi not active";
		return;
	}

	Bin bin;
	getBin(bin);
	Size bin_size = Point(bin);
	Roi valid_offset_range(Point(0), bin_size);
	if (!valid_offset_range.containsPoint(roi_bin_offset)) {
		THROW_HW_ERROR(InvalidValue) << "Invalid unaligned " 
					     << DEB_VAR1(roi_bin_offset);
	} else if (roi_bin_offset.x != 0) {
		THROW_HW_ERROR(InvalidValue) 
			<< "Invalid " << DEB_VAR1(roi_bin_offset) << ". "
			<< "Must be horizontally aligned to " << DEB_VAR1(bin);
	}

	Roi chan_roi;
	readChanRoi(chan_roi);

	Point image_tl, image_br;
	Corner c_tl, c_br;
	xformChanCoords(chan_roi.getTopLeft(),     image_tl, c_tl);
	xformChanCoords(chan_roi.getBottomRight(), image_br, c_br);
	Roi image_roi(image_tl, image_br);
	DEB_TRACE() << DEB_VAR1(image_roi);

	Flip roi_inside_mirror = getRoiInsideMirror();
	Point mirr_shift = roi_inside_mirror * (bin_size - 1);
	DEB_TRACE() << DEB_VAR1(mirr_shift);

	image_tl = image_roi.getTopLeft() + mirr_shift;
	image_tl -= image_tl % bin_size;
	DEB_TRACE() << "After alignment " << DEB_VAR1(image_tl);

	c_tl = roi_inside_mirror.getRefCorner();
	image_tl += roi_bin_offset * c_tl.getDir();
	DEB_TRACE() << "After roi_bin_offset " << DEB_VAR1(image_tl);

	Roi max_chan_roi(Point(0), getChanSize());
	bool ok = max_chan_roi.containsPoint(image_tl);
	if (ok) {
		DEB_TRACE() << "Image top-left is OK";
		image_roi.setTopLeft(image_tl);
		ok = max_chan_roi.containsRoi(image_roi);
	}
	if (!ok)
		THROW_HW_ERROR(InvalidValue) << "Cannot apply requested "
					     << DEB_VAR1(roi_bin_offset);

	Point chan_tl, chan_br;
	xformChanCoords(image_roi.getTopLeft(),     chan_tl, c_tl);
	xformChanCoords(image_roi.getBottomRight(), chan_br, c_br);
	chan_roi = Roi(chan_tl, chan_br);

	writeChanRoi(chan_roi);
}

void Camera::getRoiBinOffset(Point& roi_bin_offset)
{
	DEB_MEMBER_FUNCT();

	RoiMode roi_mode;
	getRoiMode(roi_mode);
	if (roi_mode == None) {
		roi_bin_offset = 0;
		return;
	}

	Roi chan_roi, image_roi;
	readChanRoi(chan_roi);

	Flip roi_inside_mirror = getRoiInsideMirror();
	calcImageRoi(chan_roi, roi_inside_mirror, image_roi, roi_bin_offset);

	DEB_RETURN() << DEB_VAR1(roi_bin_offset);
}


void Camera::setTrigMode(TrigMode trig_mode)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(trig_mode);
	m_trig_mode = trig_mode;
	setNbFrames(m_nb_frames);
}

void Camera::getTrigMode(TrigMode& trig_mode)
{
	DEB_MEMBER_FUNCT();
	trig_mode = m_trig_mode;
	DEB_RETURN() << DEB_VAR1(trig_mode);
}

void Camera::setTimeUnitFactor(TimeUnitFactor time_unit_factor)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(time_unit_factor);
	int time_unit = int(time_unit_factor);
	writeRegister(TimeUnit, time_unit);
}

void Camera::getTimeUnitFactor(TimeUnitFactor& time_unit_factor)
{
	DEB_MEMBER_FUNCT();
	int time_unit;
	readRegister(TimeUnit, time_unit);
	time_unit_factor = TimeUnitFactor(time_unit);
	DEB_RETURN() << DEB_VAR1(time_unit_factor);
}

void Camera::setExpTime(double exp_time)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(exp_time);

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
		THROW_HW_ERROR(InvalidValue) << "Exp. time too high: " 
					     << DEB_VAR1(exp_time);

	TimeUnitFactor time_unit_factor = (exp_val == 0) ? Milliseconds : *it;
	setTimeUnitFactor(time_unit_factor);
	writeRegister(ExpTime, exp_val);
}

void Camera::getExpTime(double& exp_time)
{
	DEB_MEMBER_FUNCT();
	TimeUnitFactor time_unit_factor;
	getTimeUnitFactor(time_unit_factor);
	int exp_val;
	readRegister(ExpTime, exp_val);
	exp_time = exp_val * TimeUnitFactorMap[time_unit_factor];
	DEB_RETURN() << DEB_VAR1(exp_time);
}

void Camera::setShutCloseTime(double shut_time)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(shut_time);
	int shut_val = int(shut_time / TimeUnitFactorMap[Milliseconds] + 0.1);
	writeRegister(ShutCloseTime, shut_val);
}

void Camera::getShutCloseTime(double& shut_time)
{
	DEB_MEMBER_FUNCT();
	int shut_val;
	readRegister(ShutCloseTime, shut_val);
	shut_time = shut_val * TimeUnitFactorMap[Milliseconds];
	DEB_RETURN() << DEB_VAR1(shut_time);
}

void Camera::setLatTime(double lat_time)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(lat_time);
	int lat_val = int(lat_time / TimeUnitFactorMap[Milliseconds] + 0.1);
	writeRegister(LatencyTime, lat_val);
}

void Camera::getLatTime(double& lat_time)
{
	DEB_MEMBER_FUNCT();
	int lat_val;
	readRegister(LatencyTime, lat_val);
	lat_time = lat_val * TimeUnitFactorMap[Milliseconds];
	DEB_RETURN() << DEB_VAR1(lat_time);
}

void Camera::setNbFrames(int nb_frames)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(nb_frames);

	TrigMode trig_mode;
	getTrigMode(trig_mode);
	bool one_frame = (trig_mode == ExtTrigMult) || (trig_mode == ExtGate);
	int cam_nb_frames = one_frame ? 1 : nb_frames;
	writeRegister(NbFrames, cam_nb_frames);
	m_nb_frames = nb_frames;
}

void Camera::getNbFrames(int& nb_frames)
{
	DEB_MEMBER_FUNCT();
	nb_frames = m_nb_frames;
	DEB_RETURN() << DEB_VAR1(nb_frames);
}

void Camera::getStatus(Status& status)
{
	DEB_MEMBER_FUNCT();
	Espia::Dev& dev = getEspiaDev();
	int ccd_status;
	dev.getCcdStatus(ccd_status);
	status = Status(ccd_status);
	DEB_RETURN() << DEB_VAR1(DEB_HEX(status));
}

bool Camera::waitStatus(Status& status, Status mask, double timeout)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR3(DEB_HEX(status), DEB_HEX(mask), timeout);

	Timestamp end;
	if (timeout > 0)
		end = Timestamp::now() + Timestamp(timeout);

	bool good_status = false;
	Status curr_status = Status(0xffff);
	while (!good_status) {
		if (end.isSet() && (Timestamp::now() >= end)) {
			DEB_WARNING() << "Timeout waiting for " 
				      << DEB_VAR2(DEB_HEX(status), 
						  DEB_HEX(curr_status));
			break;
		}

		getStatus(curr_status);
		good_status = ((curr_status & mask) == status);
	}

	status = curr_status;
	DEB_RETURN() << DEB_VAR2(DEB_HEX(status), good_status);
	return good_status;
}

void Camera::start()
{
	DEB_MEMBER_FUNCT();

	TrigMode trig_mode;
	getTrigMode(trig_mode);
	if (trig_mode == IntTrig) {
		DEB_TRACE() << "Starting camera by software";
		sendCmd(Start);
	}
}

void Camera::stop()
{
	DEB_MEMBER_FUNCT();

	Status status;
	getStatus(status);
	if (status == Wait)
		return;

	TrigMode trig_mode;
	getTrigMode(trig_mode);
	if (trig_mode != ExtGate) {
		int cam_nb_frames;
		readRegister(NbFrames, cam_nb_frames);
		if (cam_nb_frames > 1) {
			DEB_TRACE() << "Aborting current acquisition";
			sendCmd(Stop);
		}
	}

	DEB_TRACE() << "Waiting for camera to become idle";
	status = Wait;
	waitStatus(status, StatusMask, MaxIdleWaitTime);
}

void Camera::setMaxImageSizeCallbackActive(bool cb_active)
{
	m_mis_cb_act = cb_active;
}
