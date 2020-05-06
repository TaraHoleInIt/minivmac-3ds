/*
	STRCNFRE.h

	Copyright (C) 2006 Pierre Lemieux, Ga;uel Coup;ee, Paul C. Pratt

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
	STRing CoNstants for FREnch

	These strings were translated by Pierre Lemieux, and an earlier
	version of them was proofread by Ga;uel Coup;ee.
*/

#define kStrAboutTitle ";`A propos"
#define kStrAboutMessage "Pour afficher des informations sur ce programme, utilisez la commande A du mode commande de ^p. Pour en savoir plus sur le mode commande, s;eelectionnez l;}article Autres commandes;ll du menu Sp;eecial."

#define kStrMoreCommandsTitle "D;}autres commandes sont disponibles dans le mode commande de ^p."
#define kStrMoreCommandsMessage "Pour acc;eeder au mode commande, maintenez la touche ^c enfonc;eee. Vous allez demeurer en mode commande jusqu;};`a ce que vous rel;iachiez la touche ^c. Tapez H en mode commande pour afficher la liste des commandes disponibles."

#define kStrTooManyImagesTitle "Images disques trop nombreuses."
#define kStrTooManyImagesMessage "Je ne peux pas ouvrir un aussi grand nombre d;}images disques. Veuillez en ;eejecter une."

#define kStrImageInUseTitle "Image disque d;eej;`a utilis;eee."
#define kStrImageInUseMessage "Je ne peux pas ouvrir l;}image disque, car elle est d;eej;`a utilis;eee par une autre application ou d;eej;`a ouverte dans ^p."

#define kStrOutOfMemTitle "M;eemoire insuffisante."
#define kStrOutOfMemMessage "Il n;}y a plus assez de m;eemoire disponible pour lancer ^p"

#define kStrNoROMTitle "ROM introuvable."
#define kStrNoROMMessage "Je n;}arrive pas ;`a trouver le fichier image de la ROM intitul;ee ;[^r;{. Pour plus d;}information, voir : ;[^w;{."

#define kStrCorruptedROMTitle "Somme de contr;iole incorrecte."
#define kStrCorruptedROMMessage "Le fichier image de la ROM intitul;ee ;[^r;{ est peut-;ietre endommag;ee."

#define kStrUnsupportedROMTitle "ROM non support;eee."
#define kStrUnsupportedROMMessage "Le chargement du fichier image de la ROM intitul;ee ;[^r;{ a r;eeussi, mais je ne supporte pas cette version de la ROM."

#define kStrQuitWarningTitle "Veuillez ;eeteindre l;}ordinateur virtuel avant de quitter."
#define kStrQuitWarningMessage "Pour forcer ^p ;`a quitter, au risque d;}endommager les images disques ouvertes, tapez Q en mode commande. Pour en savoir plus sur le mode commande de ^p, s;eelectionnez l;}article Autres commandes;ll du menu Sp;eecial."

#define kStrReportAbnormalTitle "Situation anormale."
#define kStrReportAbnormalMessage "L;}ordinateur virtuel tente d;}effectuer une op;eeration qui n;};eetait pas pr;eevue dans le cadre d;}un usage normal."

#define kStrBadArgTitle "Argument inconnu."
#define kStrBadArgMessage "J;}ai ignor;ee l;}un des arguments de la ligne de commande, que je ne comprenais pas."

#define kStrOpenFailTitle "Tentative d;}ouverture infructueuse."
#define kStrOpenFailMessage "Je n;}ai pas r;eeussi ;`a ouvrir l;}image disque."

#define kStrNoReadROMTitle "ROM illisible."
#define kStrNoReadROMMessage "J;}ai bien trouv;ee le fichier image de la ROM intitul;ee ;[^r;{, mais je n;}arrive pas ;`a le lire."

#define kStrShortROMTitle "ROM trop courte."
#define kStrShortROMMessage "Le fichier image de la ROM intitul;ee ;[^r;{ est plus court qu;}il ne le devrait."

/* state of a boolean option */
#define kStrOn "activ;ee"
#define kStrOff "d;eesactiv;ee"

/* state of a key */
#define kStrPressed "enfonc;eee"
#define kStrReleased "rel;iach;eee"

/* state of Stopped */
#define kStrStoppedOn "activ;eee"
#define kStrStoppedOff "d;eesactiv;eee"

/* About Screen */
#define kStrProgramInfo "^v"
#define kStrSponsorIs "^v. This variation is made for:"
#define kStrWorkOfMany "copyright ^y. ^p est le fruit du travail de nombreuses personnes. La maintenance de cette version est assur;eee par :"
#define kStrForMoreInfo "Pour de plus amples informations, voir :"
#define kStrLicense "^p est distribu;ee au titre des clauses de la Licence publique g;een;eerale GNU, version 2."
#define kStrDisclaimer " ^p est distribu;ee dans l;}espoir qu;}il sera utile, mais SANS AUCUNE GARANTIE, sans m;ieme une garantie implicite de COMMERCIABILIT;eE ou DE CONFORMIT;eE ;`A UNE UTILISATION PARTICULI;`ERE."

/* Help Screen */
#define kStrHowToLeaveControl "Pour quitter le mode commande, rel;iachez la touche ^c."
#define kStrHowToPickACommand "Sinon, choisissez l;}une des commandes ci-dessous en tapant la lettre correspondante :"
#define kStrCmdAbout ";`A propos (de cette version du logiciel)"
#define kStrCmdOpenDiskImage "Ouvrir une image disque;ll"
#define kStrCmdQuit "Quitter"
#define kStrCmdSpeedControl "R;eeglage de la vitesse;ll (^s)"
#define kStrCmdMagnifyToggle "Agrandissement (^g)"
#define kStrCmdFullScrnToggle "Plein ;eecran (^f)"
#define kStrCmdCtrlKeyToggle "Touche ^m virtuelle (^k)"
#define kStrCmdReset "R;eeinitialisation"
#define kStrCmdInterrupt "Interruption"
#define kStrCmdHelp "Aide (affiche cette page)"

/* Speed Control Screen */
#define kStrCurrentSpeed "Vitesse courante : ^s"
#define kStrSpeedAllOut "Vitesse maximale"
#define kStrSpeedStopped "Pause (^h)"
#define kStrSpeedBackToggle "Fonctionnement en arri;`ere-plan (^b)"
#define kStrSpeedAutoSlowToggle "AutoSlow (^l)"
#define kStrSpeedExit "Quitter cette page"

#define kStrNewSpeed "Vitesse : ^s"
#define kStrSpeedValueAllOut "maximale"

#define kStrNewStopped "Pause ^h."
#define kStrNewRunInBack "Fonctionnement en arri;`ere-plan ^b."
#define kStrNewAutoSlow "AutoSlow ^l."

#define kStrNewMagnify "Agrandissement ^g."

#define kStrNewFullScreen "Mode plein ;eecran ^f."

#define kStrNewCntrlKey "Touche ^m virtuelle ^k."

#define kStrCmdCancel "Annuler"

#define kStrConfirmReset "Souhaitez-vous vraiment r;eeinitialiser l;}ordinateur virtuel ? Vous allez perdre les modifications non enregistr;eees et vous risquez d;}endommager les images disques ouvertes."
#define kStrResetDo "R;eeinitialiser"
#define kStrResetNo kStrCmdCancel

#define kStrHaveReset "Vous avez r;eeinitialis;ee l;}ordinateur virtuel."

#define kStrCancelledReset "R;eeinitialisation annul;eee."

#define kStrConfirmInterrupt "Souhaitez-vous vraiment interrompre l;}ordinateur virtuel ? Cela va d;eeclencher le d;eebogueur (s;}il y en a un d;}install;ee)."
#define kStrInterruptDo "Interrompre"
#define kStrInterruptNo kStrCmdCancel

#define kStrHaveInterrupted "Vous avez interrompu l;}ordinateur virtuel."

#define kStrCancelledInterrupt "Interruption annul;eee."

#define kStrConfirmQuit "Souhaitez-vous vraiment quitter ^p ? Vous devriez ;eeteindre l;}ordinateur virtuel avant de quitter, pour ;eeviter d;}endommager les images disques ouvertes."
#define kStrQuitDo kStrCmdQuit
#define kStrQuitNo kStrCmdCancel

#define kStrCancelledQuit "Commande Quitter annul;eee."

#define kStrModeConfirmReset "Mode commande : confirmation de la r;eeinitialisation"
#define kStrModeConfirmInterrupt "Mode commande : confirmation de l;}interruption"
#define kStrModeConfirmQuit "Mode commande : confirmation de la commande Quitter"
#define kStrModeSpeedControl "Mode commande : r;eeglage de la vitesse"
#define kStrModeControlBase "Mode commande (Tapez H pour afficher l;}aide.)"
#define kStrModeControlHelp "Mode commande"
#define kStrModeMessage "Message (Tapez C pour continuer.)"

#define kStrMenuFile "Fichier"
#define kStrMenuSpecial "Sp;eecial"
#define kStrMenuHelp "Aide"

#define kStrMenuItemAbout ";`A propos de ^p"
#define kStrMenuItemOpen "Ouvrir une image disque"
#define kStrMenuItemQuit "Quitter"
#define kStrMenuItemMore "Autres commandes"

#define kStrAppMenuItemHide "Masquer ^p"
#define kStrAppMenuItemHideOthers "Masquer les autres"
#define kStrAppMenuItemShowAll "Tout afficher"
#define kStrAppMenuItemQuit "Quitter ^p"

#define kStrCntrlKyName "Contr;iole"
#define kStrCmdCopyOptions "Copier variation options"
#define kStrHaveCopiedOptions "Variation options copi;eees"
