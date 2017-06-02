#include "RayonixHsBinCtrlObj.h"

using namespace lima;
using namespace lima::RayonixHs;

BinCtrlObj::BinCtrlObj(Camera *cam)
   : m_cam(cam) {
}

BinCtrlObj::~BinCtrlObj() {
}

void BinCtrlObj::setBin(const Bin& bin) {
   m_cam->setBin(bin);
}

void BinCtrlObj::getBin(Bin& bin) {
   m_cam->getBin(bin);
}

void BinCtrlObj::checkBin(Bin& bin) {
   m_cam->checkBin(bin);
}
