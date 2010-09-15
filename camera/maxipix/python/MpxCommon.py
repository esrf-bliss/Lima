from limamaxipix import Maxipix
import types

MpxVersion= [	Maxipix.MaxipixDet.DUMMY,
		Maxipix.MaxipixDet.MPX2,
		Maxipix.MaxipixDet.MXR2,
		Maxipix.MaxipixDet.TPX1 ]
MpxTypes= ["DUMMY", "MPX2", "MXR2", "TPX1"]

MpxPolarity= [	Maxipix.MaxipixDet.NEGATIVE,
		Maxipix.MaxipixDet.POSITIVE ]
MpxPolarityTypes= ["NEGATIVE","POSITIVE"]

MpxFillMode= [ Maxipix.MaxipixReconstruction.RAW,
               Maxipix.MaxipixReconstruction.ZERO,
               Maxipix.MaxipixReconstruction.DISPATCH,
               Maxipix.MaxipixReconstruction.MEAN ] 

MpxFillModeTypes= ["RAW","ZERO","DISPATCH","MEAN"]


def mpxFillMode(mode):
    if type(mode)==types.StringType:
	if mode.upper() not in MpxFillModeTypes:
	    raise MpxError("Invalid Maxipix FillMode String <%s>"%mode)
	return MpxFillMode[MpxFillModeTypes.index(mode.upper())]
    elif type(mode)==types.IntType:
	if mode not in range(len(MpxFillMode)):
	    raise MpxError("Invalid Maxipix FillMode value <%d>"%mode)
	return MpxFillMode[mode]
    else:	
	if mode not in MpxFillMode:
	    raise MpxError("Invalid Maxipix FillMode <%s>"%str(mode))
	return mode

def mpxPolarity(polarity):
    if type(polarity)==types.StringType:
	if polarity.upper() not in MpxPolarityTypes:
	    raise MpxError("Invalid Maxipix Polarity String <%s>"%polarity)
	return MpxPolarity[MpxPolarityTypes.index(polarity.upper())]
    elif type(polarity)==types.IntType:
	if polarity not in range(len(MpxPolarity)):
	    raise MpxError("Invalid Maxipix Polarity value <%d>"%polarity)
	return MpxPolarity[polarity]
    else:	
	if polarity not in MpxPolarity:
	    raise MpxError("Invalid Maxipix Polarity <%s>"%str(polarity))
	return polarity

def mpxVersion(version):
    if type(version)==types.StringType:
	if version.upper() not in MpxTypes:
	    raise MpxError("Invalid Maxipix Version String <%s>"%version)
	return MpxVersion[MpxTypes.index(version.upper())]
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
        return self.message

