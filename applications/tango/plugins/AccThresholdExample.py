from Lima import Core

##@brief this is an plugin example for accumulation saturation callback
#
#To use this plugin, you have to set AccThresholdCallbackModule property in
#LimaCCD device to "AccThresholdExample".
#AccThresholdCallbackModule should be the filename of your plugin.
class _ExampleThreshold(Core.CtAccumulation.ThresholdCallback):
    Core.DEB_CLASS(Core.DebModApplication, '_ExampleThreshold')
    def __init__(self) :
        Core.CtAccumulation.ThresholdCallback.__init__(self)
    ##@brief the effective callback methode
    @Core.DEB_MEMBER_FUNCT
    def aboveMax(self,data,value) :
        deb.Trace('aboveMax: data=%s, value=%s' % (data,value))

##@brief this function is mandatory for an AccThresholdCallbackModule
#
def get_acc_threshold_callback() :
    return _ExampleThreshold()
