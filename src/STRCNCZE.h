/*
	STRCNCZE.h

	Copyright (C) 2017 Anonymous, Paul C. Pratt

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
	STRing CoNstants for CZEch

	These strings were translated by Anonymous.
*/

#define kStrAboutTitle "O programu"
#define kStrAboutMessage "Pro zobrazen;ei informac;ei o programu, pou;vzijte p;vr;eikaz ;]A;} v ovl;eadac;eim m;eodu ^p. Pro informace o ovl;eadac;eim m;eodu, pod;eivejte se do ;[V;eice p;vr;eikaz;ru;ll;{ v menu ;[Dal;vs;ei mo;vznosti;{."

#define kStrMoreCommandsTitle "V kontroln;eim m;eodu ^p jsou dostupn;ee dal;vs;ei p;vr;eikazy."
#define kStrMoreCommandsMessage "pro pou;vzit;ei ovl;eadac;eiho m;eodu stiskn;vete a dr;vzte ;]^c;}. V ovl;eadac;eim m;eodu z;rustanete, dokud nepust;eite ;]^c;}. stiskn;vete ;]H;} v ovl;eadac;eim m;eodu pro zobrazen;ei dostupn;eych p;vr;eikaz;ru."

#define kStrTooManyImagesTitle "P;vr;eili;vs mnoho obraz;ru disk;ru"
#define kStrTooManyImagesMessage "Tolik obraz;ru disk;ru p;vripojit nezvl;eadnu. zkuste jeden odpojit."

#define kStrImageInUseTitle "Obraz disku je pou;vz;eiv;ean"
#define kStrImageInUseMessage "Nemohu p;vripojit obraz disku, proto;vze je pou;vz;eiv;ean jinou aplikac;ei nebo otev;vren v ^p."

#define kStrOutOfMemTitle "Nedostatek pam;veti"
#define kStrOutOfMemMessage "Pro su;vst;ven;ei ^p nen;ei dostatek pam;veti."

#define kStrNoROMTitle "ROM nenalezen"
#define kStrNoROMMessage "Nemohu naj;eit soubor ROM ;[^r;{. Pro v;eice informac;ei se pod;eivejte na: ;[^w;{."

#define kStrCorruptedROMTitle "Kontroln;ei sou;vcet ROMu selhal"
#define kStrCorruptedROMMessage "ROM ;[^r;{ m;ru;vze b;eyt po;vskozen."

#define kStrUnsupportedROMTitle "Nepodporovan;ey ROM"
#define kStrUnsupportedROMMessage "ROM ;[^r;{ se ;eusp;ve;vsn;ve na;vcetl, ale s touto verz;ei ROMu neum;eim pracovat."

#define kStrQuitWarningTitle "Pros;eim vypn;vete emulovan;ey po;vc;eita;vc p;vred zav;vren;eim."
#define kStrQuitWarningMessage "Pro vynucen;ei zav;vren;ei ^p (riskujete po;vskozen;ei p;vripojen;eych disk;ru),pou;vzijte p;vr;eikaz ;]Q;} ovl;eadac;eiho m;eodu ^p. Pro informace o ovl;eadac;eim m;eodu, pod;eivejte se do ;[V;eice p;vr;eikaz;ru;ll;{ v menu ;[Dal;vs;ei mo;vznosti;{."

#define kStrReportAbnormalTitle "Abnorm;ealn;ei situace"
#define kStrReportAbnormalMessage "Emulovan;ey po;vc;eite;vc se pokou;vs;ei o operaci, kter;ea nebyla o;vcek;eav;eana p;vri b;ve;vzn;eem pou;vz;eiv;ean;ei."

#define kStrBadArgTitle "Nezn;eam;ey argument"
#define kStrBadArgMessage "Nerozum;eim jednomu z p;vr;eikazov;eych argument;ru, budu ho ignorovat."

#define kStrOpenFailTitle "Otev;vren;ei selhalo"
#define kStrOpenFailMessage "Neopda;vrilo se mi otev;vr;eit obraz disku."

#define kStrNoReadROMTitle "Ne;vciteln;ey ROM"
#define kStrNoReadROMMessage "Na;vsel jsem ROM ;[^r;{, ale nemohu ho p;vre;vc;eist."

#define kStrShortROMTitle "P;vr;eili;vs kr;eatk;ey ROM"
#define kStrShortROMMessage "ROM ;[^r;{ je krat;vs;ei ne;vz obvykle."

/* state of a boolean option */
#define kStrOn "on"
#define kStrOff "off"

/* state of a key */
#define kStrPressed "stisknuto"
#define kStrReleased "pu;vst;veno"

/* state of Stopped */
#define kStrStoppedOn kStrOn
#define kStrStoppedOff kStrOff

/* About Screen */
#define kStrProgramInfo "^v"
#define kStrSponsorIs "^v. Tuto variaci sponzoruje:"
#define kStrWorkOfMany "Copyright ^y. ^p Obsahuje pr;eaci mnoha lid;ei. Tuto verzi udr;vzuje:"
#define kStrForMoreInfo "Pro v;eice informac;ei jd;vete na:"
#define kStrLicense "^p je distribuov;ean poj licenc;ei GNU GPL, verze 2."
#define kStrDisclaimer " ^p je distribuov;ean s nad;vej;ei, ;vze ude pou;vz;eiv;ean, ale BEZ JAK;eEKOLIV Z;eARUKY;ls t;ee;vz bez implikovan;ee z;earuky OBCHODOVATELNOSTI nebo VHODNOSTI PRO UR;vCIT;eY ;eU;vCEL."

/* Help Screen */
#define kStrHowToLeaveControl "Pro opu;vst;ven;ei ovl;ead;eac;eiho m;eodu pus;vtte ;]^c;}."
#define kStrHowToPickACommand "jinak stiskn;vete p;eismeno. Dostupn;ee p;vr;eikazy jsou:"
#define kStrCmdAbout "O programu (informace o verzi)"
#define kStrCmdOpenDiskImage "Otev;vr;eit obraz disku"
#define kStrCmdQuit "Zav;vr;eit"
#define kStrCmdSpeedControl "Ovl;ead;ean;ei rychlosti;ll (^s)"
#define kStrCmdMagnifyToggle "P;vribl;ei;vzen;ei (^g)"
#define kStrCmdFullScrnToggle "Cel;ea obrazovka (^f)"
#define kStrCmdCtrlKeyToggle "P;vrep;ein;ean;ei emulovan;eeho ;]^m;} (^k)"
#define kStrCmdReset "Reset"
#define kStrCmdInterrupt "Vyru;vsit"
#define kStrCmdHelp "Pomoc (uk;eaz;eat tuto str;eanku)"

/* Speed Control Screen */
#define kStrCurrentSpeed "Sou;vcasn;ea rychlost: ^s"
#define kStrSpeedAllOut "Maximum"
#define kStrSpeedStopped "zastavit (^h)"
#define kStrSpeedBackToggle "b;ve;vz;vet v pozad;ei (^b)"
#define kStrSpeedAutoSlowToggle "automatick;ee zpomalen;ei (^l)"
#define kStrSpeedExit "Zav;vr;eit ovl;ead;ean;ei rychlosti"

#define kStrNewSpeed "Rychlost: ^s"
#define kStrSpeedValueAllOut kStrSpeedAllOut

#define kStrNewStopped "Je zastaven: ^h."
#define kStrNewRunInBack "B;ve;vz;ei na pozad;ei: ^b."
#define kStrNewAutoSlow "Je automaticky zpomalen: ^l."

#define kStrNewMagnify "P;vribl;ei;vzen;ei: ^g."

#define kStrNewFullScreen "Na celou obrazovku: ^f."

#define kStrNewCntrlKey "Emulovan;ey ;]^m;}: ^k."

#define kStrCmdCancel "Storno"

#define kStrConfirmReset "Opravdu chcete resetovat emulovan;ey po;vc;eita;vc? Ztrat;eite neulo;vzen;ee zm;veny a riskujete po;vskozen;ei p;vripojen;eych obraz;ru disk;ru. Stiskn;vete p;eismeno:"
#define kStrResetDo "resetovat"
#define kStrResetNo kStrCmdCancel

#define kStrHaveReset "Resetoval jsem emulovan;ey po;vc;eita;vc"

#define kStrCancelledReset "Resetov;ean;ei zru;vseno"

#define kStrConfirmInterrupt "Opravdu chcete vyru;vsit emulovan;ey po;vc;eita;vc? Spust;ei se nainstalovan;ey debugger. Stiskn;vete p;eismeno:"
#define kStrInterruptDo "vyru;vsit"
#define kStrInterruptNo kStrCmdCancel

#define kStrHaveInterrupted "Vyru;vsil jsem emulovan;ey po;vc;eita;vc"

#define kStrCancelledInterrupt "Vyru;vsen;ei zru;vseno"

#define kStrConfirmQuit "Opravdu chcete zav;vr;eit ^p? M;veli byste vypnout emulovan;ey po;vc;eita;vc p;vred vypnut;eim pro zabr;ean;ven;ei po;vskozen;ei p;vripojen;eych obraz;ru disk;ru. Stiskn;vete p;eismeno:"
#define kStrQuitDo "odej;eit"
#define kStrQuitNo kStrCmdCancel

#define kStrCancelledQuit "Zav;vren;ei zru;vseno"

#define kStrModeConfirmReset " : Confirm Reset"
#define kStrModeConfirmInterrupt "ovl;ead;eac;ei m;eod : Potvrdit vyru;vsen;ei"
#define kStrModeConfirmQuit "ovl;ead;eac;ei m;eod : Potvrdit zav;vren;ei"
#define kStrModeSpeedControl "ovl;ead;eac;ei m;eod : Ovl;ead;ean;ei rychlosti"
#define kStrModeControlBase "ovl;ead;eac;ei m;eod (stiskn;vete ;]H;} pro pomoc) "
#define kStrModeControlHelp "ovl;ead;eac;ei m;eod"
#define kStrModeMessage "Zpr;eava (Stiskn;vete ;]C;} pro pokra;vcov;ean;ei)"

#define kStrMenuFile "Soubor"
#define kStrMenuSpecial "Dal;vs;ei mo;vznosti"
#define kStrMenuHelp "Pomoc"

#define kStrMenuItemAbout "O ^p"
#define kStrMenuItemOpen "Otev;vr;eit obraz disku"
#define kStrMenuItemQuit "Odej;eit"
#define kStrMenuItemMore "V;eice p;vr;eikaz;ru"

#define kStrAppMenuItemHide "Skr;eyt ^p"
#define kStrAppMenuItemHideOthers "Skr;eyt ostatn;ei"
#define kStrAppMenuItemShowAll "Zobrazit v;vse"
#define kStrAppMenuItemQuit "Ukon;vcit ^p"

#define kStrCmdCopyOptions "Kop;eirovat variation options"
#define kStrHaveCopiedOptions "Variation options zkop;eirov;eany"
