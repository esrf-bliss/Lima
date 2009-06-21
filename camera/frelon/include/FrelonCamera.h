#ifndef FRELONCAMERA_H
#define FRELONCAMERA_H

#include "FrelonSerialLine.h"

namespace lima
{

namespace Frelon
{

class Camera
{
 public:
	Camera(Espia::SerialLine& espia_ser_line);

	SerialLine& getSerialLine();

	void writeRegister(Reg reg, int  val);
	void readRegister (Reg reg, int& val);

	void hardReset();
	void getVersion(std::string& ver);

 private:
	void sendCmd(Cmd cmd);

	SerialLine m_ser_line;
};


} // namespace Frelon

} // namespace lima


#endif // FRELONCAMERA_H
