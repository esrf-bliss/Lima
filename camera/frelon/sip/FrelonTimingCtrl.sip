namespace Frelon
{


class TimingCtrl
{

%TypeHeaderCode
#include "FrelonTimingCtrl.h"
%End

 public:
	TimingCtrl(Frelon::Model& model, Frelon::SerialLine& ser_line);
	~TimingCtrl();

};


}; // namespace Frelon
