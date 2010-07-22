#include "FrelonModel.h"

using namespace lima;
using namespace lima::Frelon;
using namespace std;

Model::Model()
{
	DEB_CONSTRUCTOR();

	reset();
}

Model::~Model()
{
	DEB_DESTRUCTOR();
}

void Model::setVersion(const std::string& ver)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(ver);
	m_ver = ver;
}

void Model::getVersion(std::string& ver)
{
	DEB_MEMBER_FUNCT();
	ver = m_ver;
	DEB_RETURN() << DEB_VAR1(ver);
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

	m_ver.clear();
	m_complex_ser_nb = 0;
}

bool Model::isValid()
{
	DEB_MEMBER_FUNCT();

	bool valid = (m_complex_ser_nb > 0) && !m_ver.empty();
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
	checkValid();

	bool taper = bool(getSerialNbParam(Taper));
	DEB_RETURN() << DEB_VAR1(taper);
	return taper;
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
