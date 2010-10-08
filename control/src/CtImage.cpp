
#include "CtImage.h"
#include "CtAcquisition.h"

using namespace lima;

static const Bin Bin_1x1(1, 1);

// ----------------------------------------------------------------------------
// CLASS CtSwBinRoi
// ----------------------------------------------------------------------------
CtSwBinRoi::CtSwBinRoi(Size& size)
{
	DEB_CONSTRUCTOR();
	DEB_PARAM() << DEB_VAR1(size);

	m_max_size= size;
	m_max_roi= Roi(Point(0,0), m_max_size);
}

void CtSwBinRoi::setMaxSize(Size& size)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(size);

	m_max_size= size;
	m_max_roi.setSize(m_max_size / m_bin);

	if (!m_roi.isEmpty()) {
		Point roi_tl = m_roi.getTopLeft();
		Point roi_br = m_roi.getBottomRight();
		if (!m_max_roi.containsPoint(roi_tl)) {
			m_roi.reset();
		}
		else if (!m_max_roi.containsPoint(roi_br)) {
			m_roi = Roi(roi_tl, m_max_roi.getBottomRight());
		}
	}		
}

void CtSwBinRoi::setBin(const Bin& bin)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(bin);

	if (bin != m_bin) {
		m_roi= m_roi.getUnbinned(m_bin);
		m_bin= bin;
		if (!m_bin.isOne())
			m_roi= m_roi.getBinned(m_bin);
		m_max_roi.setSize(m_max_size / m_bin);
	}
}

void CtSwBinRoi::setRoi(const Roi& roi)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(roi);

	if (roi.isEmpty())
		throw LIMA_CTL_EXC(InvalidValue, "Hardware roi is empty");
	if (!m_max_roi.containsRoi(roi))
		throw LIMA_CTL_EXC(InvalidValue, "Roi out of limts");
	m_roi= roi;
}

const Size& CtSwBinRoi::getSize()
{
	DEB_MEMBER_FUNCT();

	DEB_TRACE() << DEB_VAR3(m_max_size, m_bin, m_roi);

	if (m_roi.isEmpty())
		m_size= m_max_size / m_bin;
	else	
		m_size= Size(m_roi.getSize());
	
	DEB_RETURN() << DEB_VAR1(m_size);

	return m_size;
}

void CtSwBinRoi::resetBin()
{
	DEB_MEMBER_FUNCT();

	setBin(Bin_1x1);
}

void CtSwBinRoi::resetRoi()
{
	DEB_MEMBER_FUNCT();

	m_roi.reset();
}

void CtSwBinRoi::reset()
{
	DEB_MEMBER_FUNCT();

	resetBin();
	resetRoi();
}

bool CtSwBinRoi::apply(SoftOpInternalMgr *op)
{
	DEB_MEMBER_FUNCT();
	
	op->setBin(m_bin);
	op->setRoi(m_roi);
	
	bool is_active = !m_bin.isOne() || !m_roi.isEmpty();
	
	DEB_RETURN() << DEB_VAR1(is_active);

	return is_active;
}
	
// ----------------------------------------------------------------------------
// CLASS CtSwBinRoi
// ----------------------------------------------------------------------------

CtHwBinRoi::CtHwBinRoi(HwInterface *hw, CtSwBinRoi *sw_bin_roi, Size& size)
	: m_sw_bin_roi(sw_bin_roi)
{
	DEB_CONSTRUCTOR();
	DEB_PARAM() << DEB_VAR2(*sw_bin_roi,size);

	m_has_bin= hw->getHwCtrlObj(m_hw_bin);
	m_has_roi= hw->getHwCtrlObj(m_hw_roi);
	m_has_flip= hw->getHwCtrlObj(m_hw_flip);

	if (m_has_bin)
		m_hw_bin->setBin(m_bin);
	if (m_has_roi)
		m_hw_roi->setRoi(m_set_roi);

	m_max_size= size;
	m_max_roi= Roi(Point(0,0), m_max_size / m_bin);
	m_size= m_max_size;
}

CtHwBinRoi::~CtHwBinRoi()
{
	DEB_DESTRUCTOR();
}

void CtHwBinRoi::setMaxSize(const Size& size)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(size);

	m_max_size= size;
	m_max_roi.setSize(m_max_size / m_bin);
	
	if (!m_set_roi.isEmpty()) {
		Point roi_tl = m_set_roi.getTopLeft();
		Point roi_br = m_set_roi.getBottomRight();
		if (!m_max_roi.containsPoint(roi_tl)) {
			m_set_roi.reset();
		}
		else if (!m_max_roi.containsPoint(roi_br)) {
			m_set_roi = Roi(roi_tl, m_max_roi.getBottomRight());
		}
	}

	_updateSize();
}

void CtHwBinRoi::setBin(Bin& bin, bool round)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(bin,round);

	if (!m_has_bin) {
		if (!round)
			throw LIMA_CTL_EXC(NotSupported, "No hardware binning available");
	}
	else {
		Bin set_bin= bin;
		if (!set_bin.isOne())
			m_hw_bin->checkBin(set_bin);
		if ((!round)&&(set_bin!=bin))
			throw LIMA_CTL_EXC(InvalidValue, "Given hardware binning not possible");
		if (set_bin != m_bin) {
			if (!m_set_roi.isEmpty())
				m_set_roi= m_set_roi.getUnbinned(m_bin);

			m_hw_bin->setBin(set_bin);
			m_bin= set_bin;

			if (!m_bin.isOne() && !m_set_roi.isEmpty())
				m_set_roi= m_set_roi.getBinned(m_bin);
			m_max_roi.setSize(m_max_size / m_bin);
			_updateSize();
		}
		bin= set_bin;
	}
}

void CtHwBinRoi::setRoi(Roi& roi, bool round)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(roi,round);

	if (roi.isEmpty())
		throw LIMA_CTL_EXC(InvalidValue, "Hardware roi is empty");
	if (!m_max_roi.containsRoi(roi))
		throw LIMA_CTL_EXC(InvalidValue, "Roi out of limts");

	if (!m_has_roi) {
		if (!round)
			throw LIMA_CTL_EXC(NotSupported, "No hardware roi available");
	}
	else {
		Roi real_roi;
		m_hw_roi->checkRoi(roi, real_roi);
		if ((!round)&&(real_roi!=roi))
			throw LIMA_CTL_EXC(InvalidValue, "Given hardware roi not possible");
		if (roi != m_set_roi) {
			m_hw_roi->setRoi(roi);
			m_set_roi= roi;
			_updateSize();
		}
		roi= real_roi;
	}
}

void CtHwBinRoi::_updateSize()
{
	DEB_MEMBER_FUNCT();

	Size o_size(m_size);

	if (m_set_roi.isEmpty()) {
		m_real_roi.reset();
		m_size= m_max_size / m_bin;
	} else {
		m_hw_roi->checkRoi(m_set_roi, m_real_roi);
		m_size= Size(m_real_roi.getSize());
	}

	if (o_size != m_size)
		m_sw_bin_roi->setMaxSize(m_size);
}

void CtHwBinRoi::resetBin()
{
	DEB_MEMBER_FUNCT();

	if (m_has_bin) {
		Bin bin = Bin_1x1;
		setBin(bin, false);
	} else {
		m_bin.reset();
		_updateSize();
	}
}

void CtHwBinRoi::resetRoi()
{
	DEB_MEMBER_FUNCT();

	m_set_roi.reset();
	_updateSize();
}

void CtHwBinRoi::reset()
{
	DEB_MEMBER_FUNCT();

	resetBin();
	resetRoi();
}

void CtHwBinRoi::apply()
{
	DEB_MEMBER_FUNCT();

	if (m_has_bin) 
		m_hw_bin->setBin(m_bin);
	if (m_has_roi)
		m_hw_roi->setRoi(m_set_roi);
}
	
// ----------------------------------------------------------------------------
// CLASS CtMaxImageSizeCB
// ----------------------------------------------------------------------------

void CtMaxImageSizeCB::maxImageSizeChanged(const Size& size, ImageType 
					   image_type)
{
	m_ct->setMaxImage(size, image_type);
}

// ----------------------------------------------------------------------------
// CLASS CtImage
// ----------------------------------------------------------------------------
CtImage::CtImage(HwInterface *hw,CtControl &ct)
  : m_ct(ct),m_mode(HardAndSoft)
{
	DEB_CONSTRUCTOR();

	if (!hw->getHwCtrlObj(m_hw_det))
		throw LIMA_CTL_EXC(Error, "Cannot get detector info object");

	m_hw_det->getMaxImageSize(m_max_size);
	m_hw_det->getCurrImageType(m_img_type);

	m_sw= new CtSwBinRoi(m_max_size);
	m_hw= new CtHwBinRoi(hw, m_sw, m_max_size);

	m_cb_size= new CtMaxImageSizeCB(this);
	m_hw_det->registerMaxImageSizeCallback(*m_cb_size);
}

CtImage::~CtImage()
{
	DEB_DESTRUCTOR();

	delete m_cb_size;
	delete m_hw;
	delete m_sw;
}

void CtImage::getMaxImageSize(Size& size) const
{
	DEB_MEMBER_FUNCT();

	size= m_max_size;

	DEB_RETURN() << DEB_VAR1(size);
}

void CtImage::setMaxImage(const Size &size, ImageType type)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(size,type);

	m_max_size= size;
	m_img_type= type;

	m_hw->setMaxSize(m_max_size);
}

void CtImage::getImageType(ImageType& type) const
{
	DEB_MEMBER_FUNCT();
	CtAcquisition *acq = m_ct.acquisition();
	AcqMode mode;
	acq->getAcqMode(mode);
	type= mode == Accumulation ? Bpp32S : m_img_type;

	DEB_RETURN() << DEB_VAR1(type);
}

void CtImage::getImageDim(FrameDim& dim) const
{
	DEB_MEMBER_FUNCT();
	CtAcquisition *acq = m_ct.acquisition();
	AcqMode mode;
	acq->getAcqMode(mode);
	ImageType imageType = mode == Accumulation ? Bpp32S : m_img_type;
	dim= FrameDim(m_sw->getSize(), imageType);

	DEB_RETURN() << DEB_VAR1(dim);
}

void CtImage::getHwImageDim(FrameDim& dim) const
{
	DEB_MEMBER_FUNCT();

	dim= FrameDim(m_hw->getSize(), m_img_type);

	DEB_RETURN() << DEB_VAR1(dim);
}

void CtImage::getSoft(CtSwBinRoi *& soft) const
{
	DEB_MEMBER_FUNCT();

	soft= m_sw;

	DEB_RETURN() << DEB_VAR1(soft);
}

void CtImage::getHard(CtHwBinRoi *& hard) const
{
	DEB_MEMBER_FUNCT();

	hard= m_hw;
}

void CtImage::setMode(ImageOpMode mode)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(mode);

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
	DEB_MEMBER_FUNCT();

	mode= m_mode;

	DEB_RETURN() << DEB_VAR1(mode);
}

void CtImage::setBin(Bin& bin)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(bin);

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
		
void CtImage::setRoi(Roi& roi)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(roi);

	if (roi.isEmpty()) {
		resetRoi();
		return;
	}

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

void CtImage::_setHSBin(const Bin &bin) 
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(bin);

	if (m_hw->hasBinCapability()) {
		Bin set_hw_bin = bin;
		m_hw->setBin(set_hw_bin, true);
		DEB_TRACE() << DEB_VAR2(bin, set_hw_bin);
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


void CtImage::_setHSRoi(const Roi &roi) 
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(roi);

	if (m_hw->hasRoiCapability()) {	
		Roi roi_unbin, roi_by_hw, roi_set_hw, roi_by_sw;
		Bin bin_total, bin_by_hw, bin_by_sw;

		bin_by_hw= m_hw->getBin();
		bin_by_sw= m_sw->getBin();
		bin_total= bin_by_hw * bin_by_sw;
		DEB_TRACE() << DEB_VAR3(bin_by_hw, bin_by_sw, bin_total);

		roi_unbin= roi.getUnbinned(bin_total);
		roi_by_hw= roi_unbin.getBinned(bin_by_hw);
		roi_set_hw= roi_by_hw;

		m_hw->setRoi(roi_set_hw, true);
		DEB_TRACE() << DEB_VAR2(roi_by_hw, roi_set_hw);

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

void CtImage::resetBin() 
{
	DEB_MEMBER_FUNCT();

	m_hw->resetBin();
	m_sw->resetBin();
}

void CtImage::resetRoi() 
{
	DEB_MEMBER_FUNCT();

	m_hw->resetRoi();
	m_sw->resetRoi();
}

void CtImage::reset() 
{
	DEB_MEMBER_FUNCT();

	m_hw->reset();
	m_sw->reset();
}

void CtImage::getBin(Bin& bin) const
{
	DEB_MEMBER_FUNCT();

	if (m_hw->hasBinCapability())
		bin= m_hw->getBin() * m_sw->getBin();
	else
		bin= m_sw->getBin();

	DEB_RETURN() << DEB_VAR1(bin);
}

void CtImage::getRoi(Roi& roi) const
{
	DEB_MEMBER_FUNCT();

	if (m_hw->hasRoiCapability()) {
		Roi roi_by_hw= m_hw->getRealRoi();
		Roi roi_by_sw= m_sw->getRoi();
		Bin bin_by_sw= m_sw->getBin();

		if (roi_by_hw.isEmpty()) {
			roi= roi_by_sw;
		} else if (roi_by_sw.isEmpty()) {
			roi= roi_by_hw.getBinned(bin_by_sw);	
		} else {
			Roi sw_unbin= roi_by_sw.getUnbinned(bin_by_sw);
			Roi roi_total= roi_by_hw.subRoiRel2Abs(sw_unbin);
			roi= roi_total.getBinned(bin_by_sw);
		}
	} else {
		roi= m_sw->getRoi();
	}

	DEB_RETURN() << DEB_VAR1(roi);
}

void CtImage::applyHard()
{
	DEB_MEMBER_FUNCT();

	m_hw->apply();
}

bool CtImage::applySoft(SoftOpInternalMgr *op)
{
	DEB_MEMBER_FUNCT();

	bool sw_is_active = m_sw->apply(op);

	DEB_RETURN() << sw_is_active;

	return sw_is_active;
}
