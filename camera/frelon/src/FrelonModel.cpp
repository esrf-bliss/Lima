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
#include "FrelonModel.h"
#include "RegEx.h"

using namespace lima;
using namespace lima::Frelon;
using namespace std;

Firmware::Firmware()
{
	DEB_CONSTRUCTOR();

	reset();
}


Firmware::Firmware(const string& ver)
{
	DEB_CONSTRUCTOR();
	DEB_PARAM() << DEB_VAR1(ver);

	reset();
	setVersionStr(ver);
}


Firmware::~Firmware()
{
	DEB_DESTRUCTOR();

	reset();
}


void Firmware::reset()
{
	DEB_MEMBER_FUNCT();

	m_major = m_minor = 0;
	m_rel.clear();
}

void Firmware::setVersionStr(const string& ver)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(ver);

	RegEx re("(?P<major>[0-9]+)\\.(?P<minor>[0-9]+)(?P<rel>[a-z]+)?");
	RegEx::FullNameMatchType match;

	if (!re.matchName(ver, match))
		THROW_HW_ERROR(InvalidValue) << "Invalid firmware "
					     << DEB_VAR1(ver);

	m_major = atoi(string(match["major"]).c_str());
	m_minor = atoi(string(match["minor"]).c_str());
	m_rel = match["rel"];

	if (!isValid()) {
		reset();
		THROW_HW_ERROR(InvalidValue) << "Invalid firmware "
					     << DEB_VAR1(ver);
	}
}

void Firmware::getVersionStr(string& ver) const
{
	DEB_MEMBER_FUNCT();

	ostringstream os;
	if (isValid())
		os << m_major << "." << m_minor << m_rel;
	else
		os << "Unknown";

	ver = os.str();
	DEB_RETURN() << DEB_VAR1(ver);
}

bool Firmware::isValid() const
{
	DEB_MEMBER_FUNCT();

	bool valid = (m_major > 0) || (m_minor > 0);
	DEB_RETURN() << DEB_VAR1(valid);
	return valid;
}

int Firmware::getMajor() const
{
	DEB_MEMBER_FUNCT();
	DEB_RETURN() << DEB_VAR1(m_major);
	return m_major;
}

int Firmware::getMinor() const
{
	DEB_MEMBER_FUNCT();
	DEB_RETURN() << DEB_VAR1(m_minor);
	return m_minor;
}

string Firmware::getRelease() const
{
	DEB_MEMBER_FUNCT();
	DEB_RETURN() << DEB_VAR1(m_rel);
	return m_rel;
}

void Firmware::checkValid()
{
	DEB_MEMBER_FUNCT();

	if (!isValid())
		THROW_HW_ERROR(InvalidValue) 
			<< "Frelon Firmware not fully initialised yet";
}

Model::Model()
{
	DEB_CONSTRUCTOR();

	reset();
}

Model::~Model()
{
	DEB_DESTRUCTOR();
}

void Model::setVersionStr(const std::string& ver)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(ver);
	m_firmware.setVersionStr(ver);
}

Firmware& Model::getFirmware()
{
	return m_firmware;
}

void Model::setComplexSerialNb(int complex_ser_nb)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(complex_ser_nb);
	m_complex_ser_nb = complex_ser_nb;
}

void Model::getComplexSerialNb(int& complex_ser_nb)
{
	DEB_MEMBER_FUNCT();
	complex_ser_nb = m_complex_ser_nb;
	DEB_RETURN() << DEB_VAR1(complex_ser_nb);
}

void Model::reset()
{
	DEB_MEMBER_FUNCT();

	m_firmware.reset();
	m_complex_ser_nb = 0;
}

bool Model::isValid()
{
	DEB_MEMBER_FUNCT();

	bool valid = (m_complex_ser_nb > 0) && m_firmware.isValid();
	DEB_RETURN() << DEB_VAR1(valid);
	return valid;
}

void Model::checkValid()
{
	DEB_MEMBER_FUNCT();

	if (!isValid())
		THROW_HW_ERROR(InvalidValue) 
			<< "Frelon model not fully initialised yet";
}

int Model::getSerialNbParam(SerNbParam param)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(DEB_HEX(param));

	int val = m_complex_ser_nb & int(param);
	DEB_RETURN() << DEB_VAR1(val);
	return val;
}

int Model::getSerialNb()
{
	DEB_MEMBER_FUNCT();
	checkValid();

	int ser_nb = getSerialNbParam(SerNb);
	DEB_RETURN() << DEB_VAR1(ser_nb);
	return ser_nb;
}

bool Model::isSPB1()
{
	DEB_MEMBER_FUNCT();

	bool frelon_spb1 = !isSPB2();
	DEB_RETURN() << DEB_VAR1(frelon_spb1);
	return frelon_spb1;
}
	
bool Model::isSPB2()
{
	DEB_MEMBER_FUNCT();
	checkValid();

	bool frelon_spb2 = bool(getSerialNbParam(SPB2Sign));
	DEB_RETURN() << DEB_VAR1(frelon_spb2);
	return frelon_spb2;
}
	
int Model::getAdcBits()
{
	DEB_MEMBER_FUNCT();

	int adc_bits;
	if (isSPB1())
		adc_bits = bool(getSerialNbParam(SPB1Adc16)) ? 16 : 14;
	else
		adc_bits = 16;
	DEB_RETURN() << DEB_VAR1(adc_bits);
	return adc_bits;
}

ChipType Model::getChipType()
{
	DEB_MEMBER_FUNCT();

	ChipType chip_type;
	if (isSPB1())
		chip_type = bool(getSerialNbParam(SPB1Kodak)) ? Kodak : Atmel;
	else
		chip_type = ChipType(getSerialNbParam(SPB2Type) >> 12);
	DEB_RETURN() << DEB_VAR1(chip_type);
	return chip_type;
}

bool Model::hasTaper()
{
	DEB_MEMBER_FUNCT();

	bool taper = bool(getSerialNbParam(Taper));
	DEB_RETURN() << DEB_VAR1(taper);
	return taper;
}

bool Model::hasModesAvail()
{
	DEB_MEMBER_FUNCT();

	bool avail_modes = (isSPB2() && (m_firmware >= Firmware("2.1b")));
	DEB_RETURN() << DEB_VAR1(avail_modes);
	return avail_modes;
}

bool Model::hasTimeCalc()
{
	DEB_MEMBER_FUNCT();

	bool time_calc = (isSPB2() && (m_firmware >= Firmware("2.1b")));
	DEB_RETURN() << DEB_VAR1(time_calc);
	return time_calc;
}

double Model::getPixelSize()
{
	DEB_MEMBER_FUNCT();

	ChipType chip_type = getChipType();
	double pixel_size = ChipPixelSizeMap[chip_type];
	DEB_RETURN() << DEB_VAR1(pixel_size);
	return pixel_size;
}

string Model::getName()
{
	DEB_MEMBER_FUNCT();
	checkValid();

	map<ChipType, string> chip_model;
	chip_model[Atmel] = "A7899";
	chip_model[Kodak] = "K4320";
	chip_model[E2V]   = "E230-42";

	string hd = isSPB2() ? "HD " : "";
	string name = hd + chip_model[getChipType()];

	if (isSPB1() && (getAdcBits() == 16))
		name += " 16bit";

	if (hasTaper())
		name += "T";

	DEB_RETURN() << DEB_VAR1(name);
	return name;
}
