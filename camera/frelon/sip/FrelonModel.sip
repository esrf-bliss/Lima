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
namespace Frelon
{

class Firmware
{

%TypeHeaderCode
#include "FrelonModel.h"
%End

 public:
	Firmware();
	Firmware(const std::string& ver);
	~Firmware();

	void setVersionStr(const std::string& ver);
	void getVersionStr(std::string& ver /Out/) const;

	void reset();
	bool isValid() const;

	int getMajor() const;
	int getMinor() const;
	std::string getRelease() const;

};


class Model
{

%TypeHeaderCode
#include "FrelonModel.h"
%End

 public:
	Model();
	~Model();

	void setVersionStr(const std::string& ver);
	Frelon::Firmware& getFirmware();

	void setComplexSerialNb(int  complex_ser_nb);
	void getComplexSerialNb(int& complex_ser_nb /Out/);

	void reset();
	bool isValid();

	int  getSerialNb();
	bool isSPB1();
	bool isSPB2();
	int  getAdcBits();
	int  getChipType();
	bool hasTaper();
	bool hasModesAvail();
	bool hasTimeCalc();

	double getPixelSize();

	std::string getName();

};

}; // namespace Frelon
