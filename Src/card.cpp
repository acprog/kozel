/* Kozel
  Copyright (C) 2004
  Author: Alexander Semenov <acmain@gmail.com>
*/
#include <PalmOS.h>

#include "Kozel.h"
#include "KozelRsc.h"

#include "card.h"

#include	<string.h>

card	card::kresta_six(trumps, k_six),
		card::kresta_queen(trumps, k_queen);
		
Int8	card::points[]={0, 0, 0, 4, 10, 11, 2, 2, 2, 2, 3, 3, 3, 3, 0};
		
Level	card::trump_suit=unknown;
char	*card::suit_str[6]=
	{
	"\215",
	"\217",
	"\220",
	"\216",
	"Заход",
	"Козырь"
	};




//=============================================================
void card::draw(int x, int y) const
{
	card	c(*this);
	
	if (c.level==trumps && c.force<b_jack)
		c.level=trump_suit;

	draw_bitmap(B6BitmapFamily+c.level*100+c.force, x, y);

	if (g_prefs.show_labels)
	{
	#ifndef COLOR_DEPTH_4
		RGBColorType	white={0, 255, 255, 255},
						prev_bk,
						prev_text,
						black={0, 0, 0, 0},
						red={0, 255, 0, 0},
						*text=&black;
		
		if (os_4_avaiable)
		{
			if (c.level==cherva || c.level==bubna)
				text=&red;
			else if (c.force==b_jack || c.force==c_jack || c.force==b_queen || c.force==c_queen)
				text=&red;

			WinSetBackColorRGB(&white, &prev_bk);
			WinSetTextColorRGB(text, &prev_text);
		}
	#endif
	
		static	char	*rank_str[]={"6 ", "8 ", "9 ", "K ", "10", "T ", "B\215", "B\217", "B\220", "B\216", "Д\215", "Д\217", "Д\220", "Д\216", "6\216"};
		WinDrawChars(rank_str[c.force], 2, x, y);
		if (c.level!=trumps || c.force<b_jack)
			WinDrawChars(suit_str[c.level], 1, x+5, y);

	#ifndef COLOR_DEPTH_4
		if (os_4_avaiable)
		{
			WinSetBackColorRGB(&prev_bk, NULL);
			WinSetTextColorRGB(&prev_text, NULL);
		}
	#endif
	}
}
