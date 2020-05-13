/*
	Configuration options used by platform specific code.

	This file is automatically generated by the build system,
	which tries to know what options are valid in what
	combinations. Avoid changing this file manually unless
	you know what you're doing.
*/

#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <fcntl.h>
#include <unistd.h>


#define RomFileName "MacII.ROM"
#define kCheckSumRom_Size 0x00040000
#define kRomCheckSum1 0x9779D2C4
#define kRomCheckSum2 0x97221136
#define RomStartCheckSum 1
#define EnableDragDrop 0
#define SaveDialogEnable 1
#define EnableAltKeysMode 0
#define MKC_formac_Control MKC_CM
#define MKC_formac_Command MKC_Command
#define MKC_formac_Option MKC_Option
#define MKC_formac_Shift MKC_Shift
#define MKC_formac_CapsLock MKC_CapsLock
#define MKC_formac_Escape MKC_Escape
#define MKC_formac_BackSlash MKC_BackSlash
#define MKC_formac_Slash MKC_Slash
#define MKC_formac_Grave MKC_Grave
#define MKC_formac_Enter MKC_Enter
#define MKC_formac_PageUp MKC_PageUp
#define MKC_formac_PageDown MKC_PageDown
#define MKC_formac_Home MKC_Home
#define MKC_formac_End MKC_End
#define MKC_formac_Help MKC_Help
#define MKC_formac_ForwardDel MKC_ForwardDel
#define MKC_formac_F1 MKC_Option
#define MKC_formac_F2 MKC_Command
#define MKC_formac_F3 MKC_F3
#define MKC_formac_F4 MKC_F4
#define MKC_formac_F5 MKC_F5
#define MKC_formac_RControl MKC_CM
#define MKC_formac_RCommand MKC_Command
#define MKC_formac_ROption MKC_Option
#define MKC_formac_RShift MKC_Shift
#define MKC_UnMappedKey  MKC_Control
#define VarFullScreen 1
#define WantInitFullScreen 0
#define MayFullScreen 1
#define MayNotFullScreen 1
#define WantInitMagnify 0
#define EnableMagnify 1
#define MyWindowScale 2
#define WantInitRunInBackground 0
#define WantInitNotAutoSlow 0
#define WantInitSpeedValue 2
#define WantEnblCtrlInt 1
#define WantEnblCtrlRst 1
#define WantEnblCtrlKtg 1
#define NeedRequestInsertDisk 0
#define NeedDoMoreCommandsMsg 0
#define NeedDoAboutMsg 0
#define UseControlKeys 1
#define UseActvCode 0
#define EnableDemoMsg 0

/* version and other info to display to user */

#define NeedIntlChars 0
#define kStrAppName "Mini vMac"
#define kAppVariationStr "minivmac-3ds"
#define kStrCopyrightYear "2020"
#define kMaintainerName "Tara Keeling"
#define kStrHomePage "https://github.com/TaraHoleInIt"

#define kBldOpts "-br 37 -t 3dsa -m II -hres 512 -vres 384 -depth 1 -sound 1 -ta 0"
