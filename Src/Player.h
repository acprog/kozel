/* Kozel
  Copyright (C) 2004
  Author: Alexander Semenov <acmain@gmail.com>
*/
#ifndef PLAYER_H_
#define PLAYER_H_

#include "Kozel.h"

#include <vector.h>
#include <set.h>
#include	<algorithm.h>
#include	<list>
using std::list;

#include "card.h"
#include	"turn.h"


typedef	list<card>::iterator	card_iter;


// обобщенный счет по всем 4 игрокам
union	score
{
	Int8	cast[4];
	Int32	summ;
};

//======================================================================
//	Макросы
//======================================================================
#define	CURR_PLAYER	players[g_prefs.current]
#define	VIEW_PLAYER	players[g_prefs.view_in]
#define	PRIM_PLAYER	players[g_prefs.primary]
#define	N_PLAYERS	g_prefs.n_players

#define	ALL_PLAYERS	for (UInt8	players_index=0; players_index<N_PLAYERS; players_index++)	players[players_index]

extern	player	players[];

//======================================================================
//	Игрок
//======================================================================
class player
{
public:
	player();
	void init(player_prefs *prefs, player *_left, player *_right);
	static	void new_game();
	static	void new_round();
	void draw();
	void pen_down(int x, int y);
	void update_cards_location();

private:
	void get_card(vector<card>	&pack);	// раздача
	void draw_name(int x, int y)	const;
	void draw_timer()	const;
	void prikup(vector<card> &pack, const card &jack);	// берем прикуп (передается необходимый валет)
	void next_player();
	void get_cast();
	void prepare_to_go();
	void set_trump();
	void add_loss();
	void go(const card_iter &pos);
	int  n_on_level(Level l);
	int  available_moves(card_iter &first, card_iter &last, const turn &t);	// допустимые ходы
	// процедуры AI
	void ai_drop_prikup();
	void ai_go();
	void ai_get_move(const UInt8 st);

	//-----------------------------------------------------------------------
private:
	player_prefs	*cfg;
	list<card>	cards;	// карты
	static	turn	curr_turn,
						prev_turn;
	static	int	turns_more;	// сколько ходов осталось прощитать

	static	player	*has_kqueen,	// у кого kq и
							*has_k6;			// k6

	UInt8		cast,		// очки
				id;
	player	*left,	// игроки слева и справа
				*right,	//
				*fellow;	// напарник
	static	bool			alredy_calc;	// предыдущий ход был за компьютером
	static	UInt32		timer,
								progress;
		
	enum
	{
		think,	// обдумывание
		wait,		// ожидание нажатия
		drop		// сброс карт в прикуп
	}state;
	UInt16	cards_shift,
				cards_step;

	friend class turn;
};

#endif	// PLAYER_H_