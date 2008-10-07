# Microsoft Developer Studio Project File - Name="ClothSample" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ClothSample - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ClothSample.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ClothSample.mak" CFG="ClothSample - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ClothSample - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ClothSample - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ClothSample - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "ClothSample - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ClothSample - Win32 Release"
# Name "ClothSample - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DataIO\BMPLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\ClothObject.cpp
# End Source File
# Begin Source File

SOURCE=.\ClothSample.cpp
# End Source File
# Begin Source File

SOURCE=.\ClothSample.rc
# End Source File
# Begin Source File

SOURCE=.\Configuration.cpp
# End Source File
# Begin Source File

SOURCE=.\FrameWork\DRGMatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\DRGObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\DRGObjectPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\Objects\DRGShader.cpp
# End Source File
# Begin Source File

SOURCE=.\FrameWork\DRGText.cpp
# End Source File
# Begin Source File

SOURCE=.\FrameWork\DRGTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\FrameWork\DRGWrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\FrameWork\Errors.cpp
# End Source File
# Begin Source File

SOURCE=.\DataIO\FileStream.cpp
# End Source File
# Begin Source File

SOURCE=.\DataIO\ImageLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\Physics_Constraint.cpp
# End Source File
# Begin Source File

SOURCE=.\Physics_Force.cpp
# End Source File
# Begin Source File

SOURCE=.\Physics_GravityForce.cpp
# End Source File
# Begin Source File

SOURCE=.\Physics_LargeVector.cpp
# End Source File
# Begin Source File

SOURCE=.\Physics_Matrix3x3.cpp
# End Source File
# Begin Source File

SOURCE=.\Physics_ParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\Physics_SparseSymmetricMatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\Physics_SpringForce.cpp
# End Source File
# Begin Source File

SOURCE=.\Physics_SymmetricMatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\Physics_Vector3.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DataIO\BMPLoader.h
# End Source File
# Begin Source File

SOURCE=.\ClothObject.h
# End Source File
# Begin Source File

SOURCE=.\ClothSample.h
# End Source File
# Begin Source File

SOURCE=.\Configuration.h
# End Source File
# Begin Source File

SOURCE=.\DataIO\DataStream.h
# End Source File
# Begin Source File

SOURCE=.\FrameWork\DRGMatrix.h
# End Source File
# Begin Source File

SOURCE=.\Objects\DRGObject.h
# End Source File
# Begin Source File

SOURCE=.\Objects\DRGObjectCone.h
# End Source File
# Begin Source File

SOURCE=.\Objects\DRGObjectCube.h
# End Source File
# Begin Source File

SOURCE=.\Objects\DRGObjectPlane.h
# End Source File
# Begin Source File

SOURCE=.\Objects\DRGObjectSkyPlane.h
# End Source File
# Begin Source File

SOURCE=.\Objects\DRGObjectSphere.h
# End Source File
# Begin Source File

SOURCE=.\Objects\DRGObjectXYZAxis.h
# End Source File
# Begin Source File

SOURCE=.\Objects\DRGShader.h
# End Source File
# Begin Source File

SOURCE=.\DRGShell.h
# End Source File
# Begin Source File

SOURCE=.\FrameWork\DRGText.h
# End Source File
# Begin Source File

SOURCE=.\FrameWork\DRGTexture.h
# End Source File
# Begin Source File

SOURCE=.\FrameWork\DRGWrapper.h
# End Source File
# Begin Source File

SOURCE=.\DataIO\FileStream.h
# End Source File
# Begin Source File

SOURCE=.\DataIO\ImageLoader.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\Physics.h
# End Source File
# Begin Source File

SOURCE=.\Physics_Constraint.h
# End Source File
# Begin Source File

SOURCE=.\Physics_Force.h
# End Source File
# Begin Source File

SOURCE=.\Physics_GravityForce.h
# End Source File
# Begin Source File

SOURCE=.\Physics_LargeVector.h
# End Source File
# Begin Source File

SOURCE=.\Physics_Matrix3x3.h
# End Source File
# Begin Source File

SOURCE=.\Physics_ParticleSystem.h
# End Source File
# Begin Source File

SOURCE=.\Physics_SparseSymmetricMatrix.h
# End Source File
# Begin Source File

SOURCE=.\Physics_SpringForce.h
# End Source File
# Begin Source File

SOURCE=.\Physics_SymmetricMatrix.h
# End Source File
# Begin Source File

SOURCE=.\Physics_Vector3.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
