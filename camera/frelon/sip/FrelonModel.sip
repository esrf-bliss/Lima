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
