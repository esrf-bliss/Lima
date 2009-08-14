
#include "CtImage.h"

using namespace lima;

// ----------------------------------------------------------------------------
// CLASS CtSwBinRoi
// ----------------------------------------------------------------------------
CtSwBinRoi::CtSwBinRoi(Size& size)
{
	m_max_size= size;
	m_max_roi= Roi(Point(0,0), m_max_size);
}

void CtSwBinRoi::setMaxSize(Size& size)
{
	m_max_size= size;
	m_max_roi.setSize(m_max_size / m_bin);

	if (!m_max_roi.containsPoint(m_roi.getTopLeft())) {
		m_roi.reset();
	}
	else if (!m_max_roi.containsPoint(m_roi.getBottomRight())) {
		m_roi.setCorners(m_roi.getTopLeft(), m_max_roi.getBottomRight());
	}
}

void CtSwBinRoi::setBin(Bin& bin)
{
	if (bin != m_bin) {
		m_roi= m_roi.getUnbinned(m_bin);
		m_bin= bin;
		if (!m_bin.isOne())
			m_roi= m_roi.getBinned(m_bin);
		m_max_roi.setSize(m_max_size / m_bin);
	}
}

void CtSwBinRoi::setRoi(Roi& roi)
{
	if (roi.isEmpty())
		throw LIMA_CTL_EXC(InvalidValue, "Hardware roi is empty");
	if (!m_max_roi.containsRoi(roi))
		throw LIMA_CTL_EXC(InvalidValue, "Roi out of limts");
	m_roi= roi;
}

const Size& CtSwBinRoi::getSize()
{
	if (m_roi.isEmpty())
		m_size= m_max_size / m_bin;
	else	m_size= Size(m_roi.getSize());
	return m_size;
}

void CtSwBinRoi::resetBin()
{
	m_roi= m_roi.getUnbinned(m_bin);
	m_bin.reset();
}

void CtSwBinRoi::resetRoi()
{
	m_roi.reset();
}

void CtSwBinRoi::reset()
{
	m_bin.reset();
	m_roi.reset();
}
	
// ----------------------------------------------------------------------------
// CLASS CtSwBinRoi
// ----------------------------------------------------------------------------

CtHwBinRoi::CtHwBinRoi(HwInterface *hw, CtSwBinRoi *sw_bin_roi, Size& size)
	: m_sw_bin_roi(sw_bin_roi)
{
	m_has_bin= hw->getHwCtrlObj(m_hw_bin);
	m_has_roi= hw->getHwCtrlObj(m_hw_roi);
	// m_has_hwflip= hw->getHwCtrlObj(m_hw_flip);

	m_max_size= size;
	m_max_roi= Roi(Point(0,0), m_max_size);
	m_size= m_max_size;
}

CtHwBinRoi::~CtHwBinRoi()
{
}

void CtHwBinRoi::setMaxSize(Size& size)
{
	m_max_size= size;
	m_max_roi.setSize(m_max_size / m_bin);

	if (!m_max_roi.containsPoint(m_roi.getTopLeft())) {
		m_roi.reset();
	}
	else if (!m_max_roi.containsPoint(m_roi.getBottomRight())) {
		m_roi.setCorners(m_roi.getTopLeft(), m_max_roi.getBottomRight());
	}
	_updateSize();
}

void CtHwBinRoi::setBin(Bin& bin, bool round)
{
	if (!m_has_bin) {
		if (!round)
			throw LIMA_CTL_EXC(NotSupported, "No hardware binning available");
	}
	else {
		Bin set_bin(bin);
		if (!set_bin.isOne())
			m_hw_bin->checkBin(set_bin);
		if ((!round)&&(set_bin!=bin))
			throw LIMA_CTL_EXC(InvalidValue, "Given hardware binning not possible");
		if (set_bin != m_bin) {
			m_roi= m_roi.getUnbinned(m_bin);
			m_bin= set_bin;
			if (!m_bin.isOne())
				m_roi= m_roi.getBinned(m_bin);
			m_max_roi.setSize(m_max_size / m_bin);
			_updateSize();
		}
		bin= set_bin;
	}
}

void CtHwBinRoi::setRoi(Roi& roi, bool round)
{
	if (roi.isEmpty())
		throw LIMA_CTL_EXC(InvalidValue, "Hardware roi is empty");
	if (!m_max_roi.containsRoi(roi))
		throw LIMA_CTL_EXC(InvalidValue, "Roi out of limts");
	if (!m_has_roi) {
		if (!round)
			throw LIMA_CTL_EXC(NotSupported, "No hardware roi available");
	}
	else {
		Roi set_roi(roi);
		m_hw_roi->checkRoi(roi, set_roi);
		if ((!round)&&(set_roi!=roi))
			throw LIMA_CTL_EXC(InvalidValue, "Given hardware roi not possible");
		if (set_roi != m_roi) {
			m_roi= set_roi;
			_updateSize();
		}
		roi= set_roi;
	}
}

void CtHwBinRoi::_updateSize()
{
	Size o_size(m_size);

	if (m_roi.isEmpty())
		m_size= m_max_size / m_size;
	else	m_size= Size(m_roi.getSize());

	if (o_size != m_size)
		m_sw_bin_roi->setMaxSize(m_size);
}

void CtHwBinRoi::resetBin()
{
	if (m_has_roi && !m_roi.isEmpty()) {
		Roi new_roi= m_roi.getUnbinned(m_bin);
		m_bin.reset();
		setRoi(new_roi, true);
	}
	else {
		m_bin.reset();
	}
}

void CtHwBinRoi::resetRoi()
{
	m_roi.reset();
	_updateSize();
}

void CtHwBinRoi::reset()
{
	m_bin.reset();
	m_roi.reset();
	m_size= m_max_size;
}
	
	
// ----------------------------------------------------------------------------
// CLASS CtImage
// ----------------------------------------------------------------------------

CtImage::CtImage(HwInterface *hw)
	: m_mode(HardAndSoft)
{
	if (!hw->getHwCtrlObj(m_hw_det))
		throw LIMA_CTL_EXC(Error, "Cannot get detector info object");

	m_hw_det->getMaxImageSize(m_max_size);
	m_hw_det->getCurrImageType(m_img_type);

	m_sw= new CtSwBinRoi(m_max_size);
	m_hw= new CtHwBinRoi(hw, m_sw, m_max_size);
}

CtImage::~CtImage()
{
	delete m_hw;
	delete m_sw;
}

void CtImage::getMaxImageSize(Size& size) const
{
	size= m_max_size;
}

void CtImage::setMaxImage(Size size, ImageType type)
{
	m_max_size= size;
	m_img_type= type;

	m_hw->setMaxSize(m_max_size);
}

void CtImage::getImageType(ImageType& type) const
{
	type= m_img_type;
}

void CtImage::getImageDim(FrameDim& dim) const
{
	dim= FrameDim(m_sw->getSize(), m_img_type);
}

void CtImage::getHwImageDim(FrameDim& dim) const
{
	dim= FrameDim(m_hw->getSize(), m_img_type);
}

void CtImage::getSoft(CtSwBinRoi *& soft) const
{
	soft= m_sw;
}

void CtImage::getHard(CtHwBinRoi *& hard) const
{
	hard= m_hw;
}

void CtImage::setMode(ImageOpMode mode)
{
	if (mode != m_mode) {
		if (mode==HardOnly)
			m_sw->reset();
		if (mode==SoftOnly)
			m_hw->reset();
	}
	m_mode= mode;
}

void CtImage::getMode(ImageOpMode& mode) const
{
	mode= m_mode;
}

void CtImage::setBin(Bin bin)
{
	switch (m_mode) {
		case SoftOnly:
			m_sw->setBin(bin);
			break;
		case HardOnly:
			m_hw->setBin(bin, false);
			break;
		case HardAndSoft:
			_setHSBin(bin);
			break;
	}
}
		
void CtImage::setRoi(Roi roi)
{
	switch (m_mode) {
		case SoftOnly:
			m_sw->setRoi(roi);
			break;
		case HardOnly:
			m_hw->setRoi(roi, false);
			break;
		case HardAndSoft:
			_setHSRoi(roi);
			break;
	}
}

void CtImage::_setHSBin(Bin bin) {
	if (m_hw->hasBinCapability()) {
		Bin set_hw_bin(bin);
		m_hw->setBin(set_hw_bin, true);
		if (set_hw_bin == bin) {
			m_sw->resetBin();
		} else {
			Bin set_sw_bin= bin / set_hw_bin;
			m_sw->setBin(set_sw_bin);
		}
	} else {
		m_sw->setBin(bin);
	}
}


void CtImage::_setHSRoi(Roi roi) {
	if (m_hw->hasRoiCapability()) {	
		Roi roi_unbin, roi_by_hw, roi_set_hw, roi_by_sw;
		Bin bin_total, bin_by_hw, bin_by_sw;

		bin_by_hw= m_hw->getBin();
		bin_by_sw= m_sw->getBin();
		bin_total= bin_by_hw * bin_by_sw;

		roi_unbin= roi.getUnbinned(bin_total);
		roi_by_hw= roi_unbin.getBinned(bin_by_hw);
		roi_set_hw= roi_by_hw;

		m_hw->setRoi(roi_set_hw, true);
		if (roi_set_hw==roi_by_hw) {
			m_sw->resetRoi();
		} else {
			roi_by_sw= roi_set_hw.subRoiAbs2Rel(roi_by_hw);
			roi_by_sw= roi_by_sw.getBinned(bin_by_sw);
			m_sw->setRoi(roi_by_sw);
		}
	} else {
		m_sw->setRoi(roi);
	}
}

void CtImage::resetBin() {
	m_hw->resetBin();
	m_sw->resetBin();
}

void CtImage::resetRoi() {
	m_hw->resetRoi();
	m_sw->resetRoi();
}

void CtImage::reset() {
	m_hw->reset();
	m_sw->reset();
}

void CtImage::getBin(Bin& bin) const
{
	if (m_hw->hasBinCapability())
		bin= m_hw->getBin() * m_sw->getBin();
	else	bin= m_sw->getBin();
}

void CtImage::getRoi(Roi& roi) const
{
	if (m_hw->hasRoiCapability()) {
		Roi roi_by_hw= m_hw->getRoi();
		Roi roi_by_sw= m_sw->getRoi();
		Bin bin_by_sw= m_sw->getBin();

		roi_by_sw.getUnbinned(m_sw->getBin());
		Roi roi_total= roi_by_hw.subRoiRel2Abs(roi_by_sw);
		roi= roi_total.getBinned(bin_by_sw);
	} else {
		roi= m_sw->getRoi();
	}
}

