/* Kozel
  Copyright (C) 2004
  Author: Alexander Semenov <acmain@gmail.com>
*/
#include "kozel.h"
#include "turn.h"
#include "player.h"
#include "KozelRsc.h"

bool	turn::catch_possible;	// есть шанс поймать

//=============================================================
turn::turn()
	:cast	(0)
	,first(unknown)
	,winer(0)
	,n		(0)
	,k6_and_kq(0)
{
}
	

//=============================================================
turn::turn(const turn &t)
	:cast	(t.cast)
	,first(t.first)
	,winer(t.winer)
	,n		(t.n)
	,k6_and_kq	(t.k6_and_kq)
{
	for (Int8 i=0; i<N_PLAYERS; i++)
		move[i]=t.move[i];
}


//=============================================================
const	turn &turn::operator=(const turn &t)
{
	for (Int8 i=0; i<N_PLAYERS; i++)
		move[i]=t.move[i];
	cast=t.cast;
	first=t.first;
	winer=t.winer;
	n=t.n;
	k6_and_kq=t.k6_and_kq;
	return *this;
}


//=============================================================
void turn::clear()
{
	cast=0;
	winer=0;
	n=0;
	first=unknown;
	k6_and_kq=0;
	
	card	empty;
	for (Int8 i=0; i<N_PLAYERS; i++)
		move[i]=empty;
}


//=============================================================
void turn::add_move(const player *p, card c)
{
	if (catch_possible)
		k6_and_kq+=(c==card::kresta_six || c==card::kresta_queen);

	if (n++==0)
	{
		first=(Level)c.level;
		c.level=firsts;
		winer=p->id;
	}
	else
	{
		if (c.level==first)
			c.level=firsts;
		if (move[winer]<c)
			winer=p->id;
	}
	c.droped=true;

	cast+=c.price();

	move[p->id]=c;
}


//=============================================================
void turn::draw() const
{
	player	*p=&VIEW_PLAYER;
	int	x_winer;
	if (N_PLAYERS==3)
	{
		x_winer=72;
		draw_card(64, 59, move[p->id]);
		draw_card(32, 15, move[p->left->id]);
		draw_card(96, 15, move[p->right->id]);
	}
	else
	{
		draw_card(48, 50, move[p->id]);
		draw_card(16, 30, move[p->left->id]);
		draw_card(112, 30, move[p->right->id]);
	}

	// указатель - кто берет
	if (n>0)	
	{
		int id;
		if (winer==p->id)
			id=GetBottomBitmapFamily;
		else if (winer==p->left->id)
			id=GetLeftBitmapFamily;
		else if (winer==p->right->id)
			id=GetRightBitmapFamily;
		else
			id=GetTopBitmapFamily;
		MemHandle	BitmapH = DmGetResource(bitmapRsc, id);
		MemPtr		BitmapP = MemHandleLock(BitmapH);
		WinDrawBitmap((BitmapType*)BitmapP, x_winer, 35);
		MemHandleUnlock(BitmapH);
		DmReleaseResource(BitmapH);
	}
}


//=============================================================
void turn::draw_card(int x, int y, card c) const
{
	if (!c.droped)
		return;
		
	if (c.level==firsts)
		c.level=first;

	c.draw(x, y);
}


//=============================================================
move_test turn::correct_move(const card &c) const
{
	if (n==0)
		return c.level==trumps ? trump : pass;
	return c.level==first ? pass : bad_suit;
}
