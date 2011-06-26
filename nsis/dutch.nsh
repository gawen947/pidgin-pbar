;;
;;  english.nsh
;;
;;  Default language strings for the Windows PBar NSIS installer.
;;  Windows Code page: 1252
;;  Language Code: 1033
;;

; Startup Pidgin check
LangString PIDGIN_NEEDED ${LANG_ENGLISH} "PBar requires that Pidgin be installed. You must install Pidgin before installing PBar."

LangString PBAR_TITLE ${LANG_ENGLISH} "PBar plugin for Pidgin"

LangString BAD_PIDGIN_VERSION_1 ${LANG_ENGLISH} "Incompatible version.$\r$\n$\r$\nThis version of the PBar plugin was built for Pidgin version ${PIDGIN_VERSION}.  It appears that you have Pidgin version"

LangString BAD_PIDGIN_VERSION_2 ${LANG_ENGLISH} "installed.$\r$\n$\r$\nSee http://www.hauweele.net/~gawen/pidgin-pbar.html for more information."

LangString NO_PIDGIN_VERSION ${LANG_ENGLISH} "Unable to determine installed Pidgin version."


; Overrides for default text in windows:

LangString WELCOME_TITLE ${LANG_ENGLISH} "PBar v${PBAR_VERSION} Installer"
LangString WELCOME_TEXT  ${LANG_ENGLISH} "Note: This version of the plugin is designed for Pidgin ${PIDGIN_VERSION}, and will not install or function with versions of Pidgin having a different major version number.\r\n\r\nWhen you upgrade your version of Pidgin, you must uninstall or upgrade this plugin as well.\r\n\r\n"

LangString DIR_SUBTITLE ${LANG_ENGLISH} "Please locate the directory where Pidgin is installed"
LangString DIR_INNERTEXT ${LANG_ENGLISH} "Install in this Pidgin folder:"

LangString FINISH_TITLE ${LANG_ENGLISH} "PBar v${PBAR_VERSION} Install Complete"
LangString FINISH_TEXT ${LANG_ENGLISH} "You will need to restart Pidgin for the plugin to be loaded, then go the Pidgin preferences and enable the PBar Plugin."

; during install uninstaller
LangString PBAR_PROMPT_WIPEOUT ${LANG_ENGLISH} "The pbar.dll plugin is about to be deleted from your Pidgin/plugins directory.  Continue?"

; for windows uninstall
LangString PBAR_UNINSTALL_DESC ${LANG_ENGLISH} "PBar Plugin (remove only)"
LangString un.PBAR_UNINSTALL_ERROR_1 ${LANG_ENGLISH} "The uninstaller could not find registry entries for PBar.$\rIt is likely that another user installed the plugin."
LangString un.PBAR_UNINSTALL_ERROR_2 ${LANG_ENGLISH} "You do not have the permissions necessary to uninstall the plugin."

