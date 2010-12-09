from Lima import Core

class _ExampleThreshold(Core.CtAccumulation.ThresholdCallback):
    Core.DEB_CLASS(Core.DebModApplication, '_ExempleThreshold')
    def __init__(self) :
        Core.CtAccumulation.ThresholdCallback.__init__(self)
        
    @Core.DEB_MEMBER_FUNCT
    def aboveMax(self,data,value) :
        deb.Param('aboveMax: ' +
                  'data=%s, value=%s' % \
                  (data,value))

def get_acc_threshold_callback() :
    return _ExempleThreshold()
