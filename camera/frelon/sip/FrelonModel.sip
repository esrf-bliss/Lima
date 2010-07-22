namespace Frelon
{

class Model
{

%TypeHeaderCode
#include "FrelonModel.h"
%End

 public:
	Model();
	~Model();

	void setVersion(const std::string& ver);
	void getVersion(std::string& ver /Out/);

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

	double getPixelSize();

	std::string getName();

};

}; // namespace Frelon