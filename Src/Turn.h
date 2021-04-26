/* Kozel
  Copyright (C) 2004
  Author: Alexander Semenov <acmain@gmail.com>
*/
#ifndef TURN_H_
#define TURN_H_

#include "Kozel.h"
#include "card.h"

class player;
class	card;



enum	move_test
{
	pass,		// можно ходить
	bad_suit,// ход не в масть
	trump	// первый ход козырем
};

//==============================================================
//	один ход
//==============================================================
class turn
{
public:
	turn();
	turn(const turn &t);
	const	turn &operator=(const turn &t);
	void clear();
	void add_move(const player *p, card c);
	bool end() const { return n==g_prefs.n_players || k6_and_kq==2; };
	void draw() const;	// вид от VIEW_PLAYER
	void draw_card(int x, int y, card c) const;
	move_test correct_move(const card &c) const;
	bool kq_caught()	const { return k6_and_kq==2; };

	//-------------------------------------------------------------------------
	static	bool	catch_possible;	// есть шанс поймать
	Level	first;	// заход
	
private:
	card	move[4];	// ходы всех игроков.	move[].droped - уже сходил
	Int8	cast,		// взятка
			winer,	// id берущего игрока
			n,			// кол-во сходивших
			k6_and_kq;	// в заходе есть k6 или kq (0 - нет, 1-одна, 2 -обе)
	friend class player;
};

#endif	// TURN_H_