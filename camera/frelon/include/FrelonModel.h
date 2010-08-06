#ifndef FRELONMODEL_H
#define FRELONMODEL_H

#include "Frelon.h"

namespace lima
{

namespace Frelon
{

class Firmware
{
	DEB_CLASS_NAMESPC(DebModCamera, "Firmware", "Frelon");

 public:
	Firmware();
	Firmware(const std::string& ver);
	~Firmware();

	void setVersionStr(const std::string& ver);
	void getVersionStr(std::string& ver) const;

	void reset();
	bool isValid() const;

	int getMajor() const; 
	int getMinor() const;
	std::string getRelease() const;

 private:
	void checkValid();

	int m_major;
	int m_minor;
	std::string m_rel;
};

inline bool operator ==(const Firmware& f1, const Firmware& f2)
{
	return ((f1.getMajor() == f2.getMajor()) && 
		(f1.getMinor() == f2.getMinor()) &&
		(f1.getRelease() == f2.getRelease()));
}

inline bool operator !=(const Firmware& f1, const Firmware& f2)
{
	return !(f1 == f2);
}

inline bool operator <(const Firmware& f1, const Firmware& f2)
{
	if (f1.getMajor() < f2.getMajor())
		return true;
	if (f1.getMajor() > f2.getMajor())
		return false;
	if (f1.getMinor() < f2.getMinor())
		return true;
	if (f1.getMinor() > f2.getMinor())
		return false;
	return (f1.getRelease() < f2.getRelease());
}

inline bool operator >(const Firmware& f1, const Firmware& f2)
{
	return !((f1 == f2) || (f1 < f2));
}

inline bool operator <=(const Firmware& f1, const Firmware& f2)
{
	return ((f1 == f2) || (f1 < f2));
}

inline bool operator >=(const Firmware& f1, const Firmware& f2)
{
	return !(f1 < f2);
}


class Model
{
	DEB_CLASS_NAMESPC(DebModCamera, "Model", "Frelon");

 public:
	Model();
	~Model();

	void setVersionStr(const std::string& ver);
	Firmware& getFirmware();

	void setComplexSerialNb(int  complex_ser_nb);
	void getComplexSerialNb(int& complex_ser_nb);

	void reset();
	bool isValid();

	int  getSerialNb();
	bool isSPB1();
	bool isSPB2();
	int  getAdcBits();
	ChipType getChipType();
	bool hasTaper();
	bool hasModesAvail();
	bool hasTimeCalc();

	double getPixelSize();

	std::string getName();

 private:
	void checkValid();
	int getSerialNbParam(SerNbParam param);

	Firmware m_firmware;
	int m_complex_ser_nb;
};


} // namespace Frelon

} // namespace lima


#endif // FRELONMODEL_H
