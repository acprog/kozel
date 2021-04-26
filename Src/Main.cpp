/* Kozel
  Copyright (C) 2004
  Author: Alexander Semenov <acmain@gmail.com>
*/
#include <PalmOS.h>

#include "Kozel.h"
#include "KozelRsc.h"

#include "Player.h"


// ********************************************************************
// variables
// ********************************************************************
player	players[4];

// ********************************************************************
// Internal Constants
// ********************************************************************


//=============================================================================
void statistics_form()
{
	player_prefs	*p[4];
	int	i, j;
	for (i=0; i<4; i++)
		p[i]=g_prefs.players+i;

	// сортируем
	for (i=0; i<2; i++)
		for (j=i+1; j<3; j++)
			if (p[i]->statistics<p[j]->statistics)
				swap(p[i], p[j]);

	FormType *frmP = FrmInitForm (StatisticsForm);
   char	str[20];
   for (i=0; i<4; i++)
   {
	   FrmCopyLabel(frmP, StatisticsFirstNameLabel+i, p[i]->name);
		FrmCopyLabel(frmP, StatisticsFirstScoreLabel+i, StrIToA(str, p[i]->statistics));
   }
   
   if (FrmDoDialog(frmP)==StatisticsResetButton)
		for (int i=0; i<4; i++)
			g_prefs.players[i].statistics=0;
			
   FrmDeleteForm(frmP);
}


// ********************************************************************
//		Создание настроек по умолчанию
// ********************************************************************
void DefaultCfg()
{
	g_prefs.show_labels=true;
	g_prefs.show_prev=false;
	g_prefs.auto_step=false;
	g_prefs.ai_skill=1;
	N_PLAYERS=3;
	char	*names[]={"Игрок", "Трус", "Балбес", "Бывалый"};
	player_prefs	*p=g_prefs.players;
	for (int i=0; i<4; i++, p++)
	{
		StrCopy(p->name, names[i]);
		p->comp=true;
		p->statistics=0;
		p->loss=0;
	}
	g_prefs.players[0].comp=false;
}


// FUNCTION: MainFormInit
//
// DESCRIPTION: This routine initializes the MainForm form.
//
// PARAMETERS:
//
// frm
//     pointer to the MainForm form.
static void MainFormInit(FormType * /*frmP*/)
{	
	players[0].init(g_prefs.players, players+1, players+2);
	players[1].init(g_prefs.players+1, players+2, players);
	players[2].init(g_prefs.players+2, players, players+1);
	
	player::new_round();
}


//=============================================================================
static	void config_form()
{
   FormType *frmP = FrmInitForm (ConfigForm);

	FieldType	*fldP;
	for (int i=0; i<4; i++)
	{
		fldP=GetObjectPtr<FieldType>(ConfigName0Field+i, frmP);
		FldDelete(fldP, 0, FldGetMaxChars(fldP));
		FldInsert(fldP, g_prefs.players[i].name, StrLen(g_prefs.players[i].name));
	}
		
  	CtlSetValue(GetObjectPtr<ControlType>(ConfigShowLabelsCheckbox, frmP), g_prefs.show_labels);
  	CtlSetValue(GetObjectPtr<ControlType>(ConfigAutoStepCheckbox, frmP), g_prefs.auto_step);
	FrmSetControlGroupSelection(frmP, 1, N_PLAYERS==3 ? ConfigPlayers3PushButton : ConfigPlayers4PushButton);
	FrmSetControlGroupSelection(frmP, 2, ConfigAiBadPushButton+g_prefs.ai_skill);

	for (int i=0; i<3; i++)
     	CtlSetValue(GetObjectPtr<ControlType>(ConfigPalm1Checkbox+i, frmP), g_prefs.players[i+1].comp);
  	if (FrmDoDialog (frmP)==ConfigDoneButton)
  	{
		g_prefs.ai_skill=FrmGetControlGroupSelection(frmP, 2)-17;

  		g_prefs.show_labels = CtlGetValue(GetObjectPtr<ControlType>(ConfigShowLabelsCheckbox, frmP));
  		g_prefs.auto_step = CtlGetValue(GetObjectPtr<ControlType>(ConfigAutoStepCheckbox, frmP));
		for (int i=0; i<3; i++)
			g_prefs.players[i+1].comp=CtlGetValue(GetObjectPtr<ControlType>(ConfigPalm1Checkbox+i, frmP));
		for (int i=0; i<4; i++)
			StrCopy(g_prefs.players[i].name, FldGetTextPtr(GetObjectPtr<FieldType>(ConfigName0Field+i, frmP)));

   	VIEW_PLAYER.draw();

	   // только для 3 игроков, для 4-х надо править!!
	   g_prefs.show_prev=(!g_prefs.players[1].comp || !g_prefs.players[2].comp);
   }	
 
   FrmDeleteForm (frmP);
}




// FUNCTION: MainFormDoCommand
//
// DESCRIPTION: This routine performs the menu command specified.
//
// PARAMETERS:
//
// command
//     menu item id

static Boolean MainFormDoCommand(UInt16 command)
{
   Boolean handled = false;
   FormType * frmP;
	
   switch (command)
   {
   case OptionsStartNewGame:
		   MenuEraseStatus(0);
   		player::new_game();
   		return true;
   
   //-----------------------------------------------------------------------
	case OptionsConfiguration:
			MenuEraseStatus(0);
			config_form();
		  	return true;

   //-----------------------------------------------------------------------
	case OptionsStatistics:
			MenuEraseStatus(0);
			statistics_form();
		  	return true;

	//-----------------------------------------------------------------------			
   case HelpAbout:
        // Clear the menu status from the display
        MenuEraseStatus(0);
        // Display the About Box.
        frmP = FrmInitForm (AboutForm);
        FrmCopyLabel(frmP, AboutVersionLabel, __DATE__);
        FrmDoDialog(frmP);
        FrmDeleteForm (frmP);
        handled = true;
        break;
    		
    case HelpRules:
    		FrmHelp(RulesString);
    		break;

    case HelpThatIsee:
    		FrmHelp(ThatIseeString);
    		break;
    }
    
    return handled;
}

// FUNCTION: MainFormHandleEvent
//
// DESCRIPTION:
//
// This routine is the event handler for the "MainForm" of this 
// application.
//
// PARAMETERS:
//
// eventP
//     a pointer to an EventType structure
//
// RETURNED:
//     true if the event was handled and should not be passed to
//     FrmHandleEvent

Boolean MainFormHandleEvent(EventType * eventP)
{
   Boolean handled = false;
   FormType * frmP;

	switch (eventP->eType) 
   {
   case menuEvent:
   	return MainFormDoCommand(eventP->data.menu.itemID);

	case frmOpenEvent:
      frmP = FrmGetActiveForm();
      FrmDrawForm(frmP);
      MainFormInit(frmP);
      handled = true;
      break;
	
	case frmUpdateEvent:
		// To do any custom drawing here, first call 
      // FrmDrawForm(), then do your drawing, and 
      // then set handled to true.
      break;

	case penDownEvent:
		CURR_PLAYER.pen_down(eventP->screenX, eventP->screenY);
		break;
	}
    
	return handled;
}

// turn a5 warning off to prevent it being set off by C++
// static initializer code generation
#pragma warn_a5_access reset
