# Microsoft Developer Studio Project File - Name="crossfire32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=crossfire32 - Win32 FullDebug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "crossfire32.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "crossfire32.mak" CFG="crossfire32 - Win32 FullDebug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "crossfire32 - Win32 Release" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "crossfire32 - Win32 FullDebug" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "crossfire32 - Win32 ReleaseNormal" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Win32_Release"
# PROP Intermediate_Dir "Win32_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /Gi /GX /O2 /I ".\include" /I ".\random_maps" /I ".\\" /I ".\plugin\include" /I "..\include" /I "..\random_maps" /I "..\\" /I "..\plugin\include" /I "d:\Python21\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:console /machine:I386 /libpath:"d:\python21\libs"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Win32_Release\crossfire32.exe crossfire32.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "crossfire32___Win32_FullDebug"
# PROP BASE Intermediate_Dir "crossfire32___Win32_FullDebug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Win32_FullDebug"
# PROP Intermediate_Dir "Win32_FullDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "DEBUG" /D "TIME_ARCH_LOAD" /D "CS_LOGSTATS" /D "ESRV_DEBUG" /FD /GZ /I./include /I./random_maps /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\include" /I ".\random_maps" /I ".\\" /I ".\plugin\include" /I "..\include" /I "..\random_maps" /I "..\\" /I "..\plugin\include" /I "d:\Python21\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "DEBUG" /D "TIME_ARCH_LOAD" /D "ESRV_DEBUG" /FD /GZ /I./include /I./random_maps /c
# ADD BASE RSC /l 0x407 /i "../include" /d "_DEBUG"
# ADD RSC /l 0x407 /i "../include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"d:\python21\libs"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Win32_FullDebug\crossfire32.exe crossfire32.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "crossfire32___Win32_ReleaseNormal"
# PROP BASE Intermediate_Dir "crossfire32___Win32_ReleaseNormal"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Win32_ReleaseNormal"
# PROP Intermediate_Dir "Win32_ReleaseNormal"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /Gi /GX /O2 /I ".\include" /I ".\random_maps" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MT /W3 /Gi /GX /O2 /I "..\include" /I "..\random_maps" /I "..\\" /I "..\plugin\include" /I "d:\Python21\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "DEBUG" /YX"preheader.pch" /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:console /pdb:none /machine:I386 /libpath:"d:\python21\libs"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Win32_ReleaseNormal\crossfire32.exe ..\crossfire32.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "crossfire32 - Win32 Release"
# Name "crossfire32 - Win32 FullDebug"
# Name "crossfire32 - Win32 ReleaseNormal"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "socket"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\socket\info.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\socket"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\socket\init.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\socket"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\socket\item.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\socket"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\socket\loop.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\socket"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\socket\lowlevel.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\socket"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\socket\metaserver.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\socket"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\socket\request.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\socket"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\socket\sounds.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\socket"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# End Group
# Begin Group "server"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\server\alchemy.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\apply.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\attack.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\ban.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\c_chat.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\c_misc.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\c_move.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\c_new.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\c_object.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\c_party.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\c_range.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\c_wiz.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\commands.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\disease.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\egoitem.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\gods.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\hiscore.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\init.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\login.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\main.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\monster.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\move.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\pets.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\player.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\plugins.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\resurrection.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\rune.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\shop.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\skill_util.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\skills.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\spell_effect.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\spell_util.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\swamp.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\swap.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\time.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\server\win32.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# End Group
# Begin Group "random_maps"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\random_maps\decor.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\random_maps\door.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\random_maps\exit.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\random_maps\expand2x.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\random_maps\floor.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\random_maps\maze_gen.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\random_maps\monster.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\random_maps\random_map.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\random_maps\reader.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\random_maps\rogue_layout.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\random_maps\room_gen_onion.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\random_maps\room_gen_spiral.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\random_maps\snake.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\random_maps\special.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\random_maps\square_spiral.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\random_maps\style.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\random_maps\treasure.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\random_maps\wall.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\anim.c
# End Source File
# Begin Source File

SOURCE=..\common\arch.c
# End Source File
# Begin Source File

SOURCE=..\common\button.c
# End Source File
# Begin Source File

SOURCE=..\common\exp.c
# End Source File
# Begin Source File

SOURCE=..\common\friend.c
# End Source File
# Begin Source File

SOURCE=..\common\glue.c
# End Source File
# Begin Source File

SOURCE=..\common\holy.c
# End Source File
# Begin Source File

SOURCE=..\common\image.c
# End Source File
# Begin Source File

SOURCE=..\common\info.c
# End Source File
# Begin Source File

SOURCE=..\common\init.c
# End Source File
# Begin Source File

SOURCE=..\common\item.c
# End Source File
# Begin Source File

SOURCE=..\common\links.c
# End Source File
# Begin Source File

SOURCE=..\common\living.c
# End Source File
# Begin Source File

SOURCE=..\common\loader.c
# End Source File
# Begin Source File

SOURCE=..\common\logger.c
# End Source File
# Begin Source File

SOURCE=..\common\los.c
# End Source File
# Begin Source File

SOURCE=..\common\map.c
# End Source File
# Begin Source File

SOURCE=..\common\object.c
# End Source File
# Begin Source File

SOURCE=..\common\player.c
# End Source File
# Begin Source File

SOURCE=..\common\porting.c
# End Source File
# Begin Source File

SOURCE="..\common\re-cmp.c"
# End Source File
# Begin Source File

SOURCE=..\common\readable.c
# End Source File
# Begin Source File

SOURCE=..\common\recipe.c
# End Source File
# Begin Source File

SOURCE=..\common\shstr.c
# End Source File
# Begin Source File

SOURCE=..\common\time.c
# End Source File
# Begin Source File

SOURCE=..\common\treasure.c
# End Source File
# Begin Source File

SOURCE=..\common\utils.c
# End Source File
# End Group
# Begin Group "plugin"

# PROP Default_Filter ""
# Begin Group "include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\plugin\include\plugin_python.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\plugin\plugin_python.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# ADD CPP /YX"crossfire32.pch"

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\arch.h
# End Source File
# Begin Source File

SOURCE=..\include\artifact.h
# End Source File
# Begin Source File

SOURCE=..\include\attack.h
# End Source File
# Begin Source File

SOURCE=..\include\book.h
# End Source File
# Begin Source File

SOURCE=..\include\commands.h
# End Source File
# Begin Source File

SOURCE=..\include\config.h
# End Source File
# Begin Source File

SOURCE=..\include\define.h
# End Source File
# Begin Source File

SOURCE=..\random_maps\expand2x.h
# End Source File
# Begin Source File

SOURCE=..\include\face.h
# End Source File
# Begin Source File

SOURCE=..\include\funcpoint.h
# End Source File
# Begin Source File

SOURCE=..\include\global.h
# End Source File
# Begin Source File

SOURCE=..\include\god.h
# End Source File
# Begin Source File

SOURCE=..\include\includes.h
# End Source File
# Begin Source File

SOURCE=..\include\libproto.h
# End Source File
# Begin Source File

SOURCE=..\include\living.h
# End Source File
# Begin Source File

SOURCE=..\include\loader.h
# End Source File
# Begin Source File

SOURCE=..\include\logger.h
# End Source File
# Begin Source File

SOURCE=..\include\map.h
# End Source File
# Begin Source File

SOURCE=..\include\material.h
# End Source File
# Begin Source File

SOURCE=..\random_maps\maze_gen.h
# End Source File
# Begin Source File

SOURCE=..\include\newclient.h
# End Source File
# Begin Source File

SOURCE=..\include\newserver.h
# End Source File
# Begin Source File

SOURCE=..\include\object.h
# End Source File
# Begin Source File

SOURCE=..\include\player.h
# End Source File
# Begin Source File

SOURCE=..\include\plugin.h
# End Source File
# Begin Source File

SOURCE=..\include\plugproto.h
# End Source File
# Begin Source File

SOURCE=..\include\race.h
# End Source File
# Begin Source File

SOURCE=..\random_maps\random_map.h
# End Source File
# Begin Source File

SOURCE="..\include\re-cmp.h"
# End Source File
# Begin Source File

SOURCE=..\include\recipe.h
# End Source File
# Begin Source File

SOURCE=..\random_maps\room_gen.h
# End Source File
# Begin Source File

SOURCE=..\random_maps\rproto.h
# End Source File
# Begin Source File

SOURCE=..\include\shstr.h
# End Source File
# Begin Source File

SOURCE=..\include\skillist.h
# End Source File
# Begin Source File

SOURCE=..\include\skills.h
# End Source File
# Begin Source File

SOURCE=..\include\sockproto.h
# End Source File
# Begin Source File

SOURCE=..\include\sounds.h
# End Source File
# Begin Source File

SOURCE=..\include\spellist.h
# End Source File
# Begin Source File

SOURCE=..\include\spells.h
# End Source File
# Begin Source File

SOURCE=..\include\sproto.h
# End Source File
# Begin Source File

SOURCE=..\include\treasure.h
# End Source File
# Begin Source File

SOURCE=..\include\version.h
# End Source File
# Begin Source File

SOURCE=..\include\win32.h
# End Source File
# Begin Source File

SOURCE=..\include\xdir.h
# End Source File
# End Group
# End Target
# End Project
