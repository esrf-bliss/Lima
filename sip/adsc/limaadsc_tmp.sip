%Module limaadsc 0

%ModuleHeaderCode
extern "C" { void lima_import_array();}
%End

%PostInitialisationCode
lima_import_array();
%End

%Import ../../third-party/Processlib/sip/processlib_tmp.sip
%Import ../core/limacore_tmp.sip
%Include ../../camera/adsc/sip/AdscInterface.sip
%Include ../../camera/adsc/sip/AdscCamera.sip
