/* Kozel
  Copyright (C) 2004
  Author: Alexander Semenov <acmain@gmail.com>
*/
#ifndef CARD_H_
#define CARD_H_

#include	"CardsRsc.h"

#define CARD_WIDTH	32
#define CARD_HEIGHT	50


// сила карты (6-T, Д-6к итп)
enum	Force
{
	six,
	eight,
	nine,
	king,
	ten,
	ace,
	b_jack,
	c_jack,
	p_jack,
	k_jack,
	b_queen,
	c_queen,
	p_queen,
	k_queen,
	k_six
};

// уровень карты (масть, заход, козырь)
enum	Level
{
	bubna=0,
	cherva=1,
	pika=2,
	kresta=3,
	firsts,	// заход
	trumps,	// козыри
	unknown	// неопределен
};


//=================================================
// карта
//=================================================
class card
{
private:

	union
	{
		struct
		{
			UInt8		p_level	:3,	// уровень карты (масть, заход, козырь)
						p_force	:4,	// сила карты (6-T, Д-6к итп)
						p_droped	:1;	// карта сброшена
		}param;
		UInt8	rating;
	};
	#define	force		param.p_force
	#define	level		param.p_level
	#define	droped	param.p_droped

	//=========================================================
public:
	card()
		:rating(0)
		{}

	card(Level l, Force f)
		:rating	((l<<5)|(f<<1))
		{}
	card(const card &c)
	 	:rating	(c.rating)
	 	{};
	void draw(int x, int y) const;
	Int8	price()	const	{ return points[force]; }
	bool operator==(const card &c) const	{return (rating>>1)==(c.rating>>1);}
	bool operator<(const card &c) const 	{return rating<c.rating;}
	bool operator!=(const card &c) const	{return !operator==(c);}
	
	static	Level	trump_suit;	// козырь
					//	first_suit;	// заход
	static	char	*suit_str[6];
	static	card	kresta_six, kresta_queen;
	static	Int8	points[];	// цена карт

	friend class turn;
	friend class player;
};



#endif CARD_H_