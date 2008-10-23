# Microsoft Developer Studio Project File - Name="VCollide" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=VCollide - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VCollide.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VCollide.mak" CFG="VCollide - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VCollide - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "VCollide - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "VCollide - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\RAPID\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /TP /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "VCollide - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /I "..\RAPID\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /TP /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "VCollide - Win32 Release"
# Name "VCollide - Win32 Debug"
# Begin Source File

SOURCE=..\src\AABB.H
# End Source File
# Begin Source File

SOURCE=..\src\EndPoint.H
# End Source File
# Begin Source File

SOURCE=..\src\NBody.C
# End Source File
# Begin Source File

SOURCE=..\src\NBody.H
# End Source File
# Begin Source File

SOURCE=..\src\PairData.C
# End Source File
# Begin Source File

SOURCE=..\src\PairData.H
# End Source File
# Begin Source File

SOURCE=..\src\VCollide.C
# End Source File
# Begin Source File

SOURCE=..\src\VCollide.H
# End Source File
# Begin Source File

SOURCE=..\src\VInternal.C
# End Source File
# Begin Source File

SOURCE=..\src\VInternal.H
# End Source File
# End Target
# End Project
