#ifndef SIMUHWINTERFACE_H
#define SIMUHWINTERFACE_H

#include "HwInterface.h"
#include "Simulator.h"

namespace lima
{

class SimuHwInterface: public HwInterface
{
 public:
	SimuHwInterface(Simulator& simu);
	virtual ~SimuHwInterface();

	virtual const CapList& getCapList() const;

	virtual void reset(ResetLevel reset_level);
	virtual void prepareAcq();
	virtual void startAcq();
	virtual void stopAcq();
	virtual void getStatus(StatusType& status);
	virtual int getNbAcquiredFrames();
	virtual double getStartTimeStamp();

 private:
	Simulator& m_simu;
	CapList m_cap_list;
};



}

#endif // SIMUHWINTERFACE_H
