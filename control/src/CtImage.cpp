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

#include "lima/CtImage.h"
#include "lima/CtAcquisition.h"
#include "lima/CtSaving.h"

using namespace lima;

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

CtSwBinRoiFlip::CtSwBinRoiFlip(Size& size, const Bin& bin, const Roi& roi,
                               const Flip& flip, RotationMode rotation) :
  m_rotation(Rotation_0)
{
	DEB_CONSTRUCTOR();
	DEB_PARAM() << DEB_VAR5(size, bin, roi, flip, rotation);

	m_max_size= size;
	m_max_roi= Roi(Point(0,0), m_max_size);

	setBin(bin);
	if (!roi.isEmpty())
		setRoi(roi);
	setFlip(flip);
	setRotation(rotation);
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
		THROW_CTL_ERROR(InvalidValue) << "Software roi is empty";
	if (!m_max_roi.containsRoi(roi))
		THROW_CTL_ERROR(InvalidValue) << "Roi out of limts";
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
	//WARNINGGGGGGGGGGGGGGGGGGGGGGGGG: called Roi() constructor is Roi(Point topleft, Point bottomright)
	// becaus Size /Point -> Point and not Size !!!!
	// So if size = 1024*1024 and m_bin = 1x1 then m_max_roi = 0,0 x 1025,1025
	Size roi_size = m_max_size/m_bin;
	m_max_roi= Roi(Point(0,0), roi_size);
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
		  THROW_CTL_ERROR(NotSupported) << "No hardware binning available";
	}
	else {
		Bin set_bin= bin;
		if (!set_bin.isOne())
			m_hw_bin->checkBin(set_bin);
		if ((!round)&&(set_bin!=bin))
		  THROW_CTL_ERROR(InvalidValue) << "Given hardware binning not possible";
		if (set_bin != m_bin) {
			Bin old_bin = m_bin;
			Roi old_roi = m_set_roi;
			Roi old_max_roi = m_max_roi;

			if (!m_set_roi.isEmpty())
				m_set_roi= m_set_roi.getUnbinned(m_bin);

			m_hw_bin->setBin(set_bin);
			m_bin= set_bin;

			if (!m_bin.isOne() && !m_set_roi.isEmpty())
				m_set_roi= m_set_roi.getBinned(m_bin);
			m_max_roi.setSize(m_max_size / m_bin);
			try
			  {
			    _updateSize();
			  }
			catch(Exception &exc)
			  {
			    m_bin = old_bin;
			    m_set_roi = old_roi;
			    m_max_roi = old_max_roi;
			    m_hw_bin->setBin(m_bin);
			    throw exc;
			  }
		}
		bin= set_bin;
	}
}

void CtHwBinRoiFlip::setRoi(Roi& roi, bool round)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR2(roi,round);

	if (roi.isEmpty())
		THROW_CTL_ERROR(InvalidValue) << "Hardware roi is empty";
	if (!m_max_roi.containsRoi(roi))
		THROW_CTL_ERROR(InvalidValue) << "Roi out of limts";

	if (!m_has_roi) {
		if (!round)
			THROW_CTL_ERROR(NotSupported) << "No hardware roi available";
	}
	else {
		Roi real_roi;
		m_hw_roi->checkRoi(roi, real_roi);
		if ((!round)&&(real_roi!=roi))
			THROW_CTL_ERROR(InvalidValue) << "Given hardware roi not possible";
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
	THROW_CTL_ERROR(NotSupported) << "No hardware flip available";
    }
  else 
    {
      Flip set_flip = flip;
      if (set_flip.x || set_flip.y)
	m_hw_flip->checkFlip(set_flip);
      if (mandatory && set_flip != flip)
	THROW_CTL_ERROR(InvalidValue) << "Given hardware flip not possible";
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
// CLASS _ConfigHandler
// ----------------------------------------------------------------------------
#ifdef WITH_CONFIG
class CtImage::_ConfigHandler : public CtConfig::ModuleTypeCallback
{
public:
  _ConfigHandler(CtImage& image) :
    CtConfig::ModuleTypeCallback("Image"),
    m_image(image)
  {}
  virtual void store(Setting& image_setting)
  {
    CtImage::ImageOpMode imageOpMode;
    m_image.getMode(imageOpMode);
    image_setting.set("imageOpMode",convert_2_string(imageOpMode));

    // --- Roi
    Roi roi;
    m_image.getRoi(roi);
    Setting roi_setting = image_setting.addChild("roi");

    const Point& topleft = roi.getTopLeft();
    roi_setting.set("x",topleft.x);
    roi_setting.set("y",topleft.y);

    const Size& roiSize = roi.getSize();
    roi_setting.set("width",roiSize.getWidth());
    roi_setting.set("height",roiSize.getHeight());
  
    // --- Bin
    Bin bin;
    m_image.getBin(bin);
    Setting bin_setting = image_setting.addChild("bin");

    bin_setting.set("x",bin.getX());
    bin_setting.set("y",bin.getY());

    // --- Flip
    Flip flip;
    m_image.getFlip(flip);
    Setting flip_setting = image_setting.addChild("flip");

    flip_setting.set("x",flip.x);
    flip_setting.set("y",flip.y);

    // --- Rotation
    RotationMode rMode;
    m_image.getRotation(rMode);
    image_setting.set("rotation",convert_2_string(rMode));
  }
  virtual void restore(const Setting& image_setting)
  {
    std::string strimageOpMode;
    if(image_setting.get("imageOpMode",strimageOpMode))
      {
	CtImage::ImageOpMode imageOpMode;
	convert_from_string(strimageOpMode,imageOpMode);
	m_image.setMode(imageOpMode);
      }
    // --- Flip
    Setting flip_setting;
    if(image_setting.getChild("flip",flip_setting))
      {
	Flip flip;
	if(flip_setting.get("x",flip.x) &&
	   flip_setting.get("y",flip.y))
	  m_image.setFlip(flip);
      }
    // --- Bin
    Setting bin_setting;
    if(image_setting.getChild("bin",bin_setting))
      {
	int x,y;
	if(bin_setting.get("x",x) &&
	   bin_setting.get("y",y))
	  {
	    Bin aBin(x,y);
	    m_image.setBin(aBin);
	  }
      }
    // --- Roi
    Setting roi_setting;
    if(image_setting.getChild("roi",roi_setting))
      {
	Point topleft;
	int width,height;
	if(roi_setting.get("x",topleft.x) &&
	   roi_setting.get("y",topleft.y) &&
	   roi_setting.get("width",width) &&
	   roi_setting.get("height",height))
	  {
	    Roi aRoi(topleft.x,topleft.y,width,height);
	    m_image.setRoi(aRoi);
	  }
      }
    // --- Rotation
    std::string strrMode;
    if(image_setting.get("rotation",strrMode))
      {
	RotationMode rMode;
	convert_from_string(strrMode,rMode);
	m_image.setRotation(rMode);
      }
  }
private:
  CtImage& m_image;
};
#endif //WITH_CONFIG
// ----------------------------------------------------------------------------
// CLASS CtImage
// ----------------------------------------------------------------------------
CtImage::CtImage(HwInterface *hw,CtControl &ct)
  : m_ct(ct),m_mode(HardAndSoft)
{
	DEB_CONSTRUCTOR();

	if (!hw->getHwCtrlObj(m_hw_det))
		THROW_CTL_ERROR(Error) << "Cannot get detector info object";

	m_hw_det->getMaxImageSize(m_max_size);
	m_hw_det->getCurrImageType(m_img_type);
	m_next_image_type = m_img_type;

	m_sw= new CtSwBinRoiFlip(m_max_size);
	m_hw= new CtHwBinRoiFlip(hw, m_sw, m_max_size);

	m_cb_size= new CtMaxImageSizeCB(this);
	m_hw_det->registerMaxImageSizeCallback(*m_cb_size);

	//Check if monitor mode
	HwSyncCtrlObj	*hw_sync;
	if(hw->getHwCtrlObj(hw_sync))
	  {
	    HwSyncCtrlObj::AccessMode access_mode;
	    hw_sync->getAccessMode(access_mode);
	    m_monitor_mode = access_mode == HwSyncCtrlObj::Monitor;
	    // Force image operation to be Software in monitor mode
	    m_mode = m_monitor_mode ? SoftOnly : HardAndSoft;
	  }
	else
	  m_monitor_mode = false;
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

	if(m_monitor_mode && mode != SoftOnly)
	  THROW_CTL_ERROR(Error) << "Hardware image operation " 
				 << "are not possible in monitor mode";

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
	Bin bin; getBin(bin);
	Size full_size = m_max_size / bin;
	Roi fullRoi(Point(0,0),full_size);
	RotationMode aRotation = m_sw->getRotation();
	fullRoi = fullRoi.getRotated(aRotation,full_size);
	if (roi.isEmpty() || roi == fullRoi) {
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
		_completeWithSoftRoi(m_hw->getSetRoi(),m_hw->getRealRoi());
	} else {
		m_sw->setBin(bin);
	}
}


void CtImage::_setHSRoi(const Roi &roi) 
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(roi);

	if (m_hw->hasRoiCapability()) {	
	        Roi roi_unbin, roi_by_hw, roi_set_hw;
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

		_completeWithSoftRoi(roi_by_hw, roi_set_hw);
	} else {
		m_sw->setRoi(roi);
	}
}

void CtImage::_completeWithSoftRoi(Roi roi_set,Roi hw_roi)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(roi_set,hw_roi);

  if (roi_set==hw_roi) {
    m_sw->resetRoi();
  } else {
    const Size& max_roi_size = m_hw->getMaxRoiSize();
    RotationMode aSoftwareRotation = m_sw->getRotation();
    // Remove the rotation to hardware Roi
    hw_roi= hw_roi.getUnrotated(aSoftwareRotation,max_roi_size);

    // Remove the software Flip to hardware Roi
    const Flip &aSoftwareFlip = m_sw->getFlip();
    hw_roi= hw_roi.getFlipped(aSoftwareFlip,max_roi_size);

    
    // Apply software flip to hardware roi
    roi_set = roi_set.getFlipped(aSoftwareFlip,max_roi_size);
    hw_roi = hw_roi.getFlipped(aSoftwareFlip,max_roi_size);
    //Apply software rotation to hardware roi
    roi_set = roi_set.getRotated(aSoftwareRotation,max_roi_size);
    hw_roi = hw_roi.getRotated(aSoftwareRotation,max_roi_size);
    //Calc the roi by soft needed
    Bin bin_by_sw = m_sw->getBin();
    Roi roi_by_sw;
    roi_by_sw= hw_roi.subRoiAbs2Rel(roi_set);
    roi_by_sw= roi_by_sw.getBinned(bin_by_sw);
    m_sw->setRoi(roi_by_sw);
    DEB_TRACE() << DEB_VAR1(roi_by_sw);
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
	  m_sw->setFlip(set_sw_flip);
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

	if(roi.isEmpty())
	  {
	    FrameDim dim;
	    getImageDim(dim);
	    roi = Roi(Point(0,0),dim.getSize());
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
	//Add operation into internal header
	CtSaving* saving = m_ct.saving();

	Bin bin;getBin(bin);
	if(!bin.isOne())
	  saving->addToInternalCommonHeader("binning",bin);

	Roi roi;getRoi(roi);
	if(!roi.isEmpty())
	  saving->addToInternalCommonHeader("roi",roi);

	Flip flip;getFlip(flip);
	if(flip.x || flip.y)
	  saving->addToInternalCommonHeader("flip",flip);

	RotationMode rMode;getRotation(rMode);
	if(rMode != Rotation_0)
	  saving->addToInternalCommonHeader("rotation",rMode);
}

bool CtImage::applySoft(SoftOpInternalMgr *op)
{
	DEB_MEMBER_FUNCT();

	bool sw_is_active = m_sw->apply(op);

	DEB_RETURN() << sw_is_active;

	return sw_is_active;
}

#ifdef WITH_CONFIG
CtConfig::ModuleTypeCallback* CtImage::_getConfigHandler()
{
  return new _ConfigHandler(*this);
}
#endif //WITH_CONFIG
