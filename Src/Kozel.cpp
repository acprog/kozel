/* Kozel
  Copyright (C) 2004
  Author: Alexander Semenov <acmain@gmail.com>
*/
#include <PalmOS.h>

#include "Kozel.h"
#include "KozelRsc.h"

#ifdef CLIE_HR
	UInt16 refNum;
#endif



// ********************************************************************
// Entry Points
// ********************************************************************

// ********************************************************************
// Global variables
// ********************************************************************

// g_prefs
// cache for application preferences during program execution
KozelPreferenceType g_prefs;
bool	os_4_avaiable;
//		os_5_avaiable;

// ********************************************************************
// Internal Constants
// ********************************************************************

// Define the minimum OS version we support
#define ourMinVersion    sysMakeROMVersion(3,5,0,sysROMStageDevelopment,0)
#define kPalmOS10Version sysMakeROMVersion(1,0,0,sysROMStageRelease,0)
#define OS4Version    	 sysMakeROMVersion(4,0,0,sysROMStageDevelopment,0)
#define OS5Version    	 sysMakeROMVersion(5,0,0,sysROMStageDevelopment,0)


// FUNCTION: AppHandleEvent
//
// DESCRIPTION: 
//
// This routine loads form resources and set the event handler for
// the form loaded.
//
// PARAMETERS:
//
// event
//     a pointer to an EventType structure
//
// RETURNED:
//     true if the event was handled and should not be passed
//     to a higher level handler.

static Boolean AppHandleEvent(EventType * eventP)
{
    UInt16 formId;
    FormType * frmP;

    if (eventP->eType == frmLoadEvent)
    {
        // Load the form resource.
        formId = eventP->data.frmLoad.formID;
        frmP = FrmInitForm(formId);
        FrmSetActiveForm(frmP);

        // Set the event handler for the form.  The handler of the 
        // currently active form is called by FrmHandleEvent each 
        // time is receives an event.
        switch (formId)
        {
        case MainForm:
            FrmSetEventHandler(frmP, MainFormHandleEvent);
            break;

        default:
            break;

        }
        return true;
    }

    return false;
}







// FUNCTION: AppEventLoop
//
// DESCRIPTION: This routine is the event loop for the application.

static void AppEventLoop(void)
{
    UInt16 error;
    EventType event;

    do {
        // change timeout if you need periodic nilEvents
        EvtGetEvent(&event, evtWaitForever);

        if (! SysHandleEvent(&event))
        {
            if (! MenuHandleEvent(0, &event, &error))
            {
                if (! AppHandleEvent(&event))
                {
                    FrmDispatchEvent(&event);
                }
            }
        }
    } while (event.eType != appStopEvent);
}






// FUNCTION: AppStart
//
// DESCRIPTION:  Get the current application's preferences.
//
// RETURNED:
//     errNone - if nothing went wrong

static Err AppStart(void)
{	 
    UInt16 prefsSize;

    // Read the saved preferences / saved-state information.
    prefsSize = sizeof(KozelPreferenceType);
    if (PrefGetAppPreferences(
        appFileCreator, appPrefID, &g_prefs, &prefsSize, true) == noPreferenceFound
        || g_prefs.size!=sizeof(KozelPreferenceType))
    {
   		g_prefs.size=sizeof(KozelPreferenceType);
   		// FIXME: setup g_prefs with default values
   		DefaultCfg();
    }

	UInt32	width=160,
			height=160,	 
			depth;
    Err error;
    
#ifdef COLOR_DEPTH_4
	depth = 4;
#endif
#ifdef COLOR_DEPTH_8
	depth = 8;
#endif
#ifdef COLOR_DEPTH_16
	depth = 16;
#endif

#ifdef CLIE_HR
/*
	if (os_5_avaiable)
	{
		// изменить атрибуты bit-мапов
//		BmpCreate
	}
	else*/
	{
		/* High Resolution Mode Set */
		error=SysLibFind(sonySysLibNameHR, &refNum);
		if (!error)
			error=SysLibLoad('libr', sonySysFileCHRLib, &refNum);

		if (error)
		{
			FrmAlert(HiResAlert);
			return error;
		}
		
		HROpen(refNum);
	}
	
	width=height=320;

	error = WinScreenMode(winScreenModeSet, &width, &height, &depth, NULL);
#else
	error = WinScreenMode(winScreenModeSet, NULL, NULL, &depth, NULL);
#endif
	
	if (error)
	{
	#ifdef COLOR_DEPTH_4
		depth = 2;
		error = WinScreenMode(winScreenModeSet, NULL, NULL, &depth, NULL);
	#endif
		if (error)
		{
			FrmAlert(ScreenAlert);
			return error;
		}
	}
/**/
    return errNone;
}







// FUNCTION: AppStop
//
// DESCRIPTION: Save the current state of the application.

static void AppStop(void)
{
    // Write the saved preferences / saved-state information.  This 
    // data will be saved during a HotSync backup.
    PrefSetAppPreferences(
        appFileCreator, appPrefID, appPrefVersionNum, 
        &g_prefs, sizeof(KozelPreferenceType), true);
        
    // Close all the open forms.
    FrmCloseAllForms();

#ifdef CLIE_HR
//	if (!os_5_avaiable)
		HRClose(refNum);
#endif
}







// all code from here to end of file should use no global variables
//#pragma warn_a5_access on

// FUNCTION: RomVersionCompatible
//
// DESCRIPTION: 
//
// This routine checks that a ROM version is meet your minimum 
// requirement.
//
// PARAMETERS:
//
// requiredVersion
//     minimum rom version required
//     (see sysFtrNumROMVersion in SystemMgr.h for format)
//
// launchFlags
//     flags that indicate if the application UI is initialized
//     These flags are one of the parameters to your app's PilotMain
//
// RETURNED:
//     error code or zero if ROM version is compatible

static Err RomVersionCompatible(UInt32 requiredVersion, UInt16 launchFlags)
{
    UInt32 romVersion;

    // See if we're on in minimum required version of the ROM or later.
    FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
    if (romVersion < requiredVersion)
    {
        if ((launchFlags & 
            (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) ==
            (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp))
        {
            // Palm OS 1.0 will continuously relaunch this app unless 
            // we switch to another safe one.
            if (romVersion <= kPalmOS10Version)
            {
                AppLaunchWithCommand(
                    sysFileCDefaultApp, 
                    sysAppLaunchCmdNormalLaunch, NULL);
            }
        }

        return sysErrRomIncompatible;
    }

    return errNone;
}









// FUNCTION: KozelPalmMain
//
// DESCRIPTION: This is the main entry point for the application.
//
// PARAMETERS:
//
// cmd
//     word value specifying the launch code. 
//
// cmdPB
//     pointer to a structure that is associated with the launch code
//
// launchFlags
//     word value providing extra information about the launch
//
// RETURNED:
//     Result of launch, errNone if all went OK

static UInt32 KozelPalmMain(
    UInt16 cmd, 
    MemPtr /*cmdPBP*/, 
    UInt16 launchFlags)
{
    Err error;

    error = RomVersionCompatible (ourMinVersion, launchFlags);
    if (error)
    {
        FrmAlert (RomIncompatibleAlert);
    	return (error);
    }
    
    UInt32 romVersion;
	switch (cmd)
	{
    case sysAppLaunchCmdNormalLaunch:
        FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
    	os_4_avaiable=(romVersion>=OS4Version);
        error = AppStart();
        if (error) 
            return error;

        // start application by opening the main form
        // and then entering the main event loop
        FrmGotoForm(MainForm);
        AppEventLoop();
        
        AppStop();
        break;

    default:
        break;
    }

    return errNone;
}






// FUNCTION: PilotMain
//
// DESCRIPTION: This is the main entry point for the application.
// 
// PARAMETERS:
//
// cmd
//     word value specifying the launch code. 
//
// cmdPB
//     pointer to a structure that is associated with the launch code
//
// launchFlags
//     word value providing extra information about the launch.
//
// RETURNED:
//     Result of launch, errNone if all went OK

UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
	return KozelPalmMain(cmd, cmdPBP, launchFlags);
}


//===============================================================
void draw_bitmap(DmResID id, int x, int y)
{
	MemHandle	BitmapH = DmGetResource(bitmapRsc, id);
	MemPtr		BitmapP = MemHandleLock(BitmapH);
/*
	if (os_5_avaiable)
	{
		struct bmp
		{
			// BitmapType
			Int16  	width;
			Int16  	height;
			UInt16  rowBytes;
			UInt16	flags;					// see BitmapFlagsType
			UInt8	pixelSize;				// bits per pixel
			UInt8	version;					// data structure version 3
	
			// version 3 fields
			UInt8	size;						// size of this structure in bytes (0x16)
			UInt8	pixelFormat;			// format of the pixel data, see pixelFormatType
			UInt8	unused;
			UInt8	compressionType;		// see BitmapCompressionType
			UInt16	density;					// used by the blitter to scale bitmaps
		}*b=(bmp*)BitmapP;
		if (b->density==kDensityLow)
		{
			b->density=kDensityDouble;
			b->width/=2;
			b->height/=2;
		}
	}*/
	WinDrawBitmap((BitmapType*)BitmapP, x, y);
	MemHandleUnlock(BitmapH);
	DmReleaseResource(BitmapH);
}


// turn a5 warning off to prevent it being set off by C++
// static initializer code generation
#pragma warn_a5_access reset