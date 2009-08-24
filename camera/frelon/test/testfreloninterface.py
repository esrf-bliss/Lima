import lima


print "Creating Espia.Dev"
edev = lima.Espia.Dev(0)

print "Creating Espia.Acq"
acq = lima.Espia.Acq(edev)

acqstat = acq.getStatus()
print "Whether the Acquisition is running : ", acqstat.running

print "Creating Espia.BufferMgr"
buffer_cb_mgr = lima.Espia.BufferMgr(acq)

print "Creating BufferCtrlMgr"
buffer_mgr = lima.BufferCtrlMgr(buffer_cb_mgr)

print "Creating Espia.SerialLine"
eser_line = lima.Espia.SerialLine(edev)

print "Creating Frelon.Camera"
cam = lima.Frelon.Camera(eser_line);

print "Creating the Hw Interface ... "
hw_inter = lima.Frelon.Interface(acq, buffer_mgr, cam)

print "Creating BufferSave"
buffer_save = lima.BufferSave(lima.BufferSave.EDF, "img", 0, ".edf", True, 1)

print "Getting HW detector info"
hw_det_info = hw_inter.getHwCtrlObj(lima.HwCap.DetInfo)
print type(hw_det_info)

print "Getting HW buffer"
hw_buffer = hw_inter.getHwCtrlObj(lima.HwCap.Buffer)
print type(hw_buffer)

print "Getting HW Sync"
hw_sync = hw_inter.getHwCtrlObj(lima.HwCap.Sync)
print type(hw_sync)

print "Getting HW Bin"
hw_bin = hw_inter.getHwCtrlObj(lima.HwCap.Bin)
print type(hw_bin)

print "Getting HW RoI"
hw_roi = hw_inter.getHwCtrlObj(lima.HwCap.Roi)
print type(hw_roi)

s = raw_input('Reset the hardware? (y/n):')
if s[0] == 'y' or s[0] == 'Y':
	hw_inter.reset(lima.HwInterface.HardReset)
	print "  Done!"


print "This is the End..."
