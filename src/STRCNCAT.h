/*
	STRCNCAT.h

	Copyright (C) 2017 ArduinoXino, Paul C. Pratt

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file;ls see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY;ls without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

/*
	STRing CoNstants for CATalan

	These strings were translated by ArduinoXino.
*/

#define kStrAboutTitle "Sobre"
#define kStrAboutMessage "Per mostrar informaci;eo sobre aquest programa, utilitzeu el comandament ;]A;} del ^p Mode Control. Per coneixer m;ees sobre el Mode Control, vegi ;[M;ees Comandaments;ll;{ en el menu ;[Especial;{ ."

#define kStrMoreCommandsTitle "M;ees comandaments est;`an disponibles en el mode de control de ^p ."
#define kStrMoreCommandsMessage "Per entrar en el mode de control, mantingueu premut la tecla ;]^c;} . Seguir;`a en el mode control fins que deixeu anar la tecla ;]^c;} . Polsi ;]H;} en el mode control per llistar els comandaments disponibles."

#define kStrTooManyImagesTitle "Massa imatges de disc"
#define kStrTooManyImagesMessage "No es poden muntar tantes imatges de disc. Proveu treure alguna."

#define kStrImageInUseTitle "Imatge de disc en ;eus"
#define kStrImageInUseMessage "No es pot muntar la imatge disc perqu;`e ja est;`a en ;eus per una altra aplicaci;eo, o ja est;`a oberta a ^p."

#define kStrOutOfMemTitle "Mem;`oria insuficient"
#define kStrOutOfMemMessage "No hi ha prou mem;`oria disponible per a llan;car ^p."

#define kStrNoROMTitle "Impossible localitzar la imatge ROM"
#define kStrNoROMMessage "No es troba la imatge del fitxer ROM ;[^r;{. Per a m;ees informaci;eo, veure: ;[^w;{."

#define kStrCorruptedROMTitle "El control checksum de la ROM ha fracassat"
#define kStrCorruptedROMMessage "El fitxer d;laimatge ROM ;[^r;{ pot estar corromput."

#define kStrUnsupportedROMTitle "ROM no suportada"
#define kStrUnsupportedROMMessage "Fitxer d;laimatge ROM ;[^r;{ carregat amb ;`exit, per;`o aquesta versi;eo no est;`a suportada."

#define kStrQuitWarningTitle "Si us plau, apagueu l;laordinador emulat abans de sortir."
#define kStrQuitWarningMessage "Per for;car ^p sortir, amb risc de corrupci;eo en les imatges de discos muntades, utilitzeu la ordre ;]Q;} del ^p Mode Control. Per coneixer m;ees sobre el Mode Control, vegi ;[M;ees Comandaments;ll;{ al menu ;[Especial;{ ."

#define kStrReportAbnormalTitle "Situaci;eo anormal"
#define kStrReportAbnormalMessage "L;laordinador emulat intenta fer una operaci;eo no esperada en un ;eus normal."

#define kStrBadArgTitle "Argument desconegut"
#define kStrBadArgMessage "No es va comprendre algun dels arguments de la l;einia d;laordres, i es va ignorar."

#define kStrOpenFailTitle "Obertura fracassada"
#define kStrOpenFailMessage "No s;laha pogut obrir la imatge disc."

#define kStrNoReadROMTitle "No es pot obrir la imatge ROM"
#define kStrNoReadROMMessage "Es va trobar el fitxer d;laimatge ROM ;[^r;{, per;`o no es va poder llegir."

#define kStrShortROMTitle "Imatge ROM massa curta"
#define kStrShortROMMessage "El fitxer d;laimatge ROM ;[^r;{ ;ees m;ees curt del que hauria de ser."

/* state of a boolean option */
#define kStrOn "activat"
#define kStrOff "desactivat"

/* state of a key */
#define kStrPressed "polsada"
#define kStrReleased "deixada anar"

/* state of Stopped */
#define kStrStoppedOn kStrOn
#define kStrStoppedOff kStrOff

/* About Screen */
#define kStrProgramInfo "^v"
#define kStrSponsorIs "^v. Aquesta variaci;eo est;`a patrocinada per:"
#define kStrWorkOfMany "Copyright ^y. ^p cont;ee el treball de moltes persones. Aquesta versi;eo ;ees mantinguda per:"
#define kStrForMoreInfo "Per m;ees informaci;eo, vegeu:"
#define kStrLicense "^p es distribueix sota els termes de la llic;`encia p;eublica GNU, versi;eo 2."
#define kStrDisclaimer " ^p es distribueix amb l;laesperan;ca de ser ;eutil, per;`o SENSE CAP GARANTIA;ls fins i tot sense la garantia impl;eicita de MERCANTIBILITAT O ADECUACI;eO PER A UN ;eUS PARTICULAR."

/* Help Screen */
#define kStrHowToLeaveControl "Per sortir del Mode Control, deixeu anar la tecla ;]^c;} ."
#define kStrHowToPickACommand "Sin;eo, polsi una lletra. Els comandaments disponibles s;eon:"
#define kStrCmdAbout "Sobre (informaci;eo sobre versi;eo)"
#define kStrCmdOpenDiskImage "Obrir imatge de disc..."
#define kStrCmdQuit "Sortir"
#define kStrCmdSpeedControl "Control de velocitat;ll (^s)"
#define kStrCmdMagnifyToggle "Magnificaci;eo (^g)"
#define kStrCmdFullScrnToggle "Pantalla completa (^f)"
#define kStrCmdCtrlKeyToggle "Toggle emulaci;eo tecla ;]^m;} (^k)"
#define kStrCmdReset "Restablir"
#define kStrCmdInterrupt "Interrompre"
#define kStrCmdHelp "Ajuda (mostra aquesta p;`agina)"

/* Speed Control Screen */
#define kStrCurrentSpeed "Velocitat actual: ^s"
#define kStrSpeedAllOut "M;`axima velocitat"
#define kStrSpeedStopped "Aturar (^h)"
#define kStrSpeedBackToggle "Executar en segon pla (^b)"
#define kStrSpeedAutoSlowToggle "AutoSlow (^l)"
#define kStrSpeedExit "Sortir del control de velocitat"

#define kStrNewSpeed "Velocitat: ^s"
#define kStrSpeedValueAllOut kStrSpeedAllOut

#define kStrNewStopped "Aturar est;ea ^h."
#define kStrNewRunInBack "Executar en segon pla est;`a ^b."
#define kStrNewAutoSlow "AutoSlow est;`a ^l."

#define kStrNewMagnify "Magnificaci;eo est;`a ^g."

#define kStrNewFullScreen "Pantalla completa est;`a ^f."

#define kStrNewCntrlKey "Emulat tecla ;]^m;} ^k."

#define kStrCmdCancel "cancel;l.lar"

#define kStrConfirmReset "Est;`a segur de voler reiniciar l;laordinador emulat? Els canvis no salvats es perdran, i hi ha risc de corrupci;eo en les imatges de discos muntades. Premeu una tecla:"
#define kStrResetDo "restablir"
#define kStrResetNo kStrCmdCancel

#define kStrHaveReset "S;laha reiniciat l;laordinador emulat"

#define kStrCancelledReset "Reset cancel;l.lat"

#define kStrConfirmInterrupt "Est;`a segur de voler interrompre l;laordinador emulat? Aix;`o invocar;`a qualsevol debugger instal;l.lat. Premeu una tecla:"
#define kStrInterruptDo "interrompre"
#define kStrInterruptNo kStrCmdCancel

#define kStrHaveInterrupted "S;laha interromput l;laordinador emulat"

#define kStrCancelledInterrupt "Interrupci;eo cancel;l.lada"

#define kStrConfirmQuit "Est;`a segur de desitjar sortir de ^p? Hauria apagar l;laordinador emulat abans de sortir, per evitar corrupcions en les imatges de discos muntades. Premeu una tecla:"
#define kStrQuitDo "sortir"
#define kStrQuitNo kStrCmdCancel

#define kStrCancelledQuit "Sortida cancel;l.lada"

#define kStrModeConfirmReset "Mode Control : Confirma reset"
#define kStrModeConfirmInterrupt "Mode Control : Confirma Interrupci;eo"
#define kStrModeConfirmQuit "Mode Control : Confirma sortida"
#define kStrModeSpeedControl "Mode Control : Control de velocitat"
#define kStrModeControlBase "Mode Control (Premi ;laH;la per a l;laajuda)"
#define kStrModeControlHelp "Mode Control"
#define kStrModeMessage "Missatge (Premi ;]C;} per continuar)"

#define kStrMenuFile "Fitxer"
#define kStrMenuSpecial "Especial"
#define kStrMenuHelp "Ajuda"

#define kStrMenuItemAbout "Sobre ^p"
#define kStrMenuItemOpen "Obrir imatge de disc"
#define kStrMenuItemQuit "Sortir"
#define kStrMenuItemMore "M;eess comandaments"

#define kStrAppMenuItemHide "Ocultar ^p"
#define kStrAppMenuItemHideOthers "Ocultar la resta"
#define kStrAppMenuItemShowAll "Mostrar-ho tot"
#define kStrAppMenuItemQuit "Sortir de ^p"

#define kStrCmdCopyOptions "Copiar variation options"
#define kStrHaveCopiedOptions "Variation options copiat"
