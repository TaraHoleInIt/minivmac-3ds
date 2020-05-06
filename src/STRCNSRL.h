/*
	STRCNSRL.h

	Copyright (C) 2018 SerbXenomorph, Paul C. Pratt

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
	STRing CoNstants for SeRbian Latin

	Converted from Serbian Cyrillic by 3 different online
	converters, which all gave identical results.

	comment from Serbian Cyrillic source follows:
*/

/*
	STRing CoNstants for SeRbian Cyrillic
	Translated by SerbXenomorph
	- If no support Unicode/UTF-8, etc. support, online cyrillic to latin convertor will be enough.
	- I have made some suggestions in the comments to avoid string repetition.
*/

#define kStrAboutTitle "O programu"
#define kStrAboutMessage "Da bi se pokazale informacije o programu, koristite ;]A;} komandu moda kontrole ^p . Da saznate vi;vse o kontrolnom modu, vidite ;[Jo;vs komandi;ll;{ u ;[Special;{ meniju."

#define kStrMoreCommandsTitle "Vi;vse komanda je dostupno u modu kontrole ^p."
#define kStrMoreCommandsMessage "Da bi ste u;vsli u kontrolni mod, pretisnite taster ;]^c;}. osta;ecete u njemu dok ga ne pustite. Pretisnite ;]H;} da prika;vzete listu dostupnih komandi."

#define kStrTooManyImagesTitle "Previ;vse diskovnih datoteka"
#define kStrTooManyImagesMessage "Ne mogu montirati toliko diskovnih datoteka. Izbacite jednu."

#define kStrImageInUseTitle "Diskovna datoteka se koristi"
#define kStrImageInUseMessage "Ne mogu montirati diskovnu datoteku ^p kako se ve;ec koristi u drugom programu."

#define kStrOutOfMemTitle "Nema dovoljno memorije"
#define kStrOutOfMemMessage "Nema dovoljno memorije da bi se pokrenuo ^p."
/*
	This can possibly be defined as ("%s  da bi se pokrenuo ^p.", kStrOutOfMemTitle) to avoid duplicated string, if used in a printf() like function.
*/

#define kStrNoROMTitle "Ne mogu na;eci ROM datoteku"
#define kStrNoROMMessage "Ne mogu na;eci ROM datoteku ;[^r;{. Za vi;vse informacija, vidite: ;[^w;{."
/*
	Same for this
*/

#define kStrCorruptedROMTitle "ROM provera neuspe;vsna"
#define kStrCorruptedROMMessage "ROM datoteka ;[^r;{ je mo;vzda o;vste;ecena."

#define kStrUnsupportedROMTitle "Nepodr;vzani ROM"
#define kStrUnsupportedROMMessage "ROM datoteka ;[^r;{ se u;vcitala uspe;vsno, ali ne podr;vzavam ROM verziju."

#define kStrQuitWarningTitle "Molimo ugasite emulisani kompjuter pre nego ;vsto iza;ddete iz programa."
#define kStrQuitWarningMessage "Da biste prisilili ^p da iza;dde, sa rizikom da se diskovni fajlovi o;vstete, koristi ;]Q;} komandu kontrolnog moda ^p. Da saznate vi;vse o kontrolnom modu, vidite ;[Jo;vs komandi;ll;{ u ;[Special;{ meniju."

#define kStrReportAbnormalTitle "Abnormalna situacija"
#define kStrReportAbnormalMessage "Emulisani ra;vcunar je poku;vsao operaciju koja ne bi trebalo de se izvr;vsi pri normalnom kori;vs;ecenju."

#define kStrBadArgTitle "Nepoznati argument"
#define kStrBadArgMessage "Nisam razumeo jedan od argumenata komandne linije, i ignorisao ga."

#define kStrOpenFailTitle "Otvaranje neuspe;vsno"
#define kStrOpenFailMessage "Nisam mogao otvoriti diskovnu datoteku."

#define kStrNoReadROMTitle "ROM datoteka ne;vcitljiva"
#define kStrNoReadROMMessage "Prona;vsao sam ROM datoteku ;[^r;{, ali je ne mogu pro;vcitati."

#define kStrShortROMTitle "ROM datoteka prekratka"
#define kStrShortROMMessage "ROM datoteka ;[^r;{ je kra;eca nego ;vsto bi trebala da bude."

/* state of a boolean option */
#define kStrOn "Vklju;vcen"
#define kStrOff "Isklju;vcen"

/* state of a key */
#define kStrPressed "pritisnut"
#define kStrReleased "pu;vsten"

/* state of Stopped */
#define kStrStoppedOn kStrOn
#define kStrStoppedOff kStrOff

/* About Screen */
#define kStrProgramInfo "^v"
#define kStrSponsorIs "^v. Ova varijacija je napravljena za:"
#define kStrWorkOfMany "Za;vsti;eceno autorskim pravima ^y. ^p sadr;vzi rad mnogo ljudi. Ovu verziju odr;vzava:"
#define kStrForMoreInfo "Za vi;vse informacija, vidite:"
#define kStrLicense "^p je distribuiran pod nalozima GNU javne licence, verzije 2."
#define kStrDisclaimer " ^p je distribuiran u nadi da ;ece biti koristan, ali BeZ GaRaNcIjE.;ls  ;vcak i BeZ UtR;vzIvOsTi ili PrIkLaDnOsTi Za OdRe;dDeNu SvRhU."

/* Help Screen */
#define kStrHowToLeaveControl "da biste iza;vsli iz kontrolnog moda, pustite taster ;]^c;}."
#define kStrHowToPickACommand "U drugom slu;vcaju, otkucajte slovo. Dostupne komande su:"
#define kStrCmdAbout "O programu (informacije o verziji)"
#define kStrCmdOpenDiskImage "Otvori diskovnu datoteku;ll"
#define kStrCmdQuit "Iza;ddi"
#define kStrCmdSpeedControl "Kontrola brzine;ll (^s)"
#define kStrCmdMagnifyToggle "Prekida;vc lupe (^g)"
#define kStrCmdFullScrnToggle "Prekida;vc re;vzima celog ekrana (^f)"
#define kStrCmdCtrlKeyToggle "Prekida;vc emulisanog tastera ;]^m;} (^k)"
#define kStrCmdReset "Resetuj"
#define kStrCmdInterrupt "Prekid"
#define kStrCmdHelp "Pomo;ec (poka;vzi ovu stranu)"

/* Speed Control Screen */
#define kStrCurrentSpeed "Trenutna brzina: ^s"
#define kStrSpeedAllOut "Potpuna"
#define kStrSpeedStopped "Prekida;vc zaustavljanja (^h)"
#define kStrSpeedBackToggle "Prekida;vc rada u pozadini (^b)"
#define kStrSpeedAutoSlowToggle "Prekida;vc automatskog usporavanja emulacije (^l)" /*Remove the "emulacije" if not enough space to display*/
#define kStrSpeedExit "Iza;ddi iz kontrole brzine"

#define kStrNewSpeed "Brzina: ^s"
#define kStrSpeedValueAllOut kStrSpeedAllOut

#define kStrNewStopped "Zaustavljeno jest ^h."
#define kStrNewRunInBack "Pusti u pozadini jest ^b."
#define kStrNewAutoSlow "automatsko usporavanje jest ^l."

#define kStrNewMagnify "Pribli;vzi je ^g."

#define kStrNewFullScreen "Ceo ekran je ^f."

#define kStrNewCntrlKey "Emulisani taster ;]^m;} je ^k."

#define kStrCmdCancel "odbij"

#define kStrConfirmReset "Da li ste sigurni da ;vzelite resetovati emulirani kompjuter? nesa;vcuvane izmene ;ece biti izgubljene, i postoji rizik o;vste;ecivanja diskovnih datoteka. Otkucaj slovo:"
#define kStrResetDo "resetuj"
#define kStrResetNo kStrCmdCancel

#define kStrHaveReset "Emulisani kompjuter resetovan"

#define kStrCancelledReset "Resetovanje odbijeno"

#define kStrConfirmInterrupt "Da li ste sigurni da ;vzelite prekinuti emulisani ra;vcunar? Ovo ;ece pokrenuti rad bilo kog instaliranog debagera. Otkucajte slovo:"
#define kStrInterruptDo "prekini"
#define kStrInterruptNo kStrCmdCancel

#define kStrHaveInterrupted "Emulisani kompjuter prekinut"

#define kStrCancelledInterrupt "Prekid odbijen"

#define kStrConfirmQuit "Da li ste sigurni da ;vzelite iza;eci ^p? Trebali biste ugasiti emulisani ra;vcunar pre izlaska da biste preventisali korupciju montiranih diskovnih fajlova. Otkucajte slovo:"
#define kStrQuitDo "iza;ddi"
#define kStrQuitNo kStrCmdCancel

#define kStrCancelledQuit "Izlazak odbijen"

#define kStrModeConfirmReset "Kontrolni mod : Potvrdi resetovanje"
#define kStrModeConfirmInterrupt "Kontrolni mod : Potvrdi prekid"
#define kStrModeConfirmQuit "Kontrolni mod : Potvrdi izlazak"
#define kStrModeSpeedControl "Kontrolni mod : Kontrola brzine"
#define kStrModeControlBase "Kontrolni mod (Otkucaj ;]H;} za pomo;ec)"
#define kStrModeControlHelp "Kontrolni mod"
/* Same suggestion above for "Kontrolni mod". This may be used in other languages. */
#define kStrModeMessage "Poruka (Otkucaj ;]C;} da nastavi;vs)"

#define kStrMenuFile "Fajl"
#define kStrMenuSpecial "Special"
#define kStrMenuHelp "Pomo;ec"

#define kStrMenuItemAbout "O ^p"
#define kStrMenuItemOpen "Otvori diskovnu datoteku"
#define kStrMenuItemQuit "Iza;ddi"
#define kStrMenuItemMore "Vi;vse komandi"

#define kStrAppMenuItemHide "Sakrij ^p"
#define kStrAppMenuItemHideOthers "Sakrij ostale"
#define kStrAppMenuItemShowAll "Prika;vzi sve"
#define kStrAppMenuItemQuit "Iza;ddi ^p"

#define kStrCmdCopyOptions "Kopiraj variation options"
#define kStrHaveCopiedOptions "Variation options kopirane"
