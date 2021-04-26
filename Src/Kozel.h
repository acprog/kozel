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
	bool	comp;		// ��� ��������?
	UInt8	loss;		//	��������
	Int16	statistics;
};

typedef struct KozelPreferenceType
{
	UInt16	size;					// ������ ���������
	bool		show_labels,		// ���������� ����� ����
				show_prev,			// ���������� ���������� ���
				auto_step;			// ������������� ����������� ������������ �����
	UInt8		current,				// ������� �����
				view_in,				// �� ������ ������ ��������
				n_players,			// ���-�� ������� (3/4)
				primary,				// �����, ����������� ������
				n_turns,				// ���-�� ��������� �����
				ai_skill;			// �������� AI
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


#define OptionsStartNewGame                     1000	// Menu Text: ������ �����            
#define OptionsConfiguration                    1001	// Menu Text: ���������
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
