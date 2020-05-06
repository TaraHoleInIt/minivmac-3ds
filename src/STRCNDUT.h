/*
	STRCNDUT.h

	Copyright (C) 2006 Luc Pauwels, Paul C. Pratt

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
	STRing CoNstants for DUTch

	These strings were translated by Luc Pauwels.
*/

#define kStrAboutTitle "Over ^p"
#define kStrAboutMessage "Gebruik het ;]A;}-commando van de ^p controlemodus om meer informatie over deze applicatie te tonen. Om meer te weten te komen over de controlemodus, zie ;[Meer Commando;}s;ll;{ in het ;[Speciaal;{ menu."

#define kStrMoreCommandsTitle "Meer commando;}s zijn beschikbaar in de ^p controlemodus."
#define kStrMoreCommandsMessage "Om naar de controlemodus te gaan, hou de ;]^c;} toets ingedrukt. Je blijft in de controlemodus zolang je de ;]^c;} toets ingedrukt houdt.  Type ;]H;} in de controlemodus om een lijst van beschikbare commando;}s te zien."

#define kStrTooManyImagesTitle "Teveel schijfkopiebestanden"
#define kStrTooManyImagesMessage "Het maximaal aantal schijfkopiebestanden is in gebruik.  Probeer er ;ee;een te verwijderen."

#define kStrImageInUseTitle "Schijfkopiebestand in gebruik"
#define kStrImageInUseMessage "Ik kan het schijfkopiebestand niet openen omdat het al in gebruik is door een andere applicatie of omdat het al geopend is in ^p."

#define kStrOutOfMemTitle "Niet genoeg geheugen"
#define kStrOutOfMemMessage "Er is niet genoeg geheugen beschikbaar om ^p te openen."

#define kStrNoROMTitle "ROM bestand niet gevonden"
#define kStrNoROMMessage "Ik kan het ROM bestand ;[^r;{ niet vinden. Voor meer informatie, zie ;[^w;{."

#define kStrCorruptedROMTitle "ROM controlegetal verkeerd"
#define kStrCorruptedROMMessage "Het ROM bestand ;[^r;{ is mogelijk beschadigd."

#define kStrUnsupportedROMTitle "Niet ondersteunde ROM"
#define kStrUnsupportedROMMessage "Het ROM bestand ;[^r;{ werd succesvol ingeladen, maar ik ondersteun deze ROM versie niet."

#define kStrQuitWarningTitle "Zet de ge;uemuleerde computer uit vooraleer de applicatie te stoppen."
#define kStrQuitWarningMessage "Om ^p geforceerd te stoppen, met het risico dat het gebruikte schijfkopiebestand beschadigd geraakt, gebruik het ;]Q;} commando van de ^p controlemodus. Om meer te weten te komen over de controlemodus, zie ;[Meer Commando;}s;{ in het ;[Speciaal;{ menu."

#define kStrReportAbnormalTitle "Abnormale situatie"
#define kStrReportAbnormalMessage "De ge;uemuleerde computer probeert een operatie uit te voeren die onder normale omstandigheden niet kan voorkomen."

#define kStrBadArgTitle "Onbekend argument"
#define kStrBadArgMessage "Ik herkende ;ee;een van de commandolijn-argumenten niet en heb het genegeerd."

#define kStrOpenFailTitle "Open operatie gefaald"
#define kStrOpenFailMessage "Ik kon het schijfkopiebestand niet openen."

#define kStrNoReadROMTitle "Het ROM bestand kan niet gelezen worden"
#define kStrNoReadROMMessage "Ik kon het ROM bestand ;[^r;{vinden, maar ik kon het niet lezen."

#define kStrShortROMTitle "ROM bestand te kort"
#define kStrShortROMMessage "Het ROM bestand ;[^r;{ is korter dan verwacht."

/* state of a boolean option */
#define kStrOn "aan"
#define kStrOff "uit"

/* state of a key */
#define kStrPressed "ingedrukt"
#define kStrReleased "losgelaten"

/* state of Stopped */
#define kStrStoppedOn kStrOn
#define kStrStoppedOff kStrOff

/* About Screen */
#define kStrProgramInfo "^v"
#define kStrSponsorIs "^v. This variation is made for:"
#define kStrWorkOfMany "Copyright ^y. ^p is het werk van velen. Deze versie wordt onderhouden door:"
#define kStrForMoreInfo "Voor meer informatie, zie:"
#define kStrLicense "^p wordt verspreid onder de voorwaarden van de GNU Public License, versie 2."
#define kStrDisclaimer " ^p wordt verspreid in de hoop dat het bruikbaar is, maar ZONDER ENIGE GARANTIE, zelfs zonder impliciete garanties van VERKOOPBAARHEID noch BRUIKBAARHEID VOOR EEN BEPAALD DOELEINDE."

/* Help Screen */
#define kStrHowToLeaveControl "Laat de ;]^c;} toets los om de controlemodus te verlaten."
#define kStrHowToPickACommand "Zoniet, typ een letter.  De beschikbare commando;}s zijn:"
#define kStrCmdAbout "Over ^p (informatie over deze versie)"
#define kStrCmdOpenDiskImage "Open schijfkopiebstand;ll"
#define kStrCmdQuit "Stop"
#define kStrCmdSpeedControl "Snelheidscontrole;ll (^s)"
#define kStrCmdMagnifyToggle "Schermvergroting (^g)"
#define kStrCmdFullScrnToggle "Gebruik het volledig scherm (^f)"
#define kStrCmdCtrlKeyToggle "Ge;uemuleerde ;]^m;} toets (^k)"
#define kStrCmdReset "Herstart"
#define kStrCmdInterrupt "Onderbreek"
#define kStrCmdHelp "Hulp (toon deze pagina)"

/* Speed Control Screen */
#define kStrCurrentSpeed "Huidige snelheid: ^s"
#define kStrSpeedAllOut "Zo snel mogelijk"
#define kStrSpeedStopped "Stopschakelaar (^h)"
#define kStrSpeedBackToggle "Schakelaar om in de achtergrond te draaien (^b)"
#define kStrSpeedAutoSlowToggle "AutoSlow (^l)"
#define kStrSpeedExit "Verlaat Snelheidscontrole"

#define kStrNewSpeed "Snelheid: ^s"
#define kStrSpeedValueAllOut kStrSpeedAllOut

#define kStrNewStopped "Stopschakelaar is ^h."
#define kStrNewRunInBack "In de achtergrond draaien is ^b."
#define kStrNewAutoSlow "AutoSlow is ^l."

#define kStrNewMagnify "Schermvergroting is ^g."

#define kStrNewFullScreen "Volledig scherm gebruiken is ^f."

#define kStrNewCntrlKey "Ge;uemuleerde ;]^m;} toets ^k."

#define kStrCmdCancel "annuleer"

#define kStrConfirmReset "Bent u er zeker van dat u de ge;uemuleerde computer wilt herstarten?  Werk dat niet bewaard werd zal verloren gaan, en er bestaat een kans dat het gebruikte schijfkopiebestand beschadigd geraakt. Typ een letter:"
#define kStrResetDo "herstart"
#define kStrResetNo kStrCmdCancel

#define kStrHaveReset "De ge;uemuleerde computer werd herstart"

#define kStrCancelledReset "Herstart werd geannuleerd"

#define kStrConfirmInterrupt "Bent u er zeker van dat u de ge;uemuleerde computer wilt onderbreken?  Dit zal de eventueel ge;uinstalleerde debugger activeren. Typ een letter:"
#define kStrInterruptDo "onderbreken"
#define kStrInterruptNo kStrCmdCancel

#define kStrHaveInterrupted "De ge;uemuleerde computer werd onderbroken"

#define kStrCancelledInterrupt "Onderbreking geannuleerd"

#define kStrConfirmQuit "Bent u er zeker van dat u wilt ^p wilt stoppen?  U zou eerst de ge;uemuleerde computer moeten uitzetten alvorens te stoppen om beschadiging van het gebruikte schijfkopiebestand te vermijden. Typ een letter:"
#define kStrQuitDo "stop"
#define kStrQuitNo kStrCmdCancel

#define kStrCancelledQuit "Stoppen geannuleerd"

#define kStrModeConfirmReset "Controlemodus : Bevesting herstarten"
#define kStrModeConfirmInterrupt "Controlemodus : Bevesting onderbreking"
#define kStrModeConfirmQuit "Controlemodus : Bevesting stoppen"
#define kStrModeSpeedControl "Controlemodus : Snelheidscontrole"
#define kStrModeControlBase "Controlemodus (Typ ;]H;} voor meer hulp)"
#define kStrModeControlHelp "Controlemodus"
#define kStrModeMessage "Boodschap (Typ ;]C;} om verder te gaan)"

#define kStrMenuFile "Archief"
#define kStrMenuSpecial "Speciaal"
#define kStrMenuHelp "Hulp"

#define kStrMenuItemAbout "Over ^p"
#define kStrMenuItemOpen "Open schijfkopiebestand"
#define kStrMenuItemQuit "Stop"
#define kStrMenuItemMore "Meer Commando;}s"

#define kStrAppMenuItemHide "Verberg ^p"
#define kStrAppMenuItemHideOthers "Verberg andere"
#define kStrAppMenuItemShowAll "Toon alles"
#define kStrAppMenuItemQuit "Stop ^p"

#define kStrCmdCopyOptions "Kopieer variation options"
#define kStrHaveCopiedOptions "Variation options gekopieerd"
