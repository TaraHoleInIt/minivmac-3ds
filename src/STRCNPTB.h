/*
	STRCNPTB.h

	Copyright (C) 2017 Mauricio, Paul C. Pratt

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
	STRing CoNstants for PorTuguese-Brazilian

	These strings were translated by Mauricio.
*/

#define kStrAboutTitle "Sobre"

#define kStrAboutMessage "Para exibir informa;c;noes sobre este programa, use o comando ;]A;} do Modo de Controle ^p. Para saber mais sobre o Modo de Controle, consulte o item ;[Mais Comandos;ll;{ no menu ;[Especial;{."

#define kStrMoreCommandsTitle "Mais comandos est;nao dispon;eiveis no Modo de Controle ^p."

#define kStrMoreCommandsMessage "Para entrar no Modo de Controle, pressione e mantenha pressionada a tecla ;]^c;}, voc;ie permanecer;ea no Modo de Controle at;ee que voc;ie solte a tecla ;]^c;}. Digite 'H' no Modo de Controle para listar os comandos dispon;eiveis."

#define kStrTooManyImagesTitle "Imagens de Disco excedidas"

#define kStrTooManyImagesMessage "N;nao consigo montar tantas Imagens de Disco. Tente ejetar alguma."

#define kStrImageInUseTitle "Imagem de Disco em uso"

#define kStrImageInUseMessage "N;nao consigo montar a imagem de disco porque ela j;ea est;ea em uso por outro aplicativo ou j;ea est;ea aberta em ^p."

#define kStrOutOfMemTitle "Mem;eoria insuficiente"

#define kStrOutOfMemMessage "N;nao h;ea mem;eoria suficiente dispon;eivel para iniciar ^p."

#define kStrNoROMTitle "N;nao ;ee poss;eivel localizar a imagem ROM"

#define kStrNoROMMessage "N;nao consigo encontrar o arquivo de imagem ROM ;[^r;{. Para obter mais informa;c;noes, consulte: ;[^w;{."

#define kStrCorruptedROMTitle "Falha no checksum da ROM"

#define kStrCorruptedROMMessage "O arquivo de imagem ROM ;[^r;{ pode estar corrompido."

#define kStrUnsupportedROMTitle "ROM n;nao suportada"

#define kStrUnsupportedROMMessage "O arquivo de imagem ROM ;[^r;{ foi carregado com ;iexito, mas n;nao suportamos esta vers;nao ROM."

#define kStrQuitWarningTitle "Por favor, desligue o computador emulado antes de fechar."

#define kStrQuitWarningMessage "To force ^p to quit, at the risk of corrupting the mounted disk image files, use the ;]Q;} command of the ^p Control Mode. To learn about the Control Mode, see the ;[More Commands;ll;{ item in the ;[Special;{ menu."

#define kStrReportAbnormalTitle "Situa;c;nao Anormal"

#define kStrReportAbnormalMessage "O computador emulado est;ea tentando uma opera;c;nao que n;nao era esperada para acontecer em uso normal."

#define kStrBadArgTitle "Argumento desconhecido"

#define kStrBadArgMessage "Eu n;nao entendi um dos argumentos da linha de comando e ignorei-o."

#define kStrOpenFailTitle "Falha ao abrir"

#define kStrOpenFailMessage "N;nao consegui abrir a imagem do disco."

#define kStrNoReadROMTitle "N;nao ;ee poss;eivel ler a imagem ROM"

#define kStrNoReadROMMessage "Eu encontrei o arquivo de imagem ROM ;[^r;{, mas n;nao consigo l;ie-lo."

#define kStrShortROMTitle "Imagem da ROM muito pequena"

#define kStrShortROMMessage "O arquivo de imagem ROM ;[^r;{ ;ee menor do que deveria ser."

/* state of a boolean option */
#define kStrOn "Ligado"
#define kStrOff "Desligado"

/* state of a key */
#define kStrPressed "Ligada"
#define kStrReleased "Desligada"

/* state of Stopped */
#define kStrStoppedOn kStrOn
#define kStrStoppedOff kStrOff

/* About Screen */
#define kStrProgramInfo "^v"

#define kStrSponsorIs "^v. Esta varia;c;nao ;ee patrocinada por:"

#define kStrWorkOfMany "Copyright ^y. ^p cont;eem o trabalho de muitas pessoas. Esta vers;nao ;ee mantida por:"

#define kStrForMoreInfo "Para obter mais informa;c;noes, consulte:"

#define kStrLicense "^p ;ee distribu;eido sob os termos da Licen;ca P;eublica GNU, vers;nao 2."

#define kStrDisclaimer " ^p ;ee distribu;eido com a esperan;ca de que ser;ea ;eutil, mas SEM NENHUMA GARANTIA, sem sequer a garantia impl;eicita de COMERCIALIZA;C;nAO ou ADEQUA;C;nAO A UM DETERMINADO PROP;eOSITO."

/* Help Screen */
#define kStrHowToLeaveControl "Para sair do modo de controle, solte a tecla ;]^c;}."

#define kStrHowToPickACommand "Caso contr;eario, digite uma letra. Comandos dispon;eiveis:"

#define kStrCmdAbout "Sobre (informa;c;noes sobre a vers;nao)"

#define kStrCmdOpenDiskImage "Abrir imagem de disco;ll"

#define kStrCmdQuit "Sair"

#define kStrCmdSpeedControl "Controle de velocidade;ll (^s)"

#define kStrCmdMagnifyToggle "Zoom (^g)"

#define kStrCmdFullScrnToggle "Tela Cheia (^f)"

#define kStrCmdCtrlKeyToggle "Alternar Tecla ;]^m;} emulada (^k)"

#define kStrCmdReset "Reiniciar"

#define kStrCmdInterrupt "Interromper"

#define kStrCmdHelp "Ajuda (mostrar esta p;eagina)"

/* Speed Control Screen */
#define kStrCurrentSpeed "Velocidade atual: ^s"

#define kStrSpeedAllOut "Toda a velocidade"

#define kStrSpeedStopped "parado (^h)"

#define kStrSpeedBackToggle "Rodar em Segundo Plano (^b)"

#define kStrSpeedAutoSlowToggle "Lentid;nao Autom;eatica (^l)"

#define kStrSpeedExit "Sair do controle de velocidade"

#define kStrNewSpeed "Velocidade: ^s"

#define kStrSpeedValueAllOut kStrSpeedAllOut

#define kStrNewStopped "Parado est;ea ^h."

#define kStrNewRunInBack "Rodar em segundo plano est;ea ^b."

#define kStrNewAutoSlow "Lentid;nao Autom;eatica est;ea ^l."

#define kStrNewMagnify "Zoom est;ea ^g."

#define kStrNewFullScreen "Tela Cheia est;ea ^f."

#define kStrNewCntrlKey "Emula;c;nao da tecla ;]^m;} ^k."

#define kStrCmdCancel "cancelar"

#define kStrConfirmReset "Tem certeza de que deseja reiniciar o computador emulado? As altera;c;noes n;nao salvas ser;nao perdidas e h;ea o risco de corromper os arquivos de imagem de disco montados. Digite uma letra:"

#define kStrResetDo "reiniciar"

#define kStrResetNo kStrCmdCancel

#define kStrHaveReset "Reiniciar o computador emulado"

#define kStrCancelledReset "Reinicializa;c;nao cancelada"

#define kStrConfirmInterrupt "Tem certeza de que deseja interromper o computador emulado? Isso invocar;ea qualquer depurador instalado. Digite uma letra:"

#define kStrInterruptDo "Interromper"

#define kStrInterruptNo kStrCmdCancel

#define kStrHaveInterrupted "Interrompeu o computador emulado"

#define kStrCancelledInterrupt "Interrup;c;nao cancelada"

#define kStrConfirmQuit "Tem certeza de que deseja sair ^p? Voc;ie deve desligar o computador emulado antes de sair para evitar corromper os arquivos de imagem de disco montados. Digite uma letra:"

#define kStrQuitDo "sair"

#define kStrQuitNo kStrCmdCancel

#define kStrCancelledQuit "Opera;c;nao cancelada"

#define kStrModeConfirmReset "Modo de Controle : Confirmar Reinicializa;c;nao"
#define kStrModeConfirmInterrupt "Modo de Controle : Confirmar Interrup;c;nao"
#define kStrModeConfirmQuit "Modo de Controle : Confirmar Sair"
#define kStrModeSpeedControl "Modo de Controle : Controle de Velocidade"
#define kStrModeControlBase "Modo de Controle (Digite ;]H;} para obter ajuda)"
#define kStrModeControlHelp "Modo de Controle"
#define kStrModeMessage "Message (Digite ;]C;} para continuar)"

#define kStrMenuFile "Arquivo"
#define kStrMenuSpecial "Especial"
#define kStrMenuHelp "Ajuda"

#define kStrMenuItemAbout "Sobre ^p"
#define kStrMenuItemOpen "Open Disk Image"
#define kStrMenuItemQuit "Sair"
#define kStrMenuItemMore "Mais Comandos"

#define kStrAppMenuItemHide "Esconder ^p"
#define kStrAppMenuItemHideOthers "Esconder Outras"
#define kStrAppMenuItemShowAll "Mostrar Todas"
#define kStrAppMenuItemQuit "Sair ^p"

#define kStrCmdCopyOptions "Copiar variation options"
#define kStrHaveCopiedOptions "Variation options copiadas"
