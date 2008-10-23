# Microsoft Developer Studio Project File - Name="demo_rings" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=demo_rings - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "demo_rings.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "demo_rings.mak" CFG="demo_rings - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "demo_rings - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "demo_rings - Win32 Debug" (based on\
 "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "demo_rings - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\demos\rings"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\include\\" /I "..\demos\rings\quatlib\\" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "OGL_GRAPHICS" /YX /FD /TP /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\demos\rings\rings.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib glut32.lib /nologo /subsystem:console /machine:I386 /out:"..\demos\rings\rings.exe"

!ELSEIF  "$(CFG)" == "demo_rings - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\demos\rings"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\include\\" /I "..\demos\rings\quatlib\\" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "OGL_GRAPHICS" /YX /FD /TP /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\demos\rings\rings.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib glut32.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\demos\rings\rings.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "demo_rings - Win32 Release"
# Name "demo_rings - Win32 Debug"
# Begin Source File

SOURCE=..\demos\rings\graphics.C
# End Source File
# Begin Source File

SOURCE=..\demos\rings\graphics.H
# End Source File
# Begin Source File

SOURCE=..\demos\rings\main.C
# End Source File
# Begin Source File

SOURCE=..\demos\rings\quatlib\matrix.c
# End Source File
# Begin Source File

SOURCE=..\demos\rings\quatlib\pdefs.h
# End Source File
# Begin Source File

SOURCE=..\demos\rings\polytope.C
# End Source File
# Begin Source File

SOURCE=..\demos\rings\polytope.H
# End Source File
# Begin Source File

SOURCE=..\demos\rings\quatlib\quat.c
# End Source File
# Begin Source File

SOURCE=..\demos\rings\quatlib\quat.h
# End Source File
# Begin Source File

SOURCE=..\demos\rings\triangle.H
# End Source File
# Begin Source File

SOURCE=..\demos\rings\quatlib\vector.c
# End Source File
# Begin Source File

SOURCE=..\demos\rings\vector.H
# End Source File
# Begin Source File

SOURCE=..\demos\rings\quatlib\xyzquat.c
# End Source File
# End Target
# End Project
