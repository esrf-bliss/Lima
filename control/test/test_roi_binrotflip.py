import time
from Lima import Core, Simulator

#Core.DebParams.setTypeFlags(Core.DebParams.AllFlags)

cam = Simulator.Camera()

#frame_dim = Core.FrameDim(Core.Size(800, 600), Core.Bpp32)
frame_dim = Core.FrameDim(Core.Size(600, 600), Core.Bpp32)
cam = Simulator.Camera()
getter = cam.getFrameGetter()
getter.setFrameDim(frame_dim)

hwint = Simulator.Interface(cam)
control = Core.CtControl(hwint)

acq = control.acquisition()
acq.setAcqNbFrames(1)
acq.setAcqExpoTime(0.001)

img = control.image()
#roi = Core.Roi(300,50,450,300)
roi = Core.Roi(160, 60, 120, 200)
img.setRoi(roi)

rots = [Core.Rotation_0, Core.Rotation_90, Core.Rotation_180, Core.Rotation_270]
binnings = [Core.Bin(1,1), Core.Bin(1,2), Core.Bin(2,1), Core.Bin(2,2)]
flips = [Core.Flip(False, False), Core.Flip(False, True), Core.Flip(True, False), Core.Flip(True, True)]

# Python function to print permutations of a given list (from SO)
def permutation(lst):
    if len(lst) == 0:
        return []

    if len(lst) == 1:
        return [lst]

    l = []
    for i in range(len(lst)):
       m = lst[i]

       remLst = lst[:i] + lst[i+1:]

       for p in permutation(remLst):
           l.append([m] + p)
    return l

permutations = permutation(['b', 'f', 'r'])

# For every permutation of every Bin Rot Flip parameters applied in any order
for rot in rots:
    for bin in binnings:
        for flip in flips:
            for perm in permutations:

                try:
                    for op in perm:
                        if op == 'f':
                            img.setFlip(flip)
                        elif op == 'b':
                            img.setBin(bin)
                        elif op == 'r':
                            img.setRotation(rot)

                    #print(perm, flip, rot, bin, img.getRoi(), " - OK")

                except Exception:
                    print(perm, flip, rot, bin, img.getRoi(), " - FAILED")

img.resetBin()
img.resetFlip()
img.resetRotation()

assert img.getRoi() == roi
