from Lima import Core, Simulator

Core.DebParams.setTypeFlags(Core.DebParams.AllFlags)

frame_dim = Core.FrameDim(Core.Size(800, 600), Core.Bpp32F)

cam = Simulator.Camera()
getter = cam.getFrameGetter() 
getter.setFrameDim(frame_dim)
assert getter.getFrameDim() == frame_dim

hwint = Simulator.Interface(cam)
control = Core.CtControl(hwint)

img = control.image()
assert img.getMaxImageSize() == Core.Size(800, 600)
assert img.getImageDim() == frame_dim

###
img.reset()

# Rotate first, define ROI, unrotate
img.setRotation(Core.Rotation_90)
assert img.getMaxImageSize() == Core.Size(600, 800)

roi = Core.Roi(250,300,300,450)
img.setRoi(roi)
img.setRotation(Core.Rotation_0) # InvalidValue: Invalid corner coords

###
img.reset()

# Define ROI first, rotate
roi = Core.Roi(300,50,450,300)
img.setRoi(roi)
img.setRotation(Core.Rotation_90) # InvalidValue: Roi out of limits m_max_roi=<0,0>-<800x600>, roi=<250,300>-<300x450>

###
img.reset()

# Define ROI first, bin
roi = Core.Roi(300,50,450,300)
img.setRoi(roi)
img.setBin(Core.Bin(2, 2))
assert img.getMaxImageSize() == Core.Size(400, 300) # Fail
