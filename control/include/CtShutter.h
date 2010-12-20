#ifndef CTSHUTTER_H
#define CTSHUTTER_H

#include "Compatibility.h"
#include "Constants.h"
#include "SizeUtils.h"
#include "HwInterface.h"
#include "HwShutterCtrlObj.h"

#include <vector>

namespace lima {

  class DLL_EXPORT CtShutter 
  {
    DEB_CLASS_NAMESPC(DebModControl,"Shutter","Control");

  public:
    
    CtShutter(HwInterface *hw);
    ~CtShutter();
    bool hasCapability() const;

    void getModeList(ShutterModeList& mode_list) const;
    
    bool checkMode(ShutterMode shut_mode) const;
    void setMode(ShutterMode  shut_mode);
    void getMode(ShutterMode& shut_mode) const;

    void setState(bool  shut_open);
    void getState(bool& shut_open) const;

    void setOpenTime (double  shut_open_time);
    void getOpenTime (double& shut_open_time) const;
    void setCloseTime(double  shut_close_time);
    void getCloseTime(double& shut_close_time) const;

  private:
    bool 		m_has_shutter;
    HwShutterCtrlObj* 	m_hw_shutter;
  };
  
} // namespace lima

#endif // CTSHUTTER_H
