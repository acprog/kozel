/* Kozel
  Copyright (C) 2004
  Author: Alexander Semenov <acmain@gmail.com>
*/
#ifndef _CFG_H_
#define _CFG_H_

#include "cardsRsc.h"

//=============================================================
// поддержка высокого разрешения от Sony
#ifdef CLIE_HR
#include "SonyCLIE.h"

extern UInt16 refNum;

#define	WinScreenMode(operation, widthP, heightP, depthP, enableColorP)\
	HRWinScreenMode(refNum, operation, widthP, heightP, depthP, enableColorP)

#define WinDrawBitmap(bitmapP, x, y)\
	HRWinDrawBitmap(refNum, bitmapP, (x)<<1, (y)<<1)
/*
extern	bool	os_5_avaiable;

#define	WinScreenMode(operation, widthP, heightP, depthP, enableColorP)\
	(os_5_avaiable ?\
		WinScreenMode(operation, widthP, heightP, depthP, enableColorP)\
	:\
		HRWinScreenMode(refNum, operation, widthP, heightP, depthP, enableColorP))

#define WinDrawBitmap(bitmapP, x, y)\
	(os_5_avaiable ?\
		WinDrawBitmap(bitmapP, (x)<<1, (y)<<1)\
	:\
		HRWinDrawBitmap(refNum, bitmapP, (x)<<1, (y)<<1))
/**/
#endif

#endif	// _CFG_H_