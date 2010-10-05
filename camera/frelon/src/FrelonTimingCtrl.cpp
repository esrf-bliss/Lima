#include "FrelonTimingCtrl.h"

using namespace lima;
using namespace lima::Frelon;
using namespace std;

TimingCtrl::TimingCtrl(Model& model, SerialLine& ser_line)
	: m_model(model), m_ser_line(ser_line)
{
	DEB_CONSTRUCTOR();
}

TimingCtrl::~TimingCtrl()
{
	DEB_DESTRUCTOR();
}
