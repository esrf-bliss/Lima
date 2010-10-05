#ifndef FRELONTIMINGCTRL_H
#define FRELONTIMINGCTRL_H

#include "FrelonModel.h"
#include "FrelonSerialLine.h"

namespace lima
{

namespace Frelon
{

class TimingCtrl
{
	DEB_CLASS_NAMESPC(DebModCamera, "TimingCtrl", "Frelon");

 public:
	TimingCtrl(Model& model, SerialLine& ser_line);
	~TimingCtrl();

 private:
	Model& m_model;
	SerialLine& m_ser_line;
};


} // namespace Frelon

} // namespace lima


#endif // FRELONTIMINGCTRL_H
