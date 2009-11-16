#define SIP_CONVERT_CTRLOBJECT_TO_SIPOBJECT(cppObject) \
{\
\
switch(cppObject->getType())\
{\
case HwCap::DetInfo:\
{\
  HwDetInfoCtrlObj *aCtrlObjPt = NULL;\
  if(cppObject->getCtrlObj(aCtrlObjPt))\
    sipRes = sipConvertFromInstance(aCtrlObjPt,sipClass_HwDetInfoCtrlObj,NULL); \
  break;\
}\
case HwCap::Buffer:\
{\
  HwBufferCtrlObj *aCtrlObjPt = NULL;\
  if(cppObject->getCtrlObj(aCtrlObjPt))\
    sipRes = sipConvertFromInstance(aCtrlObjPt,sipClass_HwBufferCtrlObj,NULL); \
  break;\
}\
case HwCap::Sync:\
{\
  HwSyncCtrlObj *aCtrlObjPt = NULL;\
  if(cppObject->getCtrlObj(aCtrlObjPt))\
    sipRes = sipConvertFromInstance(aCtrlObjPt,sipClass_HwSyncCtrlObj,NULL); \
  break;\
}\
case HwCap::Bin:\
{\
  HwBinCtrlObj *aCtrlObjPt = NULL;\
  if(cppObject->getCtrlObj(aCtrlObjPt))\
    sipRes = sipConvertFromInstance(aCtrlObjPt,sipClass_HwBinCtrlObj,NULL); \
  break;\
}\
case HwCap::Roi:\
{\
  HwRoiCtrlObj *aCtrlObjPt = NULL;\
  if(cppObject->getCtrlObj(aCtrlObjPt))\
    sipRes = sipConvertFromInstance(aCtrlObjPt,sipClass_HwRoiCtrlObj,NULL); \
  break;\
}\
case HwCap::Flip:\
{\
  HwFlipCtrlObj *aCtrlObjPt = NULL;\
  if(cppObject->getCtrlObj(aCtrlObjPt))\
    sipRes = sipConvertFromInstance(aCtrlObjPt,sipClass_HwFlipCtrlObj,NULL); \
  break;\
}\
default:\
std::cerr << "HwCap getCtrlObj : sip wrapping Type -> " << cppObject->getType() << " not yet managed" << std::endl;break;\
}\
}
