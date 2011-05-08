#include "HwVideoCtrlObj.h"
using namespace lima;

void HwVideoCtrlObj::registerImageCallback(HwVideoCtrlObj::ImageCallback &cb)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(&cb,m_image_cbk);

  if(m_image_cbk)
    THROW_HW_ERROR(InvalidValue) << "An ImageCallback already registered";

  m_image_cbk = &cb;
}

void HwVideoCtrlObj::unregisterImageCallback(HwVideoCtrlObj::ImageCallback &cb)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(&cb,m_image_cbk);

  if(m_image_cbk != &cb)
    THROW_HW_ERROR(InvalidValue) << "Requested ImageCallback not registered";
  
  m_image_cbk = NULL;
}
