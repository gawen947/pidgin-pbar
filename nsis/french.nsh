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
LangString PIDGIN_NEEDED ${LANG_FRENCH} "PBar requiert que Pidgin soit installé. Vous devez installer Pidgin avant de faire l'installation de PBar."

LangString PBAR_TITLE ${LANG_FRENCH} "Plugin PBar pour Pidgin"

LangString BAD_PIDGIN_VERSION_1 ${LANG_FRENCH} "Version incompatible.$\r$\n$\r$\nCette version du plugin PBar a été créé pour la version ${PIDGIN_VERSION} de Pidgin. Il apparait que vous avez la version"

LangString BAD_PIDGIN_VERSION_2 ${LANG_FRENCH} "installée de Pidgin. $\r$\n$\r$\nRegardez à http://www.hauweele.net/~gawen/pidgin-pbar.html pour plus d'informations."

LangString NO_PIDGIN_VERSION ${LANG_FRENCH} "Impossible de déterminer la version installée de Pidgin."


; Overrides for default text in windows:

LangString WELCOME_TITLE ${LANG_FRENCH} "Installeur de PBar v${PBAR_VERSION}"
LangString WELCOME_TEXT  ${LANG_FRENCH} "Note: Cette version du plugin est conçu pour Pidgin ${PIDGIN_VERSION}, et ne s'installera pas ou ne fonctionnera pas avec les versions de Pidgin ayant un numéro de version majeur différent.\r\n\r\nQuand vous mettez à jour votre version de Pidgin, vous devez désinstaller ou mettre à jour ce plugin aussi.\r\n\r\n"

LangString DIR_SUBTITLE ${LANG_FRENCH} "Merci d'indiquer le répertoire où Pidgin est installé"
LangString DIR_INNERTEXT ${LANG_FRENCH} "Installation dans le répertoire Pidgin:"

LangString FINISH_TITLE ${LANG_FRENCH} "Installation terminée de PBar v${PBAR_VERSION}"
LangString FINISH_TEXT ${LANG_FRENCH} "Vous avez besoin de redémarrer Pidgin pour que le plugin soit chargé, ensuite allez dans les préférences de Pidgin et activez le plugin PBar."

; during install uninstaller
LangString PBAR_PROMPT_WIPEOUT ${LANG_FRENCH} "La librairie pbar.dll du plugin sera supprimé de votre répertoire Pidgin/plugins.  Continuer ?"

; for windows uninstall
LangString PBAR_UNINSTALL_DESC ${LANG_FRENCH} "Plugin PBar (supprimer uniquement)"
LangString un.PBAR_UNINSTALL_ERROR_1 ${LANG_FRENCH} "Le désinstalleur ne peut pas trouver les entrées de registre pour PBar.$\rIl se peut qu'un autre utilisateur a installé le plugin."
LangString un.PBAR_UNINSTALL_ERROR_2 ${LANG_FRENCH} "Vous n'avez pas les permissions nécessaires pour désinstaller le plugin."

