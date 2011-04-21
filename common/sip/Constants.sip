enum AlignDir {
	Floor, Ceil,
};

enum ImageType {
  Bpp8, Bpp8S, Bpp10, Bpp10S, Bpp12, Bpp12S, Bpp14, Bpp14S, Bpp16, Bpp16S, Bpp32, Bpp32S
};

enum AcqMode {
	Single, Accumulation, Concatenation,
};

enum TrigMode {
	IntTrig, IntTrigMult,
	ExtTrigSingle, ExtTrigMult,
	ExtGate, ExtStartStop,
};

enum BufferMode {
	Linear, Circular,
};

enum ShutterMode {
  ShutterManual, ShutterAutoFrame, ShutterAutoSequence
};

%MappedType ShutterModeList
{
%TypeHeaderCode
#include <vector>
%End

%ConvertToTypeCode
  if(sipIsErr == NULL)
    {
      bool aReturnFlag = PySequence_Check(sipPy);
      for(int i = 0;aReturnFlag && i < PySequence_Size(sipPy);++i)
	aReturnFlag = sipCanConvertToEnum(PySequence_Fast_GET_ITEM(sipPy,i),
					  sipType_ShutterMode);
      return aReturnFlag;
    }
  std::vector<ShutterMode> *modeListPt = new std::vector<ShutterMode>();
  for(int i = 0;i < PySequence_Size(sipPy);++i)
    modeListPt->push_back(static_cast<ShutterMode>(SIPLong_AsLong(PySequence_Fast_GET_ITEM(sipPy,i))));
  *sipCppPtr = modeListPt;
  return sipGetState(sipTransferObj);
%End

%ConvertFromTypeCode
  PyObject *l;
  if(!(l = PyList_New(sipCpp->size())))
    return NULL;

  int listPos = 0;
  for(std::vector<ShutterMode>::iterator i = sipCpp->begin();
      i != sipCpp->end();++i,++listPos)
    {
      PyObject *tobj = sipConvertFromEnum(*i, sipType_ShutterMode);
      if(!tobj)
        {
	  Py_DECREF(l);
	  return NULL;
        }
      PyList_SET_ITEM(l, listPos, tobj);
    }
  return l;
%End
};

enum AcqStatus {
	AcqReady, AcqRunning, AcqFault,
};

enum DetStatus {
	DetIdle			= 0x00,
	DetFault		= 0x01, 
	DetWaitForTrigger	= 0x02,
	DetShutterOpen		= 0x04,
	DetExposure		= 0x08,
	DetShutterClose		= 0x10,
	DetChargeShift		= 0x20,
	DetReadout		= 0x40,
	DetLatency		= 0x80,
};

enum VideoMode {Y8,Y16,Y32,Y64,
		RGB555,RGB565,
		RGB24,RGB32,
		BGR24,BGR32,
		BAYER_RG8,BAYER_RG16,
		I420,YUV411,YUV422,YUV444};