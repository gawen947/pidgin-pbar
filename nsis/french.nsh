;;
;;  french.nsh
;;
;;  French language strings for the Windows PBar NSIS installer.
;;  Windows Code page: 1252
;;  Language Code: 1036
;;  Derived from the French translation for the Windows guificiations NSIS installer by Yannick LE NY
;;  
;;

; Startup Pidgin check
LangString PIDGIN_NEEDED ${LANG_FRENCH} "PBar requiert que Pidgin soit install�. Vous devez installer Pidgin avant de faire l'installation de PBar."

LangString PBAR_TITLE ${LANG_FRENCH} "Plugin PBar pour Pidgin"

LangString BAD_PIDGIN_VERSION_1 ${LANG_FRENCH} "Version incompatible.$\r$\n$\r$\nCette version du plugin PBar a �t� cr�� pour la version ${PIDGIN_VERSION} de Pidgin. Il apparait que vous avez la version"

LangString BAD_PIDGIN_VERSION_2 ${LANG_FRENCH} "install�e de Pidgin. $\r$\n$\r$\nRegardez � http://www.hauweele.net/~gawen/pidgin-pbar.html pour plus d'informations."

LangString NO_PIDGIN_VERSION ${LANG_FRENCH} "Impossible de d�terminer la version install�e de Pidgin."


; Overrides for default text in windows:

LangString WELCOME_TITLE ${LANG_FRENCH} "Installeur de PBar v${PBAR_VERSION}"
LangString WELCOME_TEXT  ${LANG_FRENCH} "Note: Cette version du plugin est con�u pour Pidgin ${PIDGIN_VERSION}, et ne s'installera pas ou ne fonctionnera pas avec les versions de Pidgin ayant un num�ro de version majeur diff�rent.\r\n\r\nQuand vous mettez � jour votre version de Pidgin, vous devez d�sinstaller ou mettre � jour ce plugin aussi.\r\n\r\n"

LangString DIR_SUBTITLE ${LANG_FRENCH} "Merci d'indiquer le r�pertoire o� Pidgin est install�"
LangString DIR_INNERTEXT ${LANG_FRENCH} "Installation dans le r�pertoire Pidgin:"

LangString FINISH_TITLE ${LANG_FRENCH} "Installation termin�e de PBar v${PBAR_VERSION}"
LangString FINISH_TEXT ${LANG_FRENCH} "Vous avez besoin de red�marrer Pidgin pour que le plugin soit charg�, ensuite allez dans les pr�f�rences de Pidgin et activez le plugin PBar."

; during install uninstaller
LangString PBAR_PROMPT_WIPEOUT ${LANG_FRENCH} "La librairie pbar.dll du plugin sera supprim� de votre r�pertoire Pidgin/plugins.  Continuer ?"

; for windows uninstall
LangString PBAR_UNINSTALL_DESC ${LANG_FRENCH} "Plugin PBar (supprimer uniquement)"
LangString un.PBAR_UNINSTALL_ERROR_1 ${LANG_FRENCH} "Le d�sinstalleur ne peut pas trouver les entr�es de registre pour PBar.$\rIl se peut qu'un autre utilisateur a install� le plugin."
LangString un.PBAR_UNINSTALL_ERROR_2 ${LANG_FRENCH} "Vous n'avez pas les permissions n�cessaires pour d�sinstaller le plugin."

