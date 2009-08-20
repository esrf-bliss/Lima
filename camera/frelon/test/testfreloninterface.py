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


print "This is the End..."
