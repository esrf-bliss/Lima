#include "lima/HwVideoCtrlObj.h"
using namespace lima;

HwVideoCtrlObj::HwVideoCtrlObj() 
  : m_image_cbk(NULL) 
{
}
      
HwVideoCtrlObj::~HwVideoCtrlObj() 
{
}

bool HwVideoCtrlObj::checkAutoGainMode(AutoGainMode mode) const
{
  DEB_MEMBER_FUNCT();
  bool checkFlag = mode == OFF;
  DEB_RETURN() << DEB_VAR1(checkFlag);
  return checkFlag;
}

void HwVideoCtrlObj::setAutoGainMode(AutoGainMode mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(mode);
  if(!checkAutoGainMode(mode))
    THROW_HW_ERROR(NotSupported) << DEB_VAR1(mode);
  setHwAutoGainMode(mode);
  m_auto_gain_mode = mode;
}
/** @brief this method should be redefined in the subclass if 
 *  the camera can managed auto exposure
 */
void HwVideoCtrlObj::setHwAutoGainMode(AutoGainMode)
{
}

void HwVideoCtrlObj::getAutoGainMode(AutoGainMode& mode) const
{
  DEB_MEMBER_FUNCT();
  DEB_RETURN() << DEB_VAR1(m_auto_gain_mode);
  mode = m_auto_gain_mode;
}

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

HwBufferCtrlObj& HwVideoCtrlObj::getHwBufferCtrlObj() 
{
  return m_hw_buffer_ctrl_obj;
}

StdBufferCbMgr& HwVideoCtrlObj::getBuffer() 
{
  return m_hw_buffer_ctrl_obj.getBuffer();
}

const char* lima::convert_2_string(HwVideoCtrlObj::AutoGainMode mode)
{
  const char* aHumanReadablePt;
  switch(mode)
    {
    case HwVideoCtrlObj::OFF:		aHumanReadablePt = "OFF";	break;
    case HwVideoCtrlObj::ON:		aHumanReadablePt = "ON";	break;
    default:
      aHumanReadablePt = "UNKNOWN";
      break;
    }
  return aHumanReadablePt;
}

void lima::convert_from_string(const std::string& val,
			       HwVideoCtrlObj::AutoGainMode& mode)
{
  std::string buffer = val;
  std::transform(buffer.begin(),buffer.end(),
		 buffer.begin(),::tolower);

  if(buffer == "off") mode = HwVideoCtrlObj::OFF;
  else if(buffer == "on") mode = HwVideoCtrlObj::ON;
  else
    {
      std::ostringstream msg;
      msg << "AutoGainMode can't be:" << DEB_VAR1(val);
      throw LIMA_EXC(Common,InvalidValue,msg.str());
    }
}

std::ostream& lima::operator<<(std::ostream& os,const HwVideoCtrlObj::AutoGainMode& mode)
{
  return os << convert_2_string(mode);
}
