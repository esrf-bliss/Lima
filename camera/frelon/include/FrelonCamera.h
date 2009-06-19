#ifndef FRELONCAMERA_H
#define FRELONCAMERA_H

#include "FrelonSerialLine.h"

namespace lima
{

namespace Frelon
{

class Camera
{
	Camera(Espia::SerialLine& espia_ser_line);


 private:
	SerialLine m_ser_line;
};


} // namespace Frelon

} // namespace lima


#endif // FRELONCAMERA_H
