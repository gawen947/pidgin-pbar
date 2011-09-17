;;
;;  english.nsh
;;
;;  Default language strings for the Windows PBar NSIS installer.
;;  Windows Code page: 1252
;;  Language Code: 1033
;;

; Startup Pidgin check
LangString PIDGIN_NEEDED ${LANG_DUTCH} "PBar vereist dat Pidgin word geinstalleerd. U moet Pidgin installeeren voordat U PBar installeerd."

LangString PBAR_TITLE ${LANG_DUTCH} "PBar plugin voor Pidgin"

LangString BAD_PIDGIN_VERSION_1 ${LANG_DUTCH} "Incompatipele versie.$\r$\n$\r$\ndeze versie van de Pbar plugin werd gebouwd voor de pidgin-versie ${PIDGIN_VERSION}.  het lijkt erop dat je Pidgin-versie hebt"

LangString BAD_PIDGIN_VERSION_2 ${LANG_DUTCH} "geïnstalleerd.$\r$\n$\r$\nSee http://www.hauweele.net/~gawen/pidgin-pbar.html voor meer informatie."

LangString NO_PIDGIN_VERSION ${LANG_DUTCH} "onbekwaam om de geïnstalleerde versie te bepalen."


; Overrides for default text in windows:

LangString WELCOME_TITLE ${LANG_DUTCH} "PBar v${PBAR_VERSION} Installieprogramma"
LangString WELCOME_TEXT  ${LANG_DUTCH} "Opmerking : Deze versie van de plugin is ontworpen voor Pidgin ${PIDGIN_VERSION}, en zal niet installeren of funktioneren met een versie van Pidgin met een verschillend hoofdversienummer.\r\n\r\nWanneer U uw versie van Pidgin upgradeerd, moet u de plugin verwijderen of ook upgraden.\r\n\r\n"

LangString DIR_SUBTITLE ${LANG_DUTCH} "Gelieve de directory waar pidgin geïnstalleerd is te zoeken"
LangString DIR_INNERTEXT ${LANG_DUTCH} "installeren in deze Pidgin folder:"

LangString FINISH_TITLE ${LANG_DUTCH} "PBar v${PBAR_VERSION} de installatie is voltooid"
LangString FINISH_TEXT ${LANG_DUTCH} "U moet Pidgin restarten om de plugin te laden, ga dan naar de Pidgin voorkeuren om de Pbar plugin te aktiveren."

; during install uninstaller
LangString PBAR_PROMPT_WIPEOUT ${LANG_DUTCH} "De pbar.dll plugin zal van uw Pidgin/plugins directory worden verwijderd. Doorgaan ?"

; for windows uninstall
LangString PBAR_UNINSTALL_DESC ${LANG_DUTCH} "PBar Plugin (Alleen verwijderen)"
LangString un.PBAR_UNINSTALL_ERROR_1 ${LANG_DUTCH} "De deïnstallatieprogramma kon de registervermeldingen voor PBar niet vinden.$\rIt het is mogelijk dat een andere gebruiker de plugin geïnstalleerd heeft."
LangString un.PBAR_UNINSTALL_ERROR_2 ${LANG_DUTCH} "U heeft de nodige toestemmingen niet om de plugin te verwijderen."

