/*
	STRCNITA.h

	Copyright (C) 2006 Paul C. Pratt, Fabio Concas, Gianluca Abbiati

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

/*
	STRing CoNstants for ITAlian

	These strings were originally translated by Fabio Concas, who
	hasn't been heard from lately, and modifications were
	needed for version 2.8.1. Gianluca Abbiati agreed to look
	at it, and made numerous changes for version 2.8.2.
*/

#define kStrAboutTitle "Informazioni"
#define kStrAboutMessage "Per visualizzare le informazioni su questo programma usa il comando ;]A;} dalla Modalit;`a Controllo di ^p. Per informazioni sulla Modalit;`a Controllo, vedi ;[Ulteriori Comandi;ll;{ nel men;`u ;[Speciale;{."

#define kStrMoreCommandsTitle "Ulteriori comandi sono disponibili nella Modalit;`a Controllo di ^p."
#define kStrMoreCommandsMessage "Per entrare nella Modalit;`a Controllo, premi e tieni premuto il tasto ;]^c;}. La Modalit;`a Controllo rester;`a attiva fino a che il tasto ;]^c;} non verr;`a rilasciato. Digita ;]H;} nella Modalit;`a Controllo per avere la lista dei comandi disponibili."

#define kStrTooManyImagesTitle "Troppe Immagini Disco."
#define kStrTooManyImagesMessage "Non posso montare altre Immagini Disco. Prova ad espellerne una."

#define kStrImageInUseTitle "Immagine Disco in uso."
#define kStrImageInUseMessage "Non posso montare l;}Immagine Disco perch;ee ;`e utilizzata da un;}altra applicazione."

#define kStrOutOfMemTitle "Memoria insufficiente."
#define kStrOutOfMemMessage "Non c;};`e abbastanza memoria per avviare ^p."

#define kStrNoROMTitle "Impossibile trovare l;}immagine della ROM."
#define kStrNoROMMessage "Non trovo il file immagine della ROM ;[^r;{. Per maggiori informazioni, vedi: ;[^w;{"

#define kStrCorruptedROMTitle "Checksum della ROM fallita."
#define kStrCorruptedROMMessage "L;}immagine della ROM ;[^r;{ potrebbe essere danneggiata."

#define kStrUnsupportedROMTitle "ROM non supportata."
#define kStrUnsupportedROMMessage "Il file immagine della ROM ;[^r;{ ;`e stato caricato, ma ^p non supporta ancora questa ROM."

#define kStrQuitWarningTitle "Dovresti spegnere la macchina emulata prima di uscire."
#define kStrQuitWarningMessage "Per forzare l;}uscita da ^p (col rischio di danneggiare le immagini disco montate), usa il comando ;]Q;} dalla Modalit;`a Controllo di ^p. Per informazioni sulla Modalit;`a Controllo, vedi ;[Ulteriori Comandi;ll;{ nel men;`u ;[Speciale;{."

#define kStrReportAbnormalTitle "Situazione Anomala"
#define kStrReportAbnormalMessage "La macchina emulata sta tentando un;}operazione inaspettata nell;}uso normale."

#define kStrBadArgTitle "Argomento sconosciuto."
#define kStrBadArgMessage "Non comprendo uno dei parametri della riga di comando, lo ignoro."

#define kStrOpenFailTitle "Apertura fallita."
#define kStrOpenFailMessage "Non posso aprire l;}immagine disco."

#define kStrNoReadROMTitle "Non posso leggere l;}immagine della ROM."
#define kStrNoReadROMMessage "Ho trovato l;}immagine della ROM ;[^r;{, ma non posso leggerla."

#define kStrShortROMTitle "L;}immagine della ROM ;`e troppo piccola."
#define kStrShortROMMessage "Il file immagine della ROM ;[^r;{ ;`e pi;`u piccolo di quanto dovrebbe essere."

/* state of a boolean option */
#define kStrOn "attivo"
#define kStrOff "inattivo"

/* state of a key */
#define kStrPressed "premuto"
#define kStrReleased "rilasciato"

/* state of Stopped */
#define kStrStoppedOn kStrOn
#define kStrStoppedOff kStrOff

/* About Screen */
#define kStrProgramInfo "^v"
#define kStrSponsorIs "^v. This variation is made for:"
#define kStrWorkOfMany "Copyright ^y. ^p contiene il lavoro di molte persone. Questa versione ;`e mantenuta da:"
#define kStrForMoreInfo "Per maggiori informazioni, vedi:"
#define kStrLicense "^p ;`e distribuito secondo i termini della Licenza Pubblica GNU, versione 2."
#define kStrDisclaimer " ^p ;`e distribuito con la speranza che sia utile, ma SENZA ALCUNA GARANZIA;ls compresa la garanzia implicita di COMMERCIABILIT;`A o IDONEIT;`A PER UN QUALSIASI SCOPO."

/* Help Screen */
#define kStrHowToLeaveControl "Per uscire dalla Modalit;`a Controllo, rilascia il tasto ;]^c;}."
#define kStrHowToPickACommand "Altrimenti premi una lettera. I comandi disponibili sono:"
#define kStrCmdAbout "Informazioni (informazioni sulla versione)"
#define kStrCmdOpenDiskImage "Apri immagine disco;ll"
#define kStrCmdQuit "Esci"
#define kStrCmdSpeedControl "Controllo velocit;`a;ll (^s)"
#define kStrCmdMagnifyToggle "Ingrandimento (^g)"
#define kStrCmdFullScrnToggle "Schermo intero (^f)"
#define kStrCmdCtrlKeyToggle "Tasto ;]^m;} emulato (^k)"
#define kStrCmdReset "Reset"
#define kStrCmdInterrupt "Interrupt"
#define kStrCmdHelp "Aiuto (mostra questo messaggio)"

/* Speed Control Screen */
#define kStrCurrentSpeed "Velocit;`a attuale: ^s"
#define kStrSpeedAllOut "Massima velocit;`a"
#define kStrSpeedStopped "Stop (^h)"
#define kStrSpeedBackToggle "Funzionamento in background (^b)"
#define kStrSpeedAutoSlowToggle "AutoSlow (^l)"
#define kStrSpeedExit "Esci da Controllo Velocit;`a"

#define kStrNewSpeed "Velocit;`a: ^s"
#define kStrSpeedValueAllOut "Massima"

#define kStrNewStopped "Lo stop ;`e ^h"
#define kStrNewRunInBack "Il funzionamento in background ;`e ^b"
#define kStrNewAutoSlow "AutoSlow ;`e ^l."

#define kStrNewMagnify "L;laingrandimento ;`e ^g"

#define kStrNewFullScreen "Lo schermo intero ;`e ^f"

#define kStrNewCntrlKey "Il tasto ;]^m;} emulato ;`e ^k"

#define kStrCmdCancel "Annulla"

#define kStrConfirmReset "Sei sicuro di voler resettare la macchina emulata? Le modifiche non salvate andranno perse e le immagini disco montate potrebbero venire danneggiate. Premi una lettera:"
#define kStrResetDo kStrCmdReset
#define kStrResetNo kStrCmdCancel

#define kStrHaveReset "La macchina emulata ;`e stata resettata"

#define kStrCancelledReset "Reset annullato"

#define kStrConfirmInterrupt "Sei sicuro di voler eseguire l;laInterrupt della macchina emulata? Questo avvier;`a il debugger installato nel tuo sistema. Premi una lettera:"
#define kStrInterruptDo kStrCmdInterrupt
#define kStrInterruptNo kStrCmdCancel

#define kStrHaveInterrupted "Interrupt della macchina emulata eseguito"

#define kStrCancelledInterrupt "Interrupt annullato"

#define kStrConfirmQuit "Sei sicuro di voler uscire? Dovresti spegnere la macchina emulata prima di uscire da ^p per prevenire potenziali danni alle immagini disco montate. Premi una lettera:"
#define kStrQuitDo kStrCmdQuit
#define kStrQuitNo kStrCmdCancel

#define kStrCancelledQuit "Uscita annullata"

#define kStrModeConfirmReset "Modalit;`a Controllo : Conferma Riavviare"
#define kStrModeConfirmInterrupt "Modalit;`a Controllo : Conferma l;}Interrupt"
#define kStrModeConfirmQuit "Modalit;`a Controllo : Conferma Uscire"
#define kStrModeSpeedControl "Modalit;`a Controllo : Controllo Velocit;`a"
#define kStrModeControlBase "Modalit;`a Controllo (Digita ;]H;} per l;}aiuto)"
#define kStrModeControlHelp "Modalit;`a Controllo"
#define kStrModeMessage "Messaggio (Digita ;]C;} per continuare)"

#define kStrMenuFile "Archivio"
#define kStrMenuSpecial "Speciale"
#define kStrMenuHelp "Aiuto"

#define kStrMenuItemAbout "Informazioni su ^p"
#define kStrMenuItemOpen "Apri Immagine Disco"
#define kStrMenuItemQuit "Esci"
#define kStrMenuItemMore "Altri Comandi"

#define kStrAppMenuItemHide "Nascondi ^p"
#define kStrAppMenuItemHideOthers "Nascondi altre"
#define kStrAppMenuItemShowAll "Mostra tutte"
#define kStrAppMenuItemQuit "Esci da ^p"

#define kStrCmdCopyOptions "Copia variation options"
#define kStrHaveCopiedOptions "Variation options copiate"
