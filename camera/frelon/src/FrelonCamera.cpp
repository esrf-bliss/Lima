#include "FrelonCamera.h"

using namespace lima::Frelon;
using namespace std;

Camera::Camera(Espia::SerialLine& espia_ser_line)
	: m_ser_line(espia_ser_line)
{

}

