
MpxTypes= ["DUMMY", "MPX2", "MXR2", "TPX1"]

def mpxVersion(type):
    if type not in MpxTypes:
	raise MaxipixError("Invalid Maxipix Type <%s>"%type)
    return MpxTypes.index(type)

class MpxError(Exception):
    def __init__(self, msg):
        self.message= msg
    def __str__(self):
        print "", self.message

