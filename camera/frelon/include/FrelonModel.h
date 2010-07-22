#ifndef FRELONMODEL_H
#define FRELONMODEL_H

#include "Frelon.h"

namespace lima
{

namespace Frelon
{

class Model
{
	DEB_CLASS_NAMESPC(DebModCamera, "Model", "Frelon");

 public:
	Model();
	~Model();

	void setVersion(const std::string& ver);
	void getVersion(std::string& ver);

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

	double getPixelSize();

	std::string getName();

 private:
	void checkValid();
	int getSerialNbParam(SerNbParam param);

	std::string m_ver;
	int m_complex_ser_nb;
};


} // namespace Frelon

} // namespace lima


#endif // FRELONMODEL_H
