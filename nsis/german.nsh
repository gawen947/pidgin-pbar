;;
;;  german.nsh
;;
;;  Default language strings for the Windows PBar NSIS installer.
;;  Windows Code page: 1252
;;  Language Code: 1033
;;

; Startup Pidgin check
LangString PIDGIN_NEEDED ${LANG_GERMAN} "PBar erfordet dass Pidgin installiert werd, Sie müssen Pidgin installieren vorher Sie PBar installieren."

LangString PBAR_TITLE ${LANG_GERMAN} "PBar plugin für Pidgin"

LangString BAD_PIDGIN_VERSION_1 ${LANG_GERMAN} "Inkompatible Version.$\r$\n$\r$\nDiese Version des PBar Plugin wurde fûr Pidgin-version gebaut ${PIDGIN_VERSION}.  Es scheint, dass Sie Pidgin-Version haben"

LangString BAD_PIDGIN_VERSION_2 ${LANG_GERMAN} "installierend.$\r$\n$\r$\nSee http://www.hauweele.net/~gawen/pidgin-pbar.html für weitere Informationenn."

LangString NO_PIDGIN_VERSION ${LANG_GERMAN} "Kann die installierte Pidgin-Version nicht bestimmen."


; Overrides for default text in windows:

LangString WELCOME_TITLE ${LANG_GERMAN} "PBar v${PBAR_VERSION} Installationprogramm"
LangString WELCOME_TEXT  ${LANG_GERMAN} "Beachten sie; dass diese Version des Plugins für Pidgin konzipiert ${PIDGIN_VERSION}, und lässt sich nicht installieren oder kann nicht funktionieren mit versionen von Pidgin mit einer anderen Hauptversionsnummer.\r\n\r\nWenn Sie Ihre Versionvon Pidgin aktualisieren, müssen Sie deinstallieren oder dieses plugin auch aktualisieren.\r\n\r\n"

LangString DIR_SUBTITLE ${LANG_GERMAN} "Bitte lokalisieren Sie das Verzeichnis, wo in Pidgin installiert ist"
LangString DIR_INNERTEXT ${LANG_GERMAN} "Installieren sie in diesem Pidgin Ordner:"

LangString FINISH_TITLE ${LANG_GERMAN} "PBar v${PBAR_VERSION} Installation vollendet"
LangString FINISH_TEXT ${LANG_GERMAN} "Sie müssen Pidgin wieder einführen für das Plugin neu geladen werden soll, dan auf die Pidgin-Einstellungen gehen und die PBar Plugin aktivieren."

; during install uninstaller
LangString PBAR_PROMPT_WIPEOUT ${LANG_GERMAN} "Die pbar.dll Plugin ist im Begriff, aus dem Pidgin/Plugins Verzeichnis gelöscht werden. Weitergehen ?"

; for windows uninstall
LangString PBAR_UNINSTALL_DESC ${LANG_GERMAN} "PBar Plugin (nur entfernen)"
LangString un.PBAR_UNINSTALL_ERROR_1 ${LANG_GERMAN} "Das Deintallationprogramm konnte der Registrierung einträge für PBar nicht finden.$\rIt es ist wahscheinlich, dass ein anderer Benutzer das Plugin installierte."
LangString un.PBAR_UNINSTALL_ERROR_2 ${LANG_GERMAN} "Sie haben die erfordelichen Berechtigungen nicht, um das Plugin zu deinstallieren."

