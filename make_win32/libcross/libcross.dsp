# Microsoft Developer Studio Project File - Name="libcross" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libcross - Win32 FullDebug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "libcross.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "libcross.mak" CFG="libcross - Win32 FullDebug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "libcross - Win32 FullDebug" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "libcross - Win32 ReleaseQuit" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE "libcross - Win32 ReleaseLog" (basierend auf  "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "FullDebug"
# PROP BASE Intermediate_Dir "FullDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "FullDebug"
# PROP Intermediate_Dir "FullDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "d:\Python21\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "DEBUG" /D "ESRV_DEBUG" /D "PYTHON_PLUGIN_EXPORTS" /YX"libcross.pch" /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libcross___Win32_ReleaseQuit"
# PROP BASE Intermediate_Dir "libcross___Win32_ReleaseQuit"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseQuit"
# PROP Intermediate_Dir "ReleaseQuit"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "PYTHON_PLUGIN_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\include" /I "d:\Python21\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "PYTHON_PLUGIN_EXPORTS" /YX"libcross.pch" /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libcross___Win32_ReleaseLog"
# PROP BASE Intermediate_Dir "libcross___Win32_ReleaseLog"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseLog"
# PROP Intermediate_Dir "ReleaseLog"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\include" /I "d:\Python21\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "PYTHON_PLUGIN_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\include" /I "d:\Python21\include" /D "_LIB" /D "PYTHON_PLUGIN_EXPORTS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "DEBUG_MOVEATTACK" /YX"libcross.pch" /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libcross - Win32 FullDebug"
# Name "libcross - Win32 ReleaseQuit"
# Name "libcross - Win32 ReleaseLog"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\common\anim.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\arch.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\button.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\exp.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\friend.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\glue.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\holy.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\image.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\info.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\init.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\item.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\links.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\living.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\loader.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\logger.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\los.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\map.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\object.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\player.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\porting.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\common\re-cmp.c"

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\readable.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\recipe.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\shstr.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\time.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\treasure.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\utils.c

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\server\win32.c
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\arch.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\artifact.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\attack.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\book.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\commands.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\config.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\define.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\face.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\funcpoint.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\global.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\god.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\includes.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\libproto.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\living.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\loader.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\logger.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\map.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\material.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\newclient.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\newserver.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\object.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\player.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\plugin.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\plugproto.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\race.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\include\re-cmp.h"

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\recipe.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\shstr.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\skillist.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\skills.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\sockproto.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\sounds.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\spellist.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\spells.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\sproto.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\timers.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\treasure.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\version.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\win32.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\include\xdir.h

!IF  "$(CFG)" == "libcross - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseQuit"

# PROP Intermediate_Dir "ReleaseQuit"

!ELSEIF  "$(CFG)" == "libcross - Win32 ReleaseLog"

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
