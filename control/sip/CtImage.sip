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
class CtSwBinRoiFlip {
%TypeHeaderCode
#include "lima/CtImage.h"
#include <sstream>
using namespace lima;
%End
    public:
	CtSwBinRoiFlip(Size& size);
	CtSwBinRoiFlip(Size& size, const Bin& bin, const Roi& roi,
		       const Flip& flip, RotationMode rotation, BinMode bin_mode);
	~CtSwBinRoiFlip();

	void setMaxSize(Size& size);
	void setBin(const Bin& bin);
	void setBinMode(BinMode aBType);
	void setRoi(const Roi& roi);
	void setFlip(const Flip& flip);
	void setRotation(RotationMode aRType);

	void resetBin();
	void resetBinMode();
	void resetRoi();
	void resetFlip();
	void resetRotation();
	void reset();

	const Bin&   	getBin() 	const;
	BinMode 	    getBinMode() 	const;
	const Roi&   	getRoi() 	const;
	const Size&  	getSize() 	const;
	const Flip&  	getFlip() 	const;
	RotationMode 	getRotation() 	const;

	SIP_PYOBJECT __repr__() const;
%MethodCode
	LIMA_REPR_CODE
%End

%PickleCode
	sipRes = Py_BuildValue("OOOOO",
		sipConvertFromType((void*)&sipCpp->getSize(), sipType_Size, NULL),
		sipConvertFromType((void*)&sipCpp->getBin(), sipType_Bin, NULL),
		sipConvertFromEnum(sipCpp->getBinMode(), sipType_BinMode),
		sipConvertFromType((void*)&sipCpp->getRoi(), sipType_Roi, NULL),
		sipConvertFromType((void*)&sipCpp->getFlip(), sipType_Flip, NULL),
		sipConvertFromEnum(sipCpp->getRotation(), sipType_RotationMode));
%End

};

class CtHwBinRoiFlip 
{
%TypeHeaderCode
#include "lima/CtImage.h"
using namespace lima;
%End
    public:
	CtHwBinRoiFlip(HwInterface *hw, CtSwBinRoiFlip *sw_bin_roi_flip, Size& size);
	~CtHwBinRoiFlip();

	bool hasBinCapability() const;
	bool hasRoiCapability() const;
	bool hasFlipCapability() const;

	void setMaxSize(const Size& size);
	void setBin(Bin& bin /In,Out/, bool round);
	void setRoi(Roi& roi /In,Out/, bool round);
	void setFlip(Flip& flip /In,Out/,bool mandatory);

	void resetBin();
	void resetRoi();
	void resetFlip();
	void reset();

	const Bin& getBin() const;
	const Roi& getSetRoi() const;
	const Roi& getRealRoi() const;
	const Size& getSize() const;
	const Size& getMaxRoiSize() const;
	const Flip& getFlip() const;

	void apply();
};

class CtImage {
%TypeHeaderCode
#include "lima/CtImage.h"
using namespace lima;
%End
    public:

	enum ImageOpMode {
		HardOnly,
		SoftOnly,
		HardAndSoft,
	};
		
	// Size, Type, Bin, XY from common

	CtImage(HwInterface *hw,CtControl&);
	~CtImage();

	void getMaxImageSize(Size& size /Out/) const;

	void getImageType(ImageType& imageType /Out/) const;
	void setImageType(ImageType imageType);

	void getHwImageDim(FrameDim& dim /Out/) const;
	void getImageDim(FrameDim& dim /Out/) const;
	// --- soft
	CtSwBinRoiFlip* getSoft() const;
	CtHwBinRoiFlip* getHard() const;

	// --- wizard
	void setMode(ImageOpMode mode);
	void getMode(ImageOpMode& mode /Out/) const;

	void setRoi(Roi &roi);
	void setBin(Bin &bin);
	void setBinMode(BinMode bin_mode);
	void setFlip(Flip &flip);
	void setRotation(RotationMode rotation);

	void resetRoi();
	void resetBin();
	void resetBinMode();
	void resetFlip();
	void resetRotation();

	// --- effective
	void getRoi(Roi& roi /Out/) const;
	void getBin(Bin& bin /Out/) const;
	void getBinMode(BinMode& bin_mode /Out/) const;
	void getFlip(Flip &flip /Out/) const;
	void getRotation(RotationMode &rotation /Out/) const;

	void reset();

	private:
	CtImage(const CtImage&);
};
