!include "MUI.nsh"

;Title Of Your Application
Name "Crossfire Server 1.7.0"

;Do A CRC Check
CRCCheck On

;Output File Name
OutFile "CrossfireServer.exe"

;License Page Introduction
LicenseText "You must agree to this license before installing."

;The Default Installation Directory
InstallDir "$PROGRAMFILES\Crossfire Server"
InstallDirRegKey HKCU "Software\Crossfire Server" ""

!define MUI_ABORTWARNING

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\COPYING"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\Release_notes.txt"
!define MUI_FINISHPAGE_SHOWREADME_TEXT "Show release notes"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_COMPONENTS
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH


!insertmacro MUI_LANGUAGE "English"

Section "Crossfire Server (required)" cf
  SectionIn RO
  ;Install Files
  SetOutPath $INSTDIR
  SetCompress Auto
  SetOverwrite IfNewer
  File "ReleaseLog\crossfire32.exe"
  File "Release_notes.txt"
  File /oname=Changelog.rtf "..\changelog"
  SetOutPath $INSTDIR\share
  File "..\lib\archetypes"
  File "..\lib\artifacts"
  File "..\lib\attackmess"
  File "..\lib\ban_file"
  File "..\lib\bmaps"
  File "..\lib\bmaps.paths"
  File "..\lib\crossfire.0"
  File "..\lib\crossfire.1"
  File "..\lib\def_help"
  File "..\lib\dm_file"
  File "..\lib\exp_table"
  File "..\lib\faces"
  File "..\lib\forbid"
  File "..\lib\formulae"
  File "..\lib\image_info"
  File "..\lib\materials"
  File "..\lib\messages"
  File "..\lib\motd"
  File "..\lib\races"
  File "..\lib\regions"
  File "..\lib\settings"
  File "..\lib\smooth"
  File "..\lib\animations"
  File /oname=treasures "..\lib\treasures.bld"
  SetOutPath $INSTDIR\share\help
  File "..\lib\help\*.*"
  SetOutPath $INSTDIR\share\wizhelp
  File "..\lib\wizhelp\*.*"

  ; Additional directories
  CreateDirectory $INSTDIR\tmp
  CreateDirectory $INSTDIR\var
  CreateDirectory $INSTDIR\var\players
  CreateDirectory $INSTDIR\var\unique-items
  CreateDirectory $INSTDIR\var\datafiles

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Crossfire Server" "DisplayName" "Crossfire Server (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Crossfire Server" "UninstallString" "$INSTDIR\Uninst.exe"
  WriteUninstaller "Uninst.exe"

  ;Ask about Windows service
  MessageBox MB_YESNO|MB_ICONQUESTION "Register Crossfire server as a Windows service?" /SD IDYES IDNO dont_install

        ;Install service
        DetailPrint "Registering service..."
        ExecWait '"$INSTDIR\Crossfire32.exe" -regsrv'
        WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Crossfire Server" "ServiceInstalled" "1"
        
  dont_install:
SectionEnd

Section "Python plugin" py
  SetOutPath $INSTDIR
  File "c:\winnt\system32\python24.dll"
  SetOutPath $INSTDIR\share\plugins
  File "plugin_python\ReleaseLog\plugin_python.dll"
SectionEnd

Section "Menu Shortcuts" menus
  ;Add Shortcuts
  SetOutPath $INSTDIR
  CreateDirectory "$SMPROGRAMS\Crossfire Server"
  CreateShortCut "$SMPROGRAMS\Crossfire Server\Crossfire Server.lnk" "$INSTDIR\crossfire32.exe" "" "$INSTDIR\crossfire32.exe" 0
  CreateShortCut "$SMPROGRAMS\Crossfire Server\Release notes.lnk" "$INSTDIR\Release_notes.txt"
  CreateShortCut "$SMPROGRAMS\Crossfire Server\Changelog.lnk" "$INSTDIR\Changelog.rtf"
  CreateShortCut "$SMPROGRAMS\Crossfire Server\Uninstall.lnk" "$INSTDIR\uninst.exe" "" "$INSTDIR\uninst.exe" 0
SectionEnd

UninstallText "This will uninstall Crossfire Server from your system"

Section "un.Crossfire Server" un_cf
  SectionIn RO
  ;Unregister service if it was installed
  ReadRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Crossfire Server" "ServiceInstalled"
  StrCmp $0 "" 0 +2
        ExecWait '"$INSTDIR\Crossfire32.exe" -unregsrv'

  ;Delete Files
  Delete "$INSTDIR\crossfire32.exe"
  Delete "$INSTDIR\python24.dll"
  Delete "$INSTDIR\Changelog.rtf"
  Delete "$INSTDIR\Share\plugins\python21.dll"
  Delete "$INSTDIR\Release_notes.txt"
  Delete "$INSTDIR\Share\treasures"
  Delete "$INSTDIR\Share\archetypes"
  Delete "$INSTDIR\Share\artifacts"
  Delete "$INSTDIR\Share\attackmess"
  Delete "$INSTDIR\Share\ban_file"
  Delete "$INSTDIR\Share\bmaps"
  Delete "$INSTDIR\Share\bmaps.paths"
  Delete "$INSTDIR\Share\crossfire.0"
  Delete "$INSTDIR\Share\crossfire.1"
  Delete "$INSTDIR\Share\def_help"
  Delete "$INSTDIR\Share\dm_file"
  Delete "$INSTDIR\Share\exp_table"
  Delete "$INSTDIR\Share\faces"
  Delete "$INSTDIR\Share\forbid"
  Delete "$INSTDIR\Share\formulae"
  Delete "$INSTDIR\Share\image_info"
  Delete "$INSTDIR\Share\materials"
  Delete "$INSTDIR\Share\messages"
  Delete "$INSTDIR\Share\motd"
  Delete "$INSTDIR\Share\races"
  Delete "$INSTDIR\Share\regions"
  Delete "$INSTDIR\Share\settings"
  Delete "$INSTDIR\Share\smooth"
  Delete "$INSTDIR\Share\animations"
  
  ;Delete help files
  RmDir /r "$INSTDIR\Share\Help"
  RmDir /r "$INSTDIR\Share\WizHelp"
  
  ;Delete plugins
  RmDir /r "$INSTDIR\Share\Plugins"
  
  ;Remove 'temp' directory
  rmdir /r "$INSTDIR\tmp"
  
  ;Remove some data files
  Delete "$INSTDIR\Var\clockdata"
  Delete "$INSTDIR\Var\crossfire.log"
  Delete "$INSTDIR\Var\crossfiremail"

  ;Delete Start Menu Shortcuts
  RmDir /r "$SMPROGRAMS\Crossfire Server"

  ;Delete Uninstaller And Unistall Registry Entries
  Delete "$INSTDIR\Uninst.exe"
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Crossfire Server"
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Crossfire Server"
SectionEnd

Section "un.Player files and unique maps data" un_pl
  ;Remove player data section
  MessageBox MB_YESNO|MB_ICONEXCLAMATION "Warning, this will remove all player files, and player data!$\rAre you sure?" IDNO skip
  RmDir /r "$INSTDIR\var\players"
  RmDir /r "$INSTDIR\var\unique-items"
  RmDir /r "$INSTDIR\var\datafiles"
  skip:
SectionEnd


Section -un.final_clean

  ;Let's check for map uninstaller
  IfFileExists "$INSTDIR\UninstMaps.exe" maps no_maps

maps:
    MessageBox MB_YESNO|MB_ICONQUESTION "Maps found. Remove them?" IDNO no_maps
    
    Banner::Show /NOUNLOAD /set 76 "Please wait" "Uninstalling maps..."

    ;Remove maps, let's call the uninstaller in silent mode, and no copying itself somewhere else
    ;(else ExecWait can't wait!)
    ExecWait '"$INSTDIR\UninstMaps.exe" /S _?=$INSTDIR'
    ;Need to remove installer, as it couldn't remove itself
    Delete "$INSTDIR\UninstMaps.exe"

    Banner::Destroy

no_maps:

  ;Delete Share directory, if empty
  Rmdir "$INSTDIR\share"

  ;Remove 'var' directory if possible (no force, since user can leave player data)
  rmdir "$INSTDIR\var"

  ;Clean main directory if possible
  RmDir "$INSTDIR"

SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${cf} "Crossfire Server (required)."
  !insertmacro MUI_DESCRIPTION_TEXT ${py} "Python plugin support. Enables post office and a few goodies. Python required."
  !insertmacro MUI_DESCRIPTION_TEXT ${menus} "Insert icons in Start Menu."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

!insertmacro MUI_UNFUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${un_cf} "Remove Crossfire Server."
  !insertmacro MUI_DESCRIPTION_TEXT ${un_pl} "Remove ALL player data, as well as unique maps information."
!insertmacro MUI_UNFUNCTION_DESCRIPTION_END

