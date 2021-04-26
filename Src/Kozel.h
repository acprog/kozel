/* Kozel
  Copyright (C) 2004
  Author: Alexander Semenov <acmain@gmail.com>
*/

#ifndef KOZEL_H_
#define KOZEL_H_

#include "cfg.h"

// ********************************************************************
// Internal Structures
// ********************************************************************
struct player_prefs
{
	char	name[21];
	bool	comp;		// это компьтер?
	UInt8	loss;		//	проигрыш
	Int16	statistics;
};

typedef struct KozelPreferenceType
{
	UInt16	size;					// размер структуры
	bool		show_labels,		// отображать метки карт
				show_prev,			// показывать предыдущий ход
				auto_step;			// автоматически выкладывать единственную карту
	UInt8		current,				// текущий игрок
				view_in,				// от какого игрока смотреть
				n_players,			// кол-во игроков (3/4)
				primary,				// игрок, назначавший козырь
				n_turns,				// кол-во прошедших конов
				ai_skill;			// крутость AI
	player_prefs	players[4];
} KozelPreferenceType;


// ********************************************************************
//	Common Funcs
// ********************************************************************
Boolean MainFormHandleEvent(EventType * eventP);
void DefaultCfg();
void statistics_form();
void draw_bitmap(DmResID id, int x, int y);

// ********************************************************************
// Global variables
// ********************************************************************

extern KozelPreferenceType g_prefs;
extern	bool	os_4_avaiable;

// ********************************************************************
// Internal Constants
// ********************************************************************

#define appFileCreator			'ACkz'
#define appName					"Kozel"
#define appVersionNum			0x01
#define appPrefID				0x00
#define appPrefVersionNum		0x01


#define	SCREEN_WIDTH	160
#define	SCREEN_HEIGHT	160


#define OptionsStartNewGame                     1000	// Menu Text: Ќачать новую            
#define OptionsConfiguration                    1001	// Menu Text: Ќастройка
#define OptionsStatistics                       1002

#define HelpMenu                                  1100
#define HelpRules                                 1100
#define HelpThatIsee                              1101
#define HelpAbout                                 1102


// ********************************************************************
// Helper template functions
// ********************************************************************
/*
// use this template like:
// ControlType *button; GetObjectPtr(button, MainOKButton);

template <class T>
void GetObjectPtr(typename T * &ptr, UInt16 id)
{
	FormType * frmP;

	frmP = FrmGetActiveForm();
	ptr = static_cast<T *>(
		FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, id)));
}
/**/
// use this template like:
// ControlType *button = 
//     GetObjectPtr<ControlType>(MainOKButton);

template <class T>
typename T * GetObjectPtr(UInt16 id, FormType *frmP=NULL)
{
	if (!frmP)
		frmP = FrmGetActiveForm();
	return static_cast<T *>(
		FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, id)));
}

#endif // KOZEL_H_
