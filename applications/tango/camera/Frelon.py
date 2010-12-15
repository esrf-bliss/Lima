#----------------------------------------------------------------------------
# Plugins
#----------------------------------------------------------------------------
from Lima import Core
from Lima import Frelon
 
_FrelonAcq = None

def get_control(espia_dev_nb = 0,**keys) :
    global _FrelonAcq
    if _FrelonAcq is None:
	_FrelonAcq = Frelon.FrelonAcq(espia_dev_nb)
    return _FrelonAcq.getGlobalControl() 





