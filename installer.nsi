; NSIS Script For PBar Plugin
; Author David Hauweele
; Based on the Guifications Plugin installer by Daniel A. Atallah
; Uses NSIS v2.0

;--------------------------------
;Include Modern UI
  !include "MUI.nsh"

!include "FileFunc.nsh"
!insertmacro GetParameters
!insertmacro GetOptions


;--------------------------------
;General
  Name "Pidgin PBar ${PBAR_VERSION}"

  ;Do A CRC Check
  CRCCheck On

  ;Output File Name
  OutFile "Pidgin-PBar_${PBAR_VERSION}.exe"

  ;The Default Installation Directory
  InstallDir "$PROGRAMFILES\pidgin"
  InstallDirRegKey HKLM SOFTWARE\pidgin ""

  ShowInstDetails show
  ShowUnInstDetails show
  SetCompressor /SOLID lzma

;Reserve files used in .onInit for faster start-up
!insertmacro MUI_RESERVEFILE_LANGDLL

  !define PBAR_UNINST_EXE     "Pidgin-PBar-uninst.exe"
  !define PBAR_DLL            "pbar.dll"
  !define PBAR_UNINSTALL_LNK  "PBar Uninstall.lnk"

;--------------------------------
; Registry keys:
  !define PBAR_REG_KEY        "SOFTWARE\pidgin-pbar"
  !define PBAR_UNINSTALL_KEY  "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\pidgin-pbar"

;-------------------------------
; Pidgin Plugin installer helper stuff
  !addincludedir "${PIDGIN_TREE_TOP}\pidgin\win32\nsis"
  !include "pidgin-plugin.nsh"

;--------------------------------
; Modern UI Configuration
  !define MUI_ICON .\nsis\install.ico
  !define MUI_UNICON .\nsis\install.ico
  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP "nsis\header.bmp"
  !define MUI_CUSTOMFUNCTION_GUIINIT pbar_checkPidginVersion
  !define MUI_ABORTWARNING

  !define MUI_LANGDLL_REGISTRY_ROOT "HKCU"
  !define MUI_LANGDLL_REGISTRY_KEY ${PBAR_REG_KEY}
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

;--------------------------------
; Pages
  ;Welcome Page
  !define MUI_WELCOMEPAGE_TITLE $(WELCOME_TITLE)
  !define MUI_WELCOMEPAGE_TEXT $(WELCOME_TEXT)
  !insertmacro MUI_PAGE_WELCOME

  ;License Page
  !define MUI_LICENSEPAGE_RADIOBUTTONS
  !insertmacro MUI_PAGE_LICENSE  ".\COPYING"

  ;Directory Page
  !define MUI_DIRECTORYPAGE_TEXT_TOP $(DIR_SUBTITLE)
  !define MUI_DIRECTORYPAGE_TEXT_DESTINATION $(DIR_INNERTEXT)
  !insertmacro MUI_PAGE_DIRECTORY

  ;Installation Page
  !insertmacro MUI_PAGE_INSTFILES

  ;Finish Page
  !define MUI_FINISHPAGE_TITLE $(FINISH_TITLE)
  !define MUI_FINISHPAGE_TEXT $(FINISH_TEXT)
  !insertmacro MUI_PAGE_FINISH


;--------------------------------
; Languages
  !insertmacro MUI_LANGUAGE "English"
  !insertmacro MUI_LANGUAGE "French"
  !insertmacro MUI_LANGUAGE "Dutch"
  !insertmacro MUI_LANGUAGE "German"

  ;Translations
  !include "nsis\translations\english.nsh"
  !include "nsis\translations\french.nsh"
  !include "nsis\translations\dutch.nsh"
  !include "nsis\translations\german.nsh"

; Uninstall the previous version if it exists
Section -SecUninstallOldPlugin
  ; Check install rights..
  Call CheckUserInstallRights
  Pop $R0

  StrCmp $R0 "HKLM" rights_hklm
  StrCmp $R0 "HKCU" rights_hkcu done

  rights_hkcu:
    ReadRegStr $R1 HKCU "${PBAR_REG_KEY}" ""
    ReadRegStr $R2 HKCU "${PBAR_REG_KEY}" "Version"
    ReadRegStr $R3 HKCU "${PBAR_UNINSTALL_KEY}" "UninstallString"
    Goto try_uninstall

  rights_hklm:
    ReadRegStr $R1 HKLM "${PBAR_REG_KEY}" ""
    ReadRegStr $R2 HKLM "${PBAR_REG_KEY}" "Version"
    ReadRegStr $R3 HKLM "${PBAR_UNINSTALL_KEY}" "UninstallString"

  ; If previous version exists .. remove
  try_uninstall:
    StrCmp $R1 "" done
      StrCmp $R2 "" uninstall_problem
        IfFileExists $R3 0 uninstall_problem
          ; Have uninstall string.. go ahead and uninstall.
          SetOverwrite on
          ; Need to copy uninstaller outside of the install dir
          ClearErrors
          CopyFiles /SILENT $R3 "$TEMP\${PBAR_UNINST_EXE}"
          SetOverwrite off
          IfErrors uninstall_problem
            ; Ready to uninstall..
            ClearErrors
            ExecWait '"$TEMP\${PBAR_UNINST_EXE}" /S _?=$R1'
            IfErrors exec_error
              Delete "$TEMP\${PBAR_UNINST_EXE}"
              Goto done

            exec_error:
              Delete "$TEMP\${PBAR_UNINST_EXE}"
              Goto uninstall_problem

        uninstall_problem:
          ; Just delete the plugin and uninstaller, and remove Registry key
          MessageBox MB_YESNO $(PBAR_PROMPT_WIPEOUT) IDYES do_wipeout IDNO cancel_install
        cancel_install:
          Quit

        do_wipeout:
          StrCmp $R0 "HKLM" del_lm_reg del_cu_reg
          del_cu_reg:
            DeleteRegKey HKCU ${PBAR_REG_KEY}
            Goto uninstall_prob_cont
          del_lm_reg:
            DeleteRegKey HKLM ${PBAR_REG_KEY}

        uninstall_prob_cont:
          ; plugin DLL
          Delete "$R1\plugins\${PBAR_DLL}"
          Delete "$R3"

  done:
SectionEnd

!macro INSTALL_GMO LANG
    SetOutPath "$INSTDIR\locale\${LANG}\LC_MESSAGES"
    File /oname=pbar.mo po\${LANG}.gmo
!macroend

Section "Install"
  Call CheckUserInstallRights
  Pop $R0

  StrCmp $R0 "NONE" instrights_none
  StrCmp $R0 "HKLM" instrights_hklm instrights_hkcu

  instrights_hklm:
    ; Write the version registry keys:
    WriteRegStr HKLM ${PBAR_REG_KEY} "" "$INSTDIR"
    WriteRegStr HKLM ${PBAR_REG_KEY} "Version" "${PBAR_VERSION}"

    ; Write the uninstall keys for Windows
    WriteRegStr HKLM ${PBAR_UNINSTALL_KEY} "DisplayName" "$(PBAR_UNINSTALL_DESC)"
    WriteRegStr HKLM ${PBAR_UNINSTALL_KEY} "UninstallString" "$INSTDIR\${PBAR_UNINST_EXE}"
    SetShellVarContext "all"
    Goto install_files

  instrights_hkcu:
    ; Write the version registry keys:
    WriteRegStr HKCU ${PBAR_REG_KEY} "" "$INSTDIR"
    WriteRegStr HKCU ${PBAR_REG_KEY} "Version" "${PBAR_VERSION}"

    ; Write the uninstall keys for Windows
    WriteRegStr HKCU ${PBAR_UNINSTALL_KEY} "DisplayName" "$(PBAR_UNINSTALL_DESC)"
    WriteRegStr HKCU ${PBAR_UNINSTALL_KEY} "UninstallString" "$INSTDIR\${PBAR_UNINST_EXE}"
    Goto install_files

  instrights_none:
    ; No registry keys for us...

  install_files:
    SetOutPath "$INSTDIR\plugins"
    SetCompress Auto
    SetOverwrite on
    File "src\${PBAR_DLL}"

    ; translations - if there is a way to automate this, i can't find it
    !insertmacro INSTALL_GMO "de"
    !insertmacro INSTALL_GMO "fr"
    !insertmacro INSTALL_GMO "nl"

    StrCmp $R0 "NONE" done
    CreateShortCut "$SMPROGRAMS\Pidgin\${PBAR_UNINSTALL_LNK}" "$INSTDIR\${PBAR_UNINST_EXE}"
    WriteUninstaller "$INSTDIR\${PBAR_UNINST_EXE}"
    SetOverWrite off

  done:
SectionEnd

Section Uninstall
  Call un.CheckUserInstallRights
  Pop $R0
  StrCmp $R0 "NONE" no_rights
  StrCmp $R0 "HKCU" try_hkcu try_hklm

  try_hkcu:
    ReadRegStr $R0 HKCU "${PBAR_REG_KEY}" ""
    StrCmp $R0 $INSTDIR 0 cant_uninstall
      ; HKCU install path matches our INSTDIR.. so uninstall
      DeleteRegKey HKCU "${PBAR_REG_KEY}"
      DeleteRegKey HKCU "${PBAR_UNINSTALL_KEY}"
      Goto cont_uninstall

  try_hklm:
    ReadRegStr $R0 HKLM "${PBAR_REG_KEY}" ""
    StrCmp $R0 $INSTDIR 0 try_hkcu
      ; HKLM install path matches our INSTDIR.. so uninstall
      DeleteRegKey HKLM "${PBAR_REG_KEY}"
      DeleteRegKey HKLM "${PBAR_UNINSTALL_KEY}"
      ; Sets start menu and desktop scope to all users..
      SetShellVarContext "all"

  cont_uninstall:
    ; plugin
    Delete "$INSTDIR\plugins\${PBAR_DLL}"

    ; translations
    ; loop through locale dirs and try to delete any PBar translations
    ClearErrors
    FindFirst $R1 $R2 "$INSTDIR\locale\*"
    IfErrors doneFindingTranslations

    processCurrentTranslationDir:
      ;Ignore "." and ".."
      StrCmp $R2 "." readNextTranslationDir
      StrCmp $R2 ".." readNextTranslationDir
      IfFileExists "$INSTDIR\locale\$R2\LC_MESSAGES\pbar.mo" +1 readNextTranslationDir
      Delete "$INSTDIR\locale\$R2\LC_MESSAGES\pbar.mo"
      RMDir  "$INSTDIR\locale\$R2\LC_MESSAGES"
      RMDir  "$INSTDIR\locale\$R2"
      ClearErrors
    readNextTranslationDir:
      FindNext $R1 $R2
    IfErrors doneFindingTranslations processCurrentTranslationDir

    doneFindingTranslations:
    FindClose $R1
    RMDir  "$INSTDIR\locale"

    ; uninstaller
    Delete "$INSTDIR\${PBAR_UNINST_EXE}"
    ; uninstaller shortcut
    Delete "$SMPROGRAMS\Pidgin\${PBAR_UNINSTALL_LNK}"

    ; try to delete the Pidgin directories, in case it has already uninstalled
    RMDir "$INSTDIR\plugins"
    RMDir "$INSTDIR"
    RMDir "$SMPROGRAMS\Pidgin"

    Goto done

  cant_uninstall:
    MessageBox MB_OK $(un.PBAR_UNINSTALL_ERROR_1) IDOK
    Quit

  no_rights:
    MessageBox MB_OK $(un.PBAR_UNINSTALL_ERROR_2) IDOK
    Quit

  done:
SectionEnd

Function .onInit
  ${GetParameters} $R0
  ClearErrors
  ${GetOptions} $R0 "/L=" $R0
  IfErrors +3
  StrCpy $LANGUAGE $R0
  Goto skip_lang

  ; Select Language
    ; Display Language selection dialog
    !insertmacro MUI_LANGDLL_DISPLAY
    skip_lang:

FunctionEnd

Function un.onInit
  ; Get stored language preference
  !insertmacro MUI_UNGETLANGUAGE
FunctionEnd


; Check that the selected installation dir contains pidgin.exe
Function .onVerifyInstDir
  IfFileExists $INSTDIR\pidgin.exe +2
    Abort
FunctionEnd

; Check that the currently installed pidgin version is compatible with the version of PBar we are installing
Function pbar_checkPidginVersion
  Push $R0

  Push ${PIDGIN_VERSION}
  Call CheckPidginVersion
  Pop $R0

  StrCmp $R0 ${PIDGIN_VERSION_OK} pbar_checkPidginVersion_OK
  StrCmp $R0 ${PIDGIN_VERSION_INCOMPATIBLE} +1 +6
    Call GetPidginVersion
    IfErrors +3
    Pop $R0
    MessageBox MB_OK|MB_ICONSTOP "$(BAD_PIDGIN_VERSION_1) $R0 $(BAD_PIDGIN_VERSION_2)"
    goto +2
    MessageBox MB_OK|MB_ICONSTOP "$(NO_PIDGIN_VERSION)"
    Quit

  pbar_checkPidginVersion_OK:
  Pop $R0
FunctionEnd

Function CheckUserInstallRights
  ClearErrors
  UserInfo::GetName
  IfErrors Win9x
  Pop $0
  UserInfo::GetAccountType
  Pop $1

  StrCmp $1 "Admin" 0 +3
    StrCpy $1 "HKLM"
    Goto done
  StrCmp $1 "Power" 0 +3
    StrCpy $1 "HKLM"
    Goto done
  StrCmp $1 "User" 0 +3
    StrCpy $1 "HKCU"
    Goto done
  StrCmp $1 "Guest" 0 +3
    StrCpy $1 "NONE"
    Goto done

  ; Unknown error
  StrCpy $1 "NONE"
  Goto done

  Win9x:
    StrCpy $1 "HKLM"

  done:
  Push $1
FunctionEnd

; This is necessary because the uninstaller doesn't have access to installer functions
; (it is identical to CheckUserInstallRights)
Function un.CheckUserInstallRights
  ClearErrors
  UserInfo::GetName
  IfErrors Win9x
  Pop $0
  UserInfo::GetAccountType
  Pop $1

  StrCmp $1 "Admin" 0 +3
    StrCpy $1 "HKLM"
    Goto done
  StrCmp $1 "Power" 0 +3
    StrCpy $1 "HKLM"
    Goto done
  StrCmp $1 "User" 0 +3
    StrCpy $1 "HKCU"
    Goto done
  StrCmp $1 "Guest" 0 +3
    StrCpy $1 "NONE"
    Goto done

  ; Unknown error
  StrCpy $1 "NONE"
  Goto done

  Win9x:
    StrCpy $1 "HKLM"

  done:
  Push $1
FunctionEnd
