
#include "CtImage.h"

using namespace lima;

void BinRoi::setMaxSize(Size& size)
{
	m_max_size= size;
	_updateRoi();
}

void BinRoi::setBin(Bin& bin)
{
	if (bin != m_bin) {
		m_roi= m_roi.getUnbinned(m_bin);
		m_bin= bin;
		if (!m_bin.isOne())
			m_roi= m_roi.getBinned(m_bin);
	}
	_updateSize();
}

void BinRoi::setRoi(Roi& roi)
{
	Size bin_size;
	_getBinSize(bin_size);
	Roi max_roi(Point(0,0),bin_size);

	if (roi != m_roi) {
		if (max_roi.containsRoi(roi)) {
			m_roi= roi;
			_updateSize();
		}
		else {
			throw LIMA_CTL_EXC(InvalidValue, "Roi out of limts");
		}
	}
}

void BinRoi::_updateSize()
{
	if (m_roi.isEmpty())
		_getBinSize(m_size);
	else	m_size= Size(m_roi.getSize());
}

void BinRoi::_updateRoi()
{
	Size bin_size;
	_getBinSize(bin_size);
	Roi max_roi(Point(0,0), bin_size);

	if (!max_roi.containsPoint(m_roi.getTopLeft())) {
		m_roi.reset();
		_updateSize();
	}
	else if (!max_roi.containsPoint(m_roi.getBottomRight())) {
		m_roi.setCorners(m_roi.getTopLeft(), max_roi.getBottomRight());
		_updateSize();
	}
}

// ----------------------------------------------------------------------------
// CLASS CtImage
// ----------------------------------------------------------------------------

CtImage::CtImage(HwInterface *hw)
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

	m_hw_size= m_max_size;
	m_sw_size= m_max_size;
}

void CtImage::getMaxImageSize(Size& size) const
{
	size= m_max_size;
}

void CtImage::setMaxImage(Size size, ImageType type)
{
	m_max_size= size;
	m_img_type= type;
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
	dim= FrameDim(m_sw_size, m_img_type);
}

void CtImage::getHwImageDim(FrameDim& dim) const
{
	dim= FrameDim(m_hw_size, m_img_type);
}
/*
void CtImage::setSoftBin(Bin bin) const
{
	m_soft_bin= bin;
	m_eff_bin= m_hard_bin * m_soft_bin;
}

void CtImage::resetSoftBin() const
{
	m_soft_bin= Bin();
	m_eff_bin= m_hard_bin;
}

void CtImage::setSoftRoi(Roi roi) const
{
	if (m_hard_roi.containsRoi(roi)) {
		m_soft_roi= roi;
		m_eff_roi= m_soft_roi;
	}
	else {
		throw LIMA_CTL_EXC(InvalidValue, "Invalid ROI");
	}
}

void CtImage::resetSoftRoi() const
{
}
*/
