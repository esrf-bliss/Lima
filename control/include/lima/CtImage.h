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
#ifndef CTIMAGE_H
#define CTIMAGE_H

#include <algorithm>

#include "lima/LimaCompatibility.h"
#include "lima/CtControl.h"
#include "lima/CtConfig.h"
#include "lima/Constants.h"
#include "lima/HwInterface.h"
#include "lima/HwDetInfoCtrlObj.h"
#include "lima/SoftOpInternalMgr.h"

namespace lima {

class CtImage;

class LIMACORE_API CtSwBinRoiFlip {
	DEB_CLASS_NAMESPC(DebModControl,"Sofware BinRoiFlip","Control");
    public:
	friend std::ostream& operator<<(std::ostream &os,const CtSwBinRoiFlip &binroi);

	CtSwBinRoiFlip(Size& size);
	CtSwBinRoiFlip(Size& size, const Bin& bin, const Roi& roi,
		       const Flip& flip, RotationMode rotation);
	~CtSwBinRoiFlip();

	void setMaxSize(Size& size);
	void setBin(const Bin& bin);
	void setRoi(const Roi& roi);
	void setFlip(const Flip& flip);
	void setRotation(RotationMode);

	void resetBin();
	void resetRoi();
	void resetFlip();
	void resetRotation();
	void reset();

	const Bin& 	getBin() 	const {return m_bin;}
	const Roi& 	getRoi() 	const {return m_roi;}
	const Size& 	getSize()	const;
	const Flip& 	getFlip() 	const {return m_flip;}
	RotationMode 	getRotation() 	const {return m_rotation;}

	bool apply(SoftOpInternalMgr *op);

    private:
	Size		m_max_size;
	mutable Size	m_size;
	Bin		m_bin;
	Roi		m_roi, m_max_roi;
	Flip    	m_flip;
	RotationMode 	m_rotation;
};


class LIMACORE_API CtHwBinRoiFlip {
	DEB_CLASS_NAMESPC(DebModControl,"Hardware BinRoiFlip","Control");
    public:
	CtHwBinRoiFlip(HwInterface *hw, CtSwBinRoiFlip *sw_bin_roi_flip, Size& size);
	~CtHwBinRoiFlip();

	bool hasBinCapability() const { return m_has_bin; }
	bool hasRoiCapability() const { return m_has_roi; }
	bool hasFlipCapability() const { return m_has_flip; }

	void setMaxSize(const Size& size);
	void setBin(Bin& bin, bool round);
	void setRoi(Roi& roi, bool round);
	void setFlip(Flip &flip,bool mandatory);

	void resetBin();
	void resetRoi();
	void resetFlip();
	void reset();

	const Bin& getBin()         const { return m_bin; }
	const Roi& getSetRoi()      const { return m_set_roi; }
	const Roi& getRealRoi()     const { return m_real_roi; }
	const Size& getSize()       const { return m_size; }
	const Size& getMaxRoiSize() const { return m_max_roi.getSize(); }
	const Flip& getFlip()       const { return m_flip; }

	void apply();

    private:
	void _updateSize();

	HwBinCtrlObj	*m_hw_bin;
	HwRoiCtrlObj	*m_hw_roi;
	HwFlipCtrlObj	*m_hw_flip;
	CtSwBinRoiFlip	*m_sw_bin_roi_flip;
	bool	m_has_bin, m_has_roi, m_has_flip;
	Size	m_max_size, m_size;
	Bin	m_bin;
	Roi	m_set_roi, m_real_roi, m_max_roi;
	Flip	m_flip;
};


class LIMACORE_API CtMaxImageSizeCB : public HwMaxImageSizeCallback
{
    public:
	CtMaxImageSizeCB(CtImage *ct) : m_ct(ct) {}
    protected:	
	void maxImageSizeChanged(const Size& size, ImageType image_type);
    private:
	CtImage *m_ct;
};

	
class LIMACORE_API CtImage {
	friend class CtControl;
	DEB_CLASS_NAMESPC(DebModControl,"Image","Control");
    public:
	friend class CtMaxImageSizeCB;

	enum ImageOpMode {
		HardOnly,
		SoftOnly,
		HardAndSoft,
	};

	CtImage(HwInterface *hw,CtControl&);
	~CtImage();

	void getMaxImageSize(Size& size) const;

	void getImageType(ImageType& type) const;
	void setImageType(ImageType type);

	void getHwImageDim(FrameDim& dim) const;
	void getImageDim(FrameDim& dim) const;

	// --- soft
	void getSoft(CtSwBinRoiFlip *& soft) const;
	void getHard(CtHwBinRoiFlip *& hard) const;

	// --- wizard
	void setMode(ImageOpMode mode);
	void getMode(ImageOpMode& mode) const;

	void setRoi(Roi& roi);
	void setBin(Bin& bin);
	void setFlip(Flip &flip);
	void setRotation(RotationMode rotation);

	void resetRoi();
	void resetBin();
	void resetFlip();
	void resetRotation();

	// --- effective
	void getRoi(Roi& roi) const;
	void getBin(Bin& bin) const;
	void getFlip(Flip &flip) const;
	void getRotation(RotationMode &rotation) const;

	void reset();

	void applyHard();
	bool applySoft(SoftOpInternalMgr *op);

    private:
	void _setMaxImage(const Size &size, ImageType type);
	void _setHSRoi(const Roi &roi);
	void _setHSBin(const Bin &bin);
	void _setHSFlip(const Flip &flip);
	void _resetFlip();

#ifdef WITH_CONFIG
	class _ConfigHandler;
	CtConfig::ModuleTypeCallback* _getConfigHandler();
#endif //WITH_CONFIG

	HwDetInfoCtrlObj* 	m_hw_det;
	CtMaxImageSizeCB* 	m_cb_size;
	CtSwBinRoiFlip* 	m_sw;
	CtHwBinRoiFlip* 	m_hw;
	CtControl&		m_ct;

	Size			m_max_size;
	ImageType		m_img_type;
	ImageType		m_next_image_type;
	ImageOpMode		m_mode;
};
 
inline std::ostream& operator<<(std::ostream& os,const CtSwBinRoiFlip &binroi)
{
	os << "<"
	   << "m_max_size=" << binroi.m_max_size << ", "
	   << "m_size=" << binroi.m_size << ", "
	   << "m_bin=" << binroi.m_bin << ", "
	   << "m_roi=" << binroi.m_roi << ", "
	   << "m_max_roi=" << binroi.m_max_roi << ","
	   << "m_flip=" << binroi.m_flip
	   << ">";
	return os;
}
inline const char* convert_2_string(CtImage::ImageOpMode mode)
{
	const char *name = "Unknown";
	switch (mode) {
	case CtImage::HardOnly:    name = "HardOnly";    break;
	case CtImage::SoftOnly:    name = "SoftOnly";    break;
	case CtImage::HardAndSoft: name = "HardAndSoft"; break;
	}
	return name;

}
inline void convert_from_string(const std::string& val,
				CtImage::ImageOpMode& mode)
{
  std::string buffer = val;
  std::transform(buffer.begin(),buffer.end(),
		 buffer.begin(),::tolower);
  
  if(buffer == "hardonly") 		mode = CtImage::HardOnly;
  else if(buffer == "softonly") 	mode = CtImage::SoftOnly;
  else if(buffer == "hardandsoft") 	mode = CtImage::HardAndSoft;
  else
    {
      std::ostringstream msg;
      msg << "ImageOpMode can't be:" << DEB_VAR1(val);
      throw LIMA_EXC(Control,InvalidValue,msg.str());
    }
}
inline std::ostream& operator <<(std::ostream& os, CtImage::ImageOpMode mode)
{
  return os << convert_2_string(mode);
}

} // namespace lima

#endif // CTIMAGE_H
