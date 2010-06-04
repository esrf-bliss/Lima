from lima import Maxipix
import types

MpxVersion= [	Maxipix.MaxipixDet.DUMMY,
		Maxipix.MaxipixDet.MPX2,
		Maxipix.MaxipixDet.MXR2,
		Maxipix.MaxipixDet.TPX1 ]
MpxTypes= ["DUMMY", "MPX2", "MXR2", "TPX1"]

MpxPolarity= [	Maxipix.MaxipixDet.NEGATIVE,
		Maxipix.MaxipixDet.POSITIVE ]

def mpxPolarity(polarity):
    if polarity not in [0,1]:
	raise MpxError("Invalid Maxipix Polarity")
    return MpxPolarity[polarity]

def mpxVersion(version):
    if type(version)==types.StringType:
	if version not in MpxTypes:
	    raise MpxError("Invalid Maxipix Version String <%s>"%version)
	return MpxVersion[MpxTypes.index(version)]
    elif type(version)==types.IntType:
	if version not in range(len(MpxVersion)):
	    raise MpxError("Invalid Maxipix Version value <%d>"%version)
	return MpxVersion[version]
    else:	
	if version not in MpxVersion:
	    raise MpxError("Invalid Maxipix Version <%s>"%str(version))
	return version

class MpxError(Exception):
    def __init__(self, msg):
        self.message= msg
    def __str__(self):
        print "", self.message

