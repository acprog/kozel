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
	pass,		// ����� ������
	bad_suit,// ��� �� � �����
	trump	// ������ ��� �������
};

//==============================================================
//	���� ���
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
	void draw() const;	// ��� �� VIEW_PLAYER
	void draw_card(int x, int y, card c) const;
	move_test correct_move(const card &c) const;
	bool kq_caught()	const { return k6_and_kq==2; };

	//-------------------------------------------------------------------------
	static	bool	catch_possible;	// ���� ���� �������
	Level	first;	// �����
	
private:
	card	move[4];	// ���� ���� �������.	move[].droped - ��� ������
	Int8	cast,		// ������
			winer,	// id �������� ������
			n,			// ���-�� ���������
			k6_and_kq;	// � ������ ���� k6 ��� kq (0 - ���, 1-����, 2 -���)
	friend class player;
};

#endif	// TURN_H_