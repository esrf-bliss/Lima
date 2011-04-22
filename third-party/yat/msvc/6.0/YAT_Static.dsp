# Microsoft Developer Studio Project File - Name="YAT_Static" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=YAT_Static - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "YAT_Static.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "YAT_Static.mak" CFG="YAT_Static - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "YAT_Static - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "YAT_Static - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "YAT_Static - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "YAT_Static___Win32_Release"
# PROP BASE Intermediate_Dir "YAT_Static___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "YAT\static\release"
# PROP Intermediate_Dir "YAT\static\release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /Z7 /O2 /I "..\..\include" /I "..\..\src" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_EVAL_PERF_" /FR /YX /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\static\msvc-6.0\libYAT.lib"

!ELSEIF  "$(CFG)" == "YAT_Static - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "YAT_Static___Win32_Debug"
# PROP BASE Intermediate_Dir "YAT_Static___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "YAT\static\debug"
# PROP Intermediate_Dir "YAT\static\debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GR /GX /Z7 /Od /I "..\..\include" /I "..\..\src" /D "_DEBUG" /D "YAT_ENABLE_LOG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_EVAL_PERF_" /FR /YX /FD /GZ /GZ /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\static\msvc-6.0\libYATd.lib"

!ENDIF 

# Begin Target

# Name "YAT_Static - Win32 Release"
# Name "YAT_Static - Win32 Debug"
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
# End Group
# Begin Group "threading"

# PROP Default_Filter ""
# Begin Group "impl"

# PROP Default_Filter ""
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
