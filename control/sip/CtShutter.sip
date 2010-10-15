class CtShutter {
%TypeHeaderCode
#include <CtShutter.h>
#include <Constants.h>
#include <sstream>
using namespace lima;
%End
    public:

        CtShutter(HwInterface *hw);
        ~CtShutter();

        bool hasCapability() const;

        void getModeList(ShutterModeList& mode_list /Out/) const;
    
        bool checkMode(ShutterMode shut_mode) const;
        void setMode(ShutterMode  shut_mode);
        void getMode(ShutterMode& shut_mode /Out/) const;

        void setState(bool  shut_open);
        void getState(bool& shut_open /Out/) const;

        void setOpenTime (double  shut_open_time);
        void getOpenTime (double& shut_open_time /Out/) const;
        void setCloseTime(double  shut_close_time);
        void getCloseTime(double& shut_close_time /Out/) const;
};
