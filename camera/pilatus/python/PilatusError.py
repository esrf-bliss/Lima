class PilatusError(Exception) :
    def __init__(self,msg) :
        self.__message = msg
    def __str__(self) :
        return self.__message
