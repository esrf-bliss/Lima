# Microsoft Developer Studio Project File - Name="YAT_Shared" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=YAT_Shared - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "YAT_Shared.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "YAT_Shared.mak" CFG="YAT_Shared - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "YAT_Shared - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "YAT_Shared - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "YAT_Shared - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "YAT_Shared___Win32_Release"
# PROP BASE Intermediate_Dir "YAT_Shared___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "YAT\shared\release"
# PROP Intermediate_Dir "YAT\shared\release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "YAT_BUILD" /D "YAT_DLL" /YX /FD /c
# ADD CPP /MD /W3 /GR /GX /Z7 /O2 /I "..\..\include" /I "..\..\src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "YAT_BUILD" /D "YAT_DLL" /D "_EVAL_PERF_" /FR /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /dll /pdb:"../../bin/msvc-6.0/libYAT.pdb" /machine:I386 /out:"../../bin/msvc-6.0/libYAT.dll" /implib:"../../lib/shared/msvc-6.0/libYAT.lib"
# SUBTRACT LINK32 /verbose /pdb:none

!ELSEIF  "$(CFG)" == "YAT_Shared - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "YAT\shared\debug"
# PROP Intermediate_Dir "YAT\shared\debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "YAT_BUILD" /D "YAT_DLL" /D "YAT_ENABLE_LOG" /YX /FD /GZ /c
# ADD CPP /MDd /W3 /GR /GX /Z7 /Od /I "..\..\include" /I "..\..\src" /D "_DEBUG" /D "YAT_ENABLE_LOG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "YAT_BUILD" /D "YAT_DLL" /D "_EVAL_PERF_" /FR /FD /GZ /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /dll /incremental:no /pdb:"../../bin/msvc-6.0/libYATd.pdb" /debug /machine:I386 /out:"../../bin/msvc-6.0/libYATd.dll" /implib:"../../lib/shared/msvc-6.0/libYATd.lib" /pdbtype:sept
# SUBTRACT LINK32 /verbose /pdb:none

!ENDIF 

# Begin Target

# Name "YAT_Shared - Win32 Release"
# Name "YAT_Shared - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter ""
# Begin Group "src_bitsstream"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\bitsstream\BitsStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bitsstream\Endianness.cpp
# End Source File
# End Group
# Begin Group "src_plugin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\plugin\PlugIn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\plugin\PlugInManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\plugin\PlugInObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\plugin\PlugInUnix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\plugin\PlugInWin32.cpp
# End Source File
# End Group
# Begin Group "src_threading"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\threading\Barrier.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\threading\Message.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\threading\MessageQ.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\threading\SharedObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\threading\Task.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\threading\WinNtThreadingImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\threading\Work.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\threading\Worker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\threading\WorkerErrorManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\threading\WorkerState.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\threading\WorkerTeam.cpp
# End Source File
# End Group
# Begin Group "src_network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\network\Address.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\network\ClientSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\network\Socket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\network\SocketException.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\Exception.cpp
# End Source File
# End Group
# Begin Group "include"

# PROP Default_Filter ""
# Begin Group "yat"

# PROP Default_Filter ""
# Begin Group "bitsstream"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\yat\bitsstream\BitsRecord.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\bitsstream\BitsStream.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\bitsstream\Endianness.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\bitsstream\Endianness.i
# End Source File
# End Group
# Begin Group "plugin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\yat\plugin\IPlugInFactory.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\plugin\IPlugInInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\plugin\IPlugInObject.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\plugin\IPlugInObjectWithAttr.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\plugin\PlugIn.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\plugin\PlugInManager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\plugin\PlugInSymbols.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\plugin\PlugInTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\plugin\PlugInTypes.tpp
# End Source File
# End Group
# Begin Group "threading"

# PROP Default_Filter ""
# Begin Group "impl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\yat\threading\impl\PosixConditionImpl.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\impl\PosixMutexImpl.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\impl\PosixSemaphoreImpl.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\impl\PosixThreadImpl.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\impl\PosixThreadingImpl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\impl\WinNtConditionImpl.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\impl\WinNtMutexImpl.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\impl\WinNtSemaphoreImpl.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\impl\WinNtThreadImpl.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\impl\WinNtThreadingImpl.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\include\yat\threading\Barrier.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\Barrier.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\Condition.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\Implementation.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\Message.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\Message.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\MessageQ.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\MessageQ.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\Mutex.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\Semaphore.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\SharedObject.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\SharedObject.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\Task.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\Task.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\Thread.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\Utilities.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\Work.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\Work.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\Worker.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\Worker.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\WorkerErrorManager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\WorkerErrorManager.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\WorkerTeam.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\threading\WorkerTeam.i
# End Source File
# End Group
# Begin Group "network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\yat\network\Address.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\network\Address.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\network\ClientSocket.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\network\ClientSocket.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\network\Socket.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\network\Socket.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\network\SocketException.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\include\yat\Any.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\Callback.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\CommonHeader.h
# End Source File
# Begin Source File

SOURCE="..\..\include\yat\config-linux.h"
# End Source File
# Begin Source File

SOURCE="..\..\include\yat\config-win32.h"
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\DataBuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\DataBuffer.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\DataBuffer.tpp
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\Exception.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\Exception.i
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\GenericContainer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\Inline.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\LogHelper.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\NonCopyable.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\Portability.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\TargetPlatform.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\Timer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\yat\XString.h
# End Source File
# End Group
# End Group
# End Target
# End Project
