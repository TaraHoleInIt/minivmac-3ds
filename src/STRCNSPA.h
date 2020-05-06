/*
	STRCNSPA.h

	Copyright (C) 2006 Chir, Paul C. Pratt

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
	STRing CoNstants for SPAnish

	These strings were translated by Chir.
*/

#define kStrAboutTitle "Acerca de"
#define kStrAboutMessage "Para mostrar informaci;eon acerca de este programa, use el comando ;]A;} del ^p Modo Control. Para saber m;eas acerca del Modo Control, vea ;[Mas Comandos;ll;{ en el menu ;[Especial;{ ."

#define kStrMoreCommandsTitle "M;eas comandos est;ean disponibles en el modo de control de ^p ."
#define kStrMoreCommandsMessage "Para entrar en el modo de control, mantenga pulsada la tecla ;]^c;} . Seguir;ea en el modo control hasta que suelte la tecla ;]^c;} . Pulse ;]H;} en el modo control para listar los comandos disponibles."

#define kStrTooManyImagesTitle "Demasiadas im;eagenes de disco"
#define kStrTooManyImagesMessage "No se pueden montar tantas im;eagenes de disco. Intente sacar alguna."

#define kStrImageInUseTitle "Imagen disco en uso"
#define kStrImageInUseMessage "No se puede montar la imagen disco porque ya est;ea en uso por otra aplicaci;eon, o ya abierta en ^p."

#define kStrOutOfMemTitle "Memoria insuficiente"
#define kStrOutOfMemMessage "No hay suficiente memoria disponible para lanzar ^p."

#define kStrNoROMTitle "Imposible localizar la imagen ROM"
#define kStrNoROMMessage "No se encuentra la imagen del fichero ROM ;[^r;{. Para m;eas informaci;eon, ver: ;[^w;{."

#define kStrCorruptedROMTitle "El control checksum de la ROM ha fracasado"
#define kStrCorruptedROMMessage "El fichero de imagen ROM ;[^r;{ puede estar corrompido."

#define kStrUnsupportedROMTitle "ROM no soportada"
#define kStrUnsupportedROMMessage "Fichero de imagen ROM ;[^r;{ cargado con ;eexito, pero esa versi;eon no est;ea soportada."

#define kStrQuitWarningTitle "Por favor, apague el ordenador emulado antes de salir."
#define kStrQuitWarningMessage "Para forzar ^p salir, con riesgo de corrupci;eon en las im;eagenes de discos montadas, use el comando ;]Q;} del ^p Modo Control. Para saber m;eas acerca del Modo Control, vea ;[Mas Comandos;ll;{ en el menu ;[Especial;{ ."

#define kStrReportAbnormalTitle "Situaci;eon anormal"
#define kStrReportAbnormalMessage "El ordenador emulado intenta hacer una operaci;eon no esperada en un uso normal."

#define kStrBadArgTitle "Argumento desconocido"
#define kStrBadArgMessage "No se comprendi;eo alguno de los argumentos de la linea de comandos, y se ignor;eo."

#define kStrOpenFailTitle "Apertura fracasada"
#define kStrOpenFailMessage "No se pudo abrir la imagen disco."

#define kStrNoReadROMTitle "No se puede abrir la imagen ROM"
#define kStrNoReadROMMessage "Se encontr;eo el fichero de imagen ROM ;[^r;{, pero no se pudo leer."

#define kStrShortROMTitle "Imagen ROM demasiado corta"
#define kStrShortROMMessage "El fichero de imagen ROM ;[^r;{ es m;eas corto de lo que deber;eia."

/* state of a boolean option */
#define kStrOn "activado"
#define kStrOff "desactivado"

/* state of a key */
#define kStrPressed "pulsada"
#define kStrReleased "soltada"

/* state of Stopped */
#define kStrStoppedOn kStrOn
#define kStrStoppedOff kStrOff

/* About Screen */
#define kStrProgramInfo "^v"
#define kStrSponsorIs "^v. This variation is made for:"
#define kStrWorkOfMany "Copyright ^y. ^p contiene el trabajo de muchas personas. Esta versi;eon es mantenida por:"
#define kStrForMoreInfo "Para m;eas informaci;eon, vea:"
#define kStrLicense "^p es distribuido bajo los t;eerminos de la licencia p;eublica GNU, versi;eon 2."
#define kStrDisclaimer " ^p es distribuido con la esperanza de ser ;eutil, pero SIN NINGUNA GARANTIA;ls incluso sin la garant;eia impl;eicita de MERCANTIBILIDAD O ADECUACION PARA UN USO PARTICULAR."

/* Help Screen */
#define kStrHowToLeaveControl "Para salir del Modo Control, suelte la tecla ;]^c;} ."
#define kStrHowToPickACommand "Sin;eo, pulse una letra. Los comandos disponibles son:"
#define kStrCmdAbout "Acerca de (informaci;eon sobre versi;eon)"
#define kStrCmdOpenDiskImage "Abrir imagen disco;ll"
#define kStrCmdQuit "Salir"
#define kStrCmdSpeedControl "Control de velocidad;ll (^s)"
#define kStrCmdMagnifyToggle "Magnificaci;eon (^g)"
#define kStrCmdFullScrnToggle "Pantalla completa (^f)"
#define kStrCmdCtrlKeyToggle "Toggle emulaci;eon tecla ;]^m;} (^k)"
#define kStrCmdReset "Reset"
#define kStrCmdInterrupt "Interrumpir"
#define kStrCmdHelp "Ayuda (mostrar esta p;eagina)"

/* Speed Control Screen */
#define kStrCurrentSpeed "Velocidad actual: ^s"
#define kStrSpeedAllOut "M;eaxima velocidad"
#define kStrSpeedStopped "Stop (^h)"
#define kStrSpeedBackToggle "Ejecutar en segundo plano (^b)"
#define kStrSpeedAutoSlowToggle "AutoSlow (^l)"
#define kStrSpeedExit "Salir del control de velocidad"

#define kStrNewSpeed "Velocidad: ^s"
#define kStrSpeedValueAllOut kStrSpeedAllOut

#define kStrNewStopped "Stop est;ea ^h."
#define kStrNewRunInBack "Ejecutar en segundo plano est;ea ^b."
#define kStrNewAutoSlow "AutoSlow est;ea ^l."

#define kStrNewMagnify "Magnificaci;eon est;ea ^g."

#define kStrNewFullScreen "Pantalla completa est;ea ^f."

#define kStrNewCntrlKey "Emulado tecla ;]^m;} ^k."

#define kStrCmdCancel "cancelar"

#define kStrConfirmReset "Est;ea seguro de desear reiniciar el ordenador emulado? Los cambios no salvados se perderan, y existe riesgo de corrupci;eon en las im;eagenes de discos montadas. Pulse una tecla:"
#define kStrResetDo "reset"
#define kStrResetNo kStrCmdCancel

#define kStrHaveReset "Se ha reiniciado el ordenador emulado"

#define kStrCancelledReset "Reset cancelado"

#define kStrConfirmInterrupt "Est;ea seguro de desear interrumpir el ordenador emulado? Esto invocar;ea cualquier debugger instalado. Pulse una tecla:"
#define kStrInterruptDo "interrumpir"
#define kStrInterruptNo kStrCmdCancel

#define kStrHaveInterrupted "Se ha interrumpido el ordenador emulado"

#define kStrCancelledInterrupt "Interrupci;eon cancelada"

#define kStrConfirmQuit "Est;ea seguro de desear salir de ^p? Deber;eia apagar el ordenador emulado antes de salir, para evitar corrupciones en las im;eagenes de discos montadas. Pulse una tecla:"
#define kStrQuitDo "salir"
#define kStrQuitNo kStrCmdCancel

#define kStrCancelledQuit "Salida cancelada"

#define kStrModeConfirmReset "Modo Control : Confirmar reset"
#define kStrModeConfirmInterrupt "Modo Control : Confirmar Interrupci;eon"
#define kStrModeConfirmQuit "Modo Control : Confirmar salida"
#define kStrModeSpeedControl "Modo Control : Control de velocidad"
#define kStrModeControlBase "Modo Control (Pulse ;]H;} para la ayuda)"
#define kStrModeControlHelp "Modo Control"
#define kStrModeMessage "Mensaje (Pulse ;]C;} para continuar)"

#define kStrMenuFile "Fichero"
#define kStrMenuSpecial "Especial"
#define kStrMenuHelp "Ayuda"

#define kStrMenuItemAbout "Acerca de ^p"
#define kStrMenuItemOpen "Abrir imagen disco"
#define kStrMenuItemQuit "Salir"
#define kStrMenuItemMore "M;eas comandos"

#define kStrAppMenuItemHide "Ocultar ^p"
#define kStrAppMenuItemHideOthers "Ocultar otros"
#define kStrAppMenuItemShowAll "Mostrar todo"
#define kStrAppMenuItemQuit "Salir de ^p"

#define kStrCmdCopyOptions "Copiar variation options"
#define kStrHaveCopiedOptions "Variation options copiadas"
