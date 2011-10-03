//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################

#include "CtImage.h"
#include "CtAcquisition.h"

using namespace lima;

static void _get_prime_decomposition(int val,std::list<int> &posDiv)
{
  int div = 2;
  int rDiv;
  int previous = 1;
  do
    {
      rDiv = val / div;
      if(!(val % div))
	{
	  posDiv.push_back(div * previous);

	  val = rDiv;
	  previous *= div;
	}
      else
	++div,previous = 1;
    }
  while(rDiv >= 1);
  posDiv.sort();
}

static const Bin Bin_1x1(1, 1);

#define SWAP_DIM_IF_ROTATION(dimStruct) \
  RotationMode aRotationMode;		\
  getRotation(aRotationMode);		\
  if(aRotationMode == Rotation_90 ||	\
     aRotationMode == Rotation_270)	\
    dimStruct.swapDimensions();

// ----------------------------------------------------------------------------
// CLASS CtSwBinRoiFlip
// ----------------------------------------------------------------------------
CtSwBinRoiFlip::CtSwBinRoiFlip(Size& size) :
  m_rotation(Rotation_0)
{
	DEB_CONSTRUCTOR();
	DEB_PARAM() << DEB_VAR1(size);

	m_max_size= size;
	m_max_roi= Roi(Point(0,0), m_max_size);
}

CtSwBinRoiFlip::~CtSwBinRoiFlip()
{
  DEB_DESTRUCTOR();
}

void CtSwBinRoiFlip::setMaxSize(Size& size)
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

void CtSwBinRoiFlip::setBin(const Bin& bin)
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

void CtSwBinRoiFlip::setRoi(const Roi& roi)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(roi);

	if (roi.isEmpty())
		throw LIMA_CTL_EXC(InvalidValue, "Software roi is empty");
	if (!m_max_roi.containsRoi(roi))
		throw LIMA_CTL_EXC(InvalidValue, "Roi out of limts");
	m_roi= roi;
}

void CtSwBinRoiFlip::setFlip(const Flip &flip)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(flip);
  m_flip = flip;
}

void CtSwBinRoiFlip::setRotation(RotationMode rotation)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(rotation);
  m_rotation = rotation;
}

const Size& CtSwBinRoiFlip::getSize() const
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

void CtSwBinRoiFlip::resetBin()
{
	DEB_MEMBER_FUNCT();

	setBin(Bin_1x1);
}

void CtSwBinRoiFlip::resetRoi()
{
	DEB_MEMBER_FUNCT();

	m_roi.reset();
}

void CtSwBinRoiFlip::resetFlip()
{
  DEB_MEMBER_FUNCT();

  m_flip.reset();
}

void CtSwBinRoiFlip::resetRotation()
{
  DEB_MEMBER_FUNCT();

  m_rotation = Rotation_0;
}

void CtSwBinRoiFlip::reset()
{
	DEB_MEMBER_FUNCT();

	resetBin();
	resetRoi();
	resetFlip();
	resetRotation();
}

bool CtSwBinRoiFlip::apply(SoftOpInternalMgr *op)
{
	DEB_MEMBER_FUNCT();
	
	op->setBin(m_bin);
	op->setRoi(m_roi);
	op->setFlip(m_flip);
	op->setRotation(m_rotation);

	bool is_active = !m_bin.isOne() || !m_roi.isEmpty() || 
	  (m_flip.x || m_flip.y) || (m_rotation != Rotation_0);
	
	DEB_RETURN() << DEB_VAR1(is_active);

	return is_active;
}
	
// ----------------------------------------------------------------------------
// CLASS CtHwBinRoiFlip
// ----------------------------------------------------------------------------

CtHwBinRoiFlip::CtHwBinRoiFlip(HwInterface *hw, CtSwBinRoiFlip *sw_bin_roi, Size& size)
	: m_sw_bin_roi_flip(sw_bin_roi)
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

CtHwBinRoiFlip::~CtHwBinRoiFlip()
{
	DEB_DESTRUCTOR();
}

void CtHwBinRoiFlip::setMaxSize(const Size& size)
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

void CtHwBinRoiFlip::setBin(Bin& bin, bool round)
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
			if(set_bin != bin)
			  {
			    // test X
			    std::list<int> primeX;
			    _get_prime_decomposition(bin.getX(),primeX);
			    set_bin = Bin(1,1);
			    for(std::list<int>::iterator i = primeX.begin();
				i != primeX.end();++i)
			      {
				Bin askedBin = Bin(*i,1);
				Bin tmpBin = askedBin;;
				m_hw_bin->checkBin(tmpBin);
				if(tmpBin == askedBin)
				  set_bin = askedBin;
			      }
			    // test Y
			    std::list<int> primeY;
			    _get_prime_decomposition(bin.getY(),primeY);
			    for(std::list<int>::iterator i = primeY.begin();
				i != primeY.end();++i)
			      {
				Bin askedBin = Bin(set_bin.getX(),*i);
				Bin tmpBin = askedBin;
				m_hw_bin->checkBin(tmpBin);
				if(tmpBin == askedBin)
				  set_bin = askedBin;
			      }
			  }
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

void CtHwBinRoiFlip::setRoi(Roi& roi, bool round)
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
/** @brief set Hardware Flip
 *  @param flip the flip structure
 *  @param mandatory if all flip should be done by hardware
 */
void CtHwBinRoiFlip::setFlip(Flip& flip, bool mandatory)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(flip,mandatory);

  if (!m_has_flip) 
    {
      if (mandatory)
	throw LIMA_CTL_EXC(NotSupported, "No hardware flip available");
    }
  else 
    {
      Flip set_flip = flip;
      if (set_flip.x || set_flip.y)
	m_hw_flip->checkFlip(set_flip);
      if (mandatory && set_flip != flip)
	throw LIMA_CTL_EXC(InvalidValue, "Given hardware flip not possible");
      if (set_flip != m_flip)
	{
	  m_hw_flip->setFlip(set_flip);
	  m_flip = set_flip;
	}
      flip = set_flip;
    }
}

void CtHwBinRoiFlip::_updateSize()
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
		m_sw_bin_roi_flip->setMaxSize(m_size);
}

void CtHwBinRoiFlip::resetBin()
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

void CtHwBinRoiFlip::resetRoi()
{
	DEB_MEMBER_FUNCT();

	m_set_roi.reset();
	_updateSize();
}

void CtHwBinRoiFlip::resetFlip()
{
  DEB_MEMBER_FUNCT();

  m_flip.reset();
}

void CtHwBinRoiFlip::reset()
{
	DEB_MEMBER_FUNCT();

	resetBin();
	resetRoi();
	resetFlip();
}

void CtHwBinRoiFlip::apply()
{
	DEB_MEMBER_FUNCT();

	if (m_has_bin) 
		m_hw_bin->setBin(m_bin);
	if (m_has_roi)
		m_hw_roi->setRoi(m_set_roi);
	if (m_has_flip)
		m_hw_flip->setFlip(m_flip);
}
	
// ----------------------------------------------------------------------------
// CLASS CtMaxImageSizeCB
// ----------------------------------------------------------------------------

void CtMaxImageSizeCB::maxImageSizeChanged(const Size& size, ImageType 
					   image_type)
{
	m_ct->_setMaxImage(size, image_type);
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
	m_next_image_type = m_img_type;

	m_sw= new CtSwBinRoiFlip(m_max_size);
	m_hw= new CtHwBinRoiFlip(hw, m_sw, m_max_size);

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

	SWAP_DIM_IF_ROTATION(size);

	DEB_RETURN() << DEB_VAR1(size);
}

void CtImage::_setMaxImage(const Size &size, ImageType type)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(size,type);

	m_max_size= size;
	m_next_image_type = m_img_type = type;

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

void CtImage::setImageType(ImageType type)
{
	DEB_MEMBER_FUNCT();
	m_next_image_type = type;
}

void CtImage::getImageDim(FrameDim& dim) const
{
	DEB_MEMBER_FUNCT();
	CtAcquisition *acq = m_ct.acquisition();
	AcqMode mode;
	acq->getAcqMode(mode);
	ImageType imageType = mode == Accumulation ? Bpp32S : m_img_type;
	dim= FrameDim(m_sw->getSize(), imageType);

	SWAP_DIM_IF_ROTATION(dim);

	DEB_RETURN() << DEB_VAR1(dim);
}

void CtImage::getHwImageDim(FrameDim& dim) const
{
	DEB_MEMBER_FUNCT();

	dim= FrameDim(m_hw->getSize(), m_img_type);

	DEB_RETURN() << DEB_VAR1(dim);
}

void CtImage::getSoft(CtSwBinRoiFlip *& soft) const
{
	DEB_MEMBER_FUNCT();

	soft= m_sw;

	DEB_RETURN() << DEB_VAR1(soft);
}

void CtImage::getHard(CtHwBinRoiFlip *& hard) const
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
		  {
		    const Size& max_roi_size = m_hw->getMaxRoiSize();
		    // Remove the software Rotation to hardware Roi
		    RotationMode aSoftwareRotation = m_sw->getRotation();
		    roi = roi.getUnrotated(aSoftwareRotation,max_roi_size);
		    // Remove the software Flip to hardware Roi
		    const Flip &aSoftwareFlip = m_sw->getFlip();
		    roi = roi.getFlipped(aSoftwareFlip,max_roi_size);

		    m_hw->setRoi(roi, false);

		    // Add Flip
		    roi = roi.getFlipped(aSoftwareFlip,max_roi_size);
		    // Add Rotation
		    roi = roi.getRotated(aSoftwareRotation,max_roi_size);
		  }
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

		RotationMode aSoftwareRotation = m_sw->getRotation();

		bin_by_hw= m_hw->getBin();

		bin_by_sw= m_sw->getBin();
		bin_total= bin_by_hw * bin_by_sw;
		DEB_TRACE() << DEB_VAR3(bin_by_hw, bin_by_sw, bin_total);

		roi_unbin= roi.getUnbinned(bin_total);
		roi_by_hw= roi_unbin.getBinned(bin_by_hw);

		const Size& max_roi_size = m_hw->getMaxRoiSize();

		// Remove the rotation to hardware Roi
		roi_by_hw= roi_by_hw.getUnrotated(aSoftwareRotation,max_roi_size);

		// Remove the software Flip to hardware Roi
		const Flip &aSoftwareFlip = m_sw->getFlip();
		roi_by_hw= roi_by_hw.getFlipped(aSoftwareFlip,max_roi_size);

		roi_set_hw= roi_by_hw;

		m_hw->setRoi(roi_set_hw, true);
		DEB_TRACE() << DEB_VAR2(roi_by_hw, roi_set_hw);

		if (roi_set_hw==roi_by_hw) {
			m_sw->resetRoi();
		} else {
			// Apply software flip to hardware roi
			roi_set_hw = roi_set_hw.getFlipped(aSoftwareFlip,max_roi_size);
			//Apply software rotation to hardware roi
			roi_set_hw = roi_set_hw.getRotated(aSoftwareRotation,max_roi_size);
			roi_by_sw= roi_set_hw.subRoiAbs2Rel(roi_by_hw);
			roi_by_sw= roi_by_sw.getBinned(bin_by_sw);
			m_sw->setRoi(roi_by_sw);
		}
	} else {
		m_sw->setRoi(roi);
	}
}

void CtImage::setFlip(Flip &flip)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(flip);
  //Get previous roi unflipped
  Flip currentFlip;
  getFlip(currentFlip);
  Roi currentRoi;
  getRoi(currentRoi);
  RotationMode currentRotation;
  getRotation(currentRotation);

  const Size& max_roi_size = m_hw->getMaxRoiSize();
  currentRoi = currentRoi.getUnrotated(currentRotation,max_roi_size);
  currentRoi = currentRoi.getFlipped(currentFlip,max_roi_size);

  if(!flip.x && ! flip.y)
    _resetFlip();
  else
    {
      switch(m_mode)
	{
	case SoftOnly:
	  m_sw->setFlip(flip);
	  break;
	case HardOnly:
	  m_hw->setFlip(flip,true);
	  break;
	case HardAndSoft:
	  _setHSFlip(flip);
	  break;
	}
    }
  //Set the previous roi
  resetRoi();
  currentRoi = currentRoi.getFlipped(flip,max_roi_size);
  currentRoi = currentRoi.getRotated(currentRotation,max_roi_size);
  setRoi(currentRoi);
}

void CtImage::setRotation(RotationMode rotation)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(rotation);
  // Get previous roi unrotated
  RotationMode currentRotation;
  getRotation(currentRotation);
  Roi currentRoi;
  getRoi(currentRoi);
  Bin currentBin;
  getBin(currentBin);

  const Size& max_roi_size = m_hw->getMaxRoiSize();
  currentRoi = currentRoi.getUnrotated(currentRotation,max_roi_size);

  m_sw->setRotation(rotation);

  resetRoi();

  currentRoi = currentRoi.getRotated(rotation,max_roi_size);
  setRoi(currentRoi);
}

void CtImage::_setHSFlip(const Flip &flip)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(flip);

  if(m_hw->hasFlipCapability())
    {
      Flip set_hw_flip = flip;
      m_hw->setFlip(set_hw_flip,false);
      DEB_TRACE() << DEB_VAR2(flip,set_hw_flip);
      if(set_hw_flip == flip)
	m_sw->resetFlip();
      else
	{
	  Flip set_sw_flip = flip - set_hw_flip;
	  m_sw->setFlip(set_hw_flip);
	}
    }
  else
    m_sw->setFlip(flip);
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
void CtImage::resetFlip()
{
  Flip aFlip(false,false);
  setFlip(aFlip);
}

void CtImage::_resetFlip()
{
  DEB_MEMBER_FUNCT();
  
  m_hw->resetFlip();
  m_sw->resetFlip();
}

void CtImage::resetRotation()
{
  DEB_MEMBER_FUNCT();

  setRotation(Rotation_0);
}

void CtImage::reset() 
{
	DEB_MEMBER_FUNCT();

	m_hw->reset();
	m_sw->reset();

	// Resync FrameDim just in case
	m_hw_det->getMaxImageSize(m_max_size);
	m_hw_det->getCurrImageType(m_img_type);
	m_next_image_type = m_img_type;
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
		const Size& max_roi_size = m_hw->getMaxRoiSize();
		Roi roi_by_hw= m_hw->getRealRoi();
		// Add software Flip to Roi
		const Flip &aSoftwareFlip = m_sw->getFlip();
		roi_by_hw = roi_by_hw.getFlipped(aSoftwareFlip,max_roi_size);
		// Add rotation to Roi
		RotationMode aSoftRotation = m_sw->getRotation();
		roi_by_hw = roi_by_hw.getRotated(aSoftRotation,max_roi_size);

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

void CtImage::getFlip(Flip &flip) const
{
  DEB_MEMBER_FUNCT();

  flip = m_hw->getFlip() + m_sw->getFlip();
  
  DEB_RETURN() << DEB_VAR1(flip);
}

void CtImage::getRotation(RotationMode &rotation) const
{
  DEB_MEMBER_FUNCT();

  rotation = m_sw->getRotation();

  DEB_RETURN() << DEB_VAR1(rotation);
}

void CtImage::applyHard()
{
	DEB_MEMBER_FUNCT();

	if(m_img_type != m_next_image_type)
	  {
	    m_hw_det->setCurrImageType(m_next_image_type);
	    m_img_type = m_next_image_type;
	  }

	m_hw->apply();
}

bool CtImage::applySoft(SoftOpInternalMgr *op)
{
	DEB_MEMBER_FUNCT();

	bool sw_is_active = m_sw->apply(op);

	DEB_RETURN() << sw_is_active;

	return sw_is_active;
}
