#----------------------------------------------------------------------------
# Plugins
#----------------------------------------------------------------------------
from Lima import Core
from Lima import Simulator
 
_SimuInterface = None


def get_control(**keys) :
    global _SimuInterface
    if _SimuInterface is None:
	simu = Simulator.Simulator()
        _SimuInterface = Simulator.SimuHwInterface(simu)
	_SimuInterface._ref_interface = simu
    return Core.CtControl(_SimuInterface)



