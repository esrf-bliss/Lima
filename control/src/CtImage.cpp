
#include "CtImage.h"

using namespace lima;

// ----------------------------------------------------------------------------
// CLASS SwBinRoi
// ----------------------------------------------------------------------------
SwBinRoi::SwBinRoi(Size& size)
	: m_size(), m_bin(), m_roi(), m_max_roi()
{
	m_max_roi= Roi(Point(0,0), size);
	setMaxSize(size);
}

void SwBinRoi::setMaxSize(Size& size)
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

void SwBinRoi::setBin(Bin& bin)
{
	if (bin != m_bin) {
		m_roi= m_roi.getUnbinned(m_bin);
		m_bin= bin;
		if (!m_bin.isOne())
			m_roi= m_roi.getBinned(m_bin);
		m_max_roi.setSize(m_max_size / m_bin);
	}
}

void SwBinRoi::setRoi(Roi& roi)
{
	if (roi != m_roi) {
		if (m_max_roi.containsRoi(roi)) {
			m_roi= roi;
		}
		else {
			throw LIMA_CTL_EXC(InvalidValue, "Roi out of limts");
		}
	}
}

const Size& SwBinRoi::getSize()
{
	if (m_roi.isEmpty())
		m_size= m_max_size / m_size;
	else	m_size= Size(m_roi.getSize());
	return m_size;
}

// ----------------------------------------------------------------------------
// CLASS CtImage
// ----------------------------------------------------------------------------

CtImage::CtImage(HwInterface *hw)
	: m_mode(HardAndSoft)
{
	if (!hw->getHwCtrlObj(m_hw_det))
		throw LIMA_CTL_EXC(Error, "Cannot get detector info object");

	m_has_hw_bin= hw->getHwCtrlObj(m_hw_bin);
	// m_has_hwroi= hw->getHwCtrlObj(m_hw_roi);
	m_has_hw_roi= false;
	// m_has_hwflip= hw->getHwCtrlObj(m_hw_flip);
	m_has_hw_flip= false;

	m_hw_det->getMaxImageSize(m_max_size);
	m_hw_det->getCurrImageType(m_img_type);

	m_sw= new SwBinRoi(m_max_size);

	m_hw_size= m_max_size;
	m_sw_size= m_max_size;
}

CtImage::~CtImage()
{
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

	m_sw->setMaxSize(m_max_size);
}
/*
	Roi maxroi();

	m_max_size= size;
	m_hw_size= m_max_size / m_hw_bin;
	if (!m_hw_roi.isEmpty()) {
		maxroi.setSize(m_hw_size);
		if (maxroi.containsRoi(m_hw_roi)) {
			m_hw_size= m_hw_roi.getSize();
		} else {
			// --- need to resize roi
		}
	}

	m_sw_size= m_hw_size / m_sw_bin;
*/			
	

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
	dim= FrameDim(m_max_size, m_img_type);
}

void CtImage::getSoft(SwBinRoi *& soft) const
{
	soft= m_sw;
}

void CtImage::setMode(ImageOpMode mode)
{
	m_mode= mode;
	// resets unwanted mode
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
			break;
		case HardAndSoft:
			_setHSRoi(roi);
			break;
	}
}

void CtImage::_setHSBin(Bin bin) {
}

void CtImage::_setHSRoi(Roi roi) {
}
