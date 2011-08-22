%Module limaadsc 0

%ModuleHeaderCode
extern "C" { void lima_import_array();}
%End

%PostInitialisationCode
lima_import_array();
%End
