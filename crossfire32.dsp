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
# ADD CPP /nologo /MT /W3 /Gi /GX /O2 /I ".\include" /I ".\random_maps" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:console /machine:I386
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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "DEBUG" /D "TIME_ARCH_LOAD" /D "CS_LOGSTATS" /D "ESRV_DEBUG" /FD /GZ /I./include /I./random_maps /c
# ADD BASE RSC /l 0x407 /i "../include" /d "_DEBUG"
# ADD RSC /l 0x407 /i "../include" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
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
# ADD CPP /nologo /MT /W3 /Gi /GX /O2 /I ".\include" /I ".\random_maps" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "DEBUG" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib winmm.lib /nologo /subsystem:console /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Win32_ReleaseNormal\crossfire32.exe crossfire32.exe
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

SOURCE=.\socket\info.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\socket"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/socket"
# PROP Intermediate_Dir "Win32_FullDebug/socket"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\socket"
# PROP Intermediate_Dir "Win32_ReleaseNormal\socket"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\socket\init.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\socket"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/socket"
# PROP Intermediate_Dir "Win32_FullDebug/socket"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\socket"
# PROP Intermediate_Dir "Win32_ReleaseNormal\socket"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\socket\item.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\socket"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/socket"
# PROP Intermediate_Dir "Win32_FullDebug/socket"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\socket"
# PROP Intermediate_Dir "Win32_ReleaseNormal\socket"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\socket\loop.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\socket"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/socket"
# PROP Intermediate_Dir "Win32_FullDebug/socket"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\socket"
# PROP Intermediate_Dir "Win32_ReleaseNormal\socket"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\socket\lowlevel.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\socket"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/socket"
# PROP Intermediate_Dir "Win32_FullDebug/socket"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\socket"
# PROP Intermediate_Dir "Win32_ReleaseNormal\socket"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\socket\metaserver.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\socket"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/socket"
# PROP Intermediate_Dir "Win32_FullDebug/socket"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\socket"
# PROP Intermediate_Dir "Win32_ReleaseNormal\socket"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\socket\request.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\socket"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/socket"
# PROP Intermediate_Dir "Win32_FullDebug/socket"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\socket"
# PROP Intermediate_Dir "Win32_ReleaseNormal\socket"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\socket\sounds.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\socket"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/socket"
# PROP Intermediate_Dir "Win32_FullDebug/socket"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\socket"
# PROP Intermediate_Dir "Win32_ReleaseNormal\socket"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# End Group
# Begin Group "server"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\server\alchemy.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\apply.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\attack.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\ban.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\c_chat.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\c_misc.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\c_move.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\c_new.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\c_object.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\c_party.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\c_range.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\c_wiz.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\commands.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\disease.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\egoitem.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\encounter.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\gods.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\hiscore.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\init.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\login.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\main.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\monster.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\move.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\pets.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\player.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\resurrection.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\rune.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\shop.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\skill_util.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\skills.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\spell_effect.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\spell_util.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\swamp.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\swap.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\time.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\server"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/server"
# PROP Intermediate_Dir "Win32_FullDebug/server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\server"
# PROP Intermediate_Dir "Win32_ReleaseNormal\server"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\server\win32.c
# End Source File
# End Group
# Begin Group "random_maps"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\random_maps\decor.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\door.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\exit.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\floor.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\maze_gen.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\maze_gen.h

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\monster.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\random_map.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\random_map.h

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\reader.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\rogue_layout.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\room_gen.h

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\room_gen_onion.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\room_gen_spiral.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\rproto.h

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\snake.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\special.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\square_spiral.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP Intermediate_Dir "Win32_FullDebug\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\style.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\treasure.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\random_maps\wall.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\random_maps"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/random_maps"
# PROP Intermediate_Dir "Win32_FullDebug\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\random_maps"
# PROP Intermediate_Dir "Win32_ReleaseNormal\random_maps"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\common\anim.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\arch.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\button.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\exp.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\friend.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\glue.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\holy.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\image.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\info.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\init.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\item.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\links.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\living.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\loader.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\logger.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\los.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\ltostr.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\map.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\object.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\player.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\porting.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\common\re-cmp.c"

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\readable.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\recipe.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\shstr.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\sqrt.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\time.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\treasure.c

!IF  "$(CFG)" == "crossfire32 - Win32 Release"

# PROP Intermediate_Dir "Win32_Release\common"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 FullDebug"

# PROP BASE Intermediate_Dir "Debug/common"
# PROP Intermediate_Dir "Win32_FullDebug\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ELSEIF  "$(CFG)" == "crossfire32 - Win32 ReleaseNormal"

# PROP BASE Intermediate_Dir "Release\common"
# PROP Intermediate_Dir "Win32_ReleaseNormal\common"
# ADD BASE CPP /YX"global.h"
# ADD CPP /YX"global.h"

!ENDIF 

# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\arch.h
# End Source File
# Begin Source File

SOURCE=.\include\artifact.h
# End Source File
# Begin Source File

SOURCE=.\include\attack.h
# End Source File
# Begin Source File

SOURCE=.\include\book.h
# End Source File
# Begin Source File

SOURCE=.\include\commands.h
# End Source File
# Begin Source File

SOURCE=.\include\config.h
# End Source File
# Begin Source File

SOURCE=.\include\define.h
# End Source File
# Begin Source File

SOURCE=.\include\face.h
# End Source File
# Begin Source File

SOURCE=.\include\funcpoint.h
# End Source File
# Begin Source File

SOURCE=.\include\global.h
# End Source File
# Begin Source File

SOURCE=.\include\god.h
# End Source File
# Begin Source File

SOURCE=.\include\includes.h
# End Source File
# Begin Source File

SOURCE=.\include\libproto.h
# End Source File
# Begin Source File

SOURCE=.\include\living.h
# End Source File
# Begin Source File

SOURCE=.\include\loader.h
# End Source File
# Begin Source File

SOURCE=.\include\logger.h
# End Source File
# Begin Source File

SOURCE=.\include\map.h
# End Source File
# Begin Source File

SOURCE=.\include\material.h
# End Source File
# Begin Source File

SOURCE=.\include\newclient.h
# End Source File
# Begin Source File

SOURCE=.\include\newserver.h
# End Source File
# Begin Source File

SOURCE=.\include\object.h
# End Source File
# Begin Source File

SOURCE=.\include\player.h
# End Source File
# Begin Source File

SOURCE=.\include\race.h
# End Source File
# Begin Source File

SOURCE=".\include\re-cmp.h"
# End Source File
# Begin Source File

SOURCE=.\include\recipe.h
# End Source File
# Begin Source File

SOURCE=.\include\shstr.h
# End Source File
# Begin Source File

SOURCE=.\include\skillist.h
# End Source File
# Begin Source File

SOURCE=.\include\skills.h
# End Source File
# Begin Source File

SOURCE=.\include\sockproto.h
# End Source File
# Begin Source File

SOURCE=.\include\sounds.h
# End Source File
# Begin Source File

SOURCE=.\include\spellist.h
# End Source File
# Begin Source File

SOURCE=.\include\spells.h
# End Source File
# Begin Source File

SOURCE=.\include\sproto.h
# End Source File
# Begin Source File

SOURCE=.\include\treasure.h
# End Source File
# Begin Source File

SOURCE=.\include\version.h
# End Source File
# Begin Source File

SOURCE=.\include\win32.h
# End Source File
# Begin Source File

SOURCE=.\include\xdir.h
# End Source File
# End Group
# End Target
# End Project
