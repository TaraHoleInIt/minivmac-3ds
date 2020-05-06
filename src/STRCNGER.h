/*
	STRCNGER.h

	Copyright (C) 2006 Paul C. Pratt, Detlef Kahner

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
	STRing CoNstants for GERman

	Translated by Detlef Kahner
*/

#define kStrAboutTitle ";uUber ^p"
#define kStrAboutMessage "Mehr Informationen zu ^p erhalten Sie ;uuber die Taste ;]A;} im Kontrollmodus. Informationen zum Kontrollmodus erhalten Sie ;uuber den Men;uubefehl ;[Mehr Befehle;ll;{ im Spezialmen;uu."

#define kStrMoreCommandsTitle "Mehr Befehle sind im Kontrollmodus abrufbar."
#define kStrMoreCommandsMessage "Um den Kontrollmodus von ^p aufzurufen, dr;uucken Sie die ;]^c;}-Taste und lassen diese gedr;uuckt. Sie verbleiben im Kontrollmodus, solange Sie diese Taste gedr;uuckt halten. Eine Liste verf;uugbarer Befehle erhalten Sie w;uahrenddessen durch Dr;uucken der Taste ;]H;}."

#define kStrTooManyImagesTitle "Zuviele Disketten-Images."
#define kStrTooManyImagesMessage "Soviele Images k;uonnen nicht gemountet sein. Bitte werfen Sie einige aus."

#define kStrImageInUseTitle "Disketten-Image bereits in Gebrauch."
#define kStrImageInUseMessage "Disketten-Image kann nicht gemountet werden, weil es entweder bereits in ^p gemountet ist oder von einem anderen Programm (z. B. dem Finder) verwendet wird."

#define kStrOutOfMemTitle "Zuwenig Arbeitsspeicher."
#define kStrOutOfMemMessage "Es ist nicht gen;uugend Arbeitsspeicher vorhanden, um ^p zu starten."

#define kStrNoROMTitle "ROM nicht gefunden."
#define kStrNoROMMessage "Das ROM namens ;[^r;{ kann nicht gefunden werden. Mehr Informationen hier: ;[^w;{"

#define kStrCorruptedROMTitle "ROM-Pr;uufsumme fehlgeschlagen."
#define kStrCorruptedROMMessage "Das ROM ;[^r;{ ist m;uoglicherweise besch;uadigt."

#define kStrUnsupportedROMTitle "ROM wird nicht unterst;uutzt."
#define kStrUnsupportedROMMessage "Das ROM ;[^r;{ wurde erfolgreich geladen, aber diese Version von ^p unterst;uutzt es nicht."

#define kStrQuitWarningTitle "Bitte fahren Sie das emulierte MacOS herunter, bevor Sie ^p beenden."
#define kStrQuitWarningMessage "Um ^p sofort zu beenden, aber Besch;uadigungen an den gemounteten Disketten-Images in Kauf zu nehmen, dr;uucken Sie die Taste ;]Q;} innerhalb des Kontrollmodus. Zugriff zum Kontrollmodus von ^p erhalten Sie, indem Sie die ;]^c;}-Taste gedr;uuckt halten."

#define kStrReportAbnormalTitle "Unerwartete Situation"
#define kStrReportAbnormalMessage "Der emulierte Rechner hat versucht, eine Operation durchzuf;uuhren, die im Rahmen eines normalen Gebrauches nicht vorgesehen war."

#define kStrBadArgTitle "Unbekannter Befehl."
#define kStrBadArgMessage "Befehlseingabe nicht verstanden und deshalb ignoriert."

#define kStrOpenFailTitle ";uOffnen fehlgeschlagen."
#define kStrOpenFailMessage "Das Disk-Image konnte nicht ge;uoffnet werden."

#define kStrNoReadROMTitle "ROM nicht lesbar."
#define kStrNoReadROMMessage "Das ROM namens ;[^r;{ wurde gefunden, kann aber von dieser Version von ^p nicht verwendet werden."

#define kStrShortROMTitle "ROM-Image zu klein."
#define kStrShortROMMessage "Das ROM ;[^r;{ ist kleiner als erwartet."

/* state of a boolean option */
#define kStrOn "an"
#define kStrOff "aus"

/* state of a key */
#define kStrPressed "gedr;uuckt"
#define kStrReleased "losgelassen"

/* state of Stopped */
#define kStrStoppedOn kStrOn
#define kStrStoppedOff kStrOff

/* About Screen */
#define kStrProgramInfo "^v"
#define kStrSponsorIs "^v. This variation is made for:"
#define kStrWorkOfMany "Copyright ^y. ^p enth;ualt die Arbeit mehrerer Personen. Diese Version wird gepflegt von:"
#define kStrForMoreInfo "Mehr Informationen:"
#define kStrLicense "^p wird unter den Bestimmungen der GNU Public License, Version 2, verbreitet."
#define kStrDisclaimer " ^p wird angeboten in der Hoffnung, brauchbar zu sein, aber OHNE JEDE GARANTIE;ls gleichfalls ohne garantierte EIGNUNG ZU EINEM BESTIMMTEN ZWECK."

/* Help Screen */
#define kStrHowToLeaveControl "Um den Kontrollmodus zu verlassen, lassen Sie ;]^c;} los."
#define kStrHowToPickACommand "Anderenfalls dr;uucken Sie einen dieser Buchstaben:"
#define kStrCmdAbout "Informationen zu dieser Version"
#define kStrCmdOpenDiskImage "Disketten-Image ;uoffnen;ll"
#define kStrCmdQuit "Beenden"
#define kStrCmdSpeedControl "Geschwindigkeitskontrolle;ll (^s)"
#define kStrCmdMagnifyToggle "Vergr;uosserung (^g)"
#define kStrCmdFullScrnToggle "Vollbild (^f)"
#define kStrCmdCtrlKeyToggle "Emulierte ;]^m;}-Taste (^k)"
#define kStrCmdReset "Neustarten (Reset durchf;uuhren)"
#define kStrCmdInterrupt "Interrupt"
#define kStrCmdHelp "Hilfe (zeigt diese Befehlsliste)"

/* Speed Control Screen */
#define kStrCurrentSpeed "Momentane Geschwindigkeit: ^s"
#define kStrSpeedAllOut "H;uochstgeschwindigkeit"
#define kStrSpeedStopped "^p jetzt anhalten (^h)"
#define kStrSpeedBackToggle "^p im Hintergrund anhalten (^b)"
#define kStrSpeedAutoSlowToggle "AutoSlow (^l)"
#define kStrSpeedExit "Geschwindigkeits-Einstellungen verlassen"

#define kStrNewSpeed "Neue Geschwindigkeit: ^s"
#define kStrSpeedValueAllOut kStrSpeedAllOut

#define kStrNewStopped "^p ist jetzt angehalten (^h)"
#define kStrNewRunInBack "^p anhalten, wenn nicht aktiv (^b)"
#define kStrNewAutoSlow "AutoSlow (^l)"

#define kStrNewMagnify "Vergr;uo;serung ist ^g"

#define kStrNewFullScreen "Vollbildmodus ist ^f"

#define kStrNewCntrlKey "Emulierte ;]^m;}-Taste ^k"

#define kStrCmdCancel "Abbrechen"

#define kStrConfirmReset "Wollen Sie den emulierten Rechner wirklich neustarten? Ungesicherte ;uAnderungen werden verlorengehen und gemountete Disketten-Images k;uonnen besch;uadigt werden. Dr;uucken Sie einen dieser Buchstaben:"
#define kStrResetDo "^p neustarten"
#define kStrResetNo "Nicht neustarten"

#define kStrHaveReset "Emulierter Rechner wurde zur;uuckgesetzt."

#define kStrCancelledReset "Neustart wird nicht durchgef;uuhrt"

#define kStrConfirmInterrupt "Interrupt f;uur diesen emulierten Rechner ausf;uuhren? Dies ruft alle implementierten Debugger auf. Dr;uucken Sie einen dieser Buchstaben:"
#define kStrInterruptDo "Interrupt ausf;uuhren"
#define kStrInterruptNo "Interrupt nicht ausf;uuhren"

#define kStrHaveInterrupted "Interrupt wurde durchgef;uuhrt"

#define kStrCancelledInterrupt "Interrupt wird nicht ausgef;uuhrt"

#define kStrConfirmQuit "Wollen Sie ^p wirklich beenden? Sie sollten zuvor den emulierten Rechner herunterfahren, um Besch;uadigungen an gemounteten Disketten-Images zu vermeiden. Dr;uucken Sie einen dieser Buchstaben:"
#define kStrQuitDo kStrCmdQuit
#define kStrQuitNo "Nicht beenden"

#define kStrCancelledQuit "^p wird nicht beendet"

#define kStrModeConfirmReset "Kontrollmodus : Neustarten von ^p best;uatigen"
#define kStrModeConfirmInterrupt "Kontrollmodus : Interrupt best;uatigen"
#define kStrModeConfirmQuit "Kontrollmodus : Beenden von ^p best;uatigen"
#define kStrModeSpeedControl "Kontrollmodus : Geschwindigkeitskontrolle"
#define kStrModeControlBase "Kontrollmodus (Tippen Sie ;]H;}, um Hilfe zu erhalten)"
#define kStrModeControlHelp "Kontrollmodus"
#define kStrModeMessage "Mitteilung (Tippen Sie ;]C;}, um fortzusetzen)"

#define kStrMenuFile "Ablage"
#define kStrMenuFile_win "Datei"
#define kStrMenuSpecial "Spezial"
#define kStrMenuHelp "Hilfe"

#define kStrMenuItemAbout ";uUber ^p"
#define kStrMenuItemOpen "Disketten-Image ;uoffnen"
#define kStrMenuItemQuit "Beenden"
#define kStrMenuItemMore "Mehr Befehle"

#define kStrAppMenuItemHide "^p ausblenden"
#define kStrAppMenuItemHideOthers "Andere ausblenden"
#define kStrAppMenuItemShowAll "Alle einblenden"
#define kStrAppMenuItemQuit "^p beenden"

#define kStrCntrlKyName "Control"
#define kStrCmdCopyOptions "Variation options kopieren"
#define kStrHaveCopiedOptions "Variation options kopiert"
