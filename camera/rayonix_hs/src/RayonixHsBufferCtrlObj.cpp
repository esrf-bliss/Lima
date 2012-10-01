#include "RayonixHsBufferCtrlObj.h"
#include "RayonixHsSyncCtrlObj.h"
#include "RayonixHsCamera.h"

using namespace lima;
using namespace lima::RayonixHs;

BufferCtrlObj::BufferCtrlObj(Camera *cam)
   : m_exposing(false),
     m_cam(cam) {

   DEB_CONSTRUCTOR();
}

void BufferCtrlObj::prepareAcq() {
   DEB_MEMBER_FUNCT();

   m_cam->prepareAcq();
}

void BufferCtrlObj::startAcq() {
   DEB_MEMBER_FUNCT();

   m_cam->startAcq();
}
