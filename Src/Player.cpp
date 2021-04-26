/* Kozel
  Copyright (C) 2004
  Author: Alexander Semenov <acmain@gmail.com>
*/
#include	"kozel.h"

#include	"player.h"
#include <iterator.h>

#include "KozelRsc.h"

#include "strings.h"

static	list<card>	p_droped;	// карты скинутые в прикуп
turn		player::curr_turn,
			player::prev_turn;
int		player::turns_more;
bool		player::alredy_calc;
player	*player::has_kqueen=NULL,
			*player::has_k6=NULL;

//============================================================================
player::player()
	:cast		(0)
	,left		(NULL)
	,right	(NULL)
	,state	(think)
	,id		(this-players)
{
}


//============================================================================
//	инициализация
void player::init(player_prefs *prefs, player *_left, player *_right)
{
	cfg=prefs;
	
	left=_left;
	right=_right;
}


//============================================================================
void player::new_game()
{
	ALL_PLAYERS.cfg->loss=0;
	new_round();
}


//============================================================================
void player::new_round()
{
	bool	end_game=false;
	UInt8	i;
	player_prefs	*p=g_prefs.players;
	for (i=0; i<N_PLAYERS; i++, p++)
		if (p->loss>=12)
		{
			static	char	*loss_str[]={"двенадцать", "четырнадцать", "шестнадцать"};
			FrmCustomAlert(LosserAlert, p->name, loss_str[(p->loss-12)/2], NULL);
			static	char	dec_stat[]={1, 2, 4};
			p->statistics-=dec_stat[(p->loss-12)/2];
			end_game=true;
		}
	if (end_game)
	{
		UInt8 min=10;
		for (i=0, p=g_prefs.players; i<N_PLAYERS; i++, p++)
			if (p->loss==0)
				p->statistics+=4;
			else if (p->loss<min)
				min=p->loss;

		for (i=0, p=g_prefs.players; i<N_PLAYERS; i++, p++)
			if (p->loss==min)
				p->statistics+=2;
				
		statistics_form();		
		new_game();
		return;
	}

	//---------------------------------------------------------------------------
	card::trump_suit = unknown;
	curr_turn.clear();
	has_kqueen=has_k6=NULL;
	g_prefs.n_turns=0;
	ALL_PLAYERS.cards.clear();
	ALL_PLAYERS.cast=0;
	ALL_PLAYERS.state=think;
	ALL_PLAYERS.alredy_calc=false;

	// заполняем колоду
	vector<card>	pack;	// колода
	UInt8	l, f;
	for (l=bubna; l<=pika; l++)
		for (f=six; f<=ace; f++)
			pack.push_back(card((Level)l, (Force)f));

	// крести
	for (f=eight; f<=ace; f++)
		pack.push_back(card(kresta, (Force)f));

	// постоянные козыри
	for (f=b_jack; f<=k_six; f++)
		pack.push_back(card(trumps, (Force)f));

	for (UInt8 i=0; i<10; i++)	// раздача карт
		ALL_PLAYERS.get_card(pack);


	// выдача прикупа
	card	kj(trumps, k_jack),
			pj(trumps, p_jack),
			cj(trumps, c_jack),
			j(trumps, k_jack);
	if (pack[0]==kj)
		j=pj;
	else
		if (pack[1]==kj)
			j=(pack[0]==pj) ? cj : pj;
	ALL_PLAYERS.prikup(pack, j);
	
	VIEW_PLAYER.update_cards_location();
	VIEW_PLAYER.draw();
}


//============================================================================
//	раздача
void player::get_card(vector<card> &pack)	// раздача
{
	vector<card>::iterator num=pack.begin();

	num+=SysRandom(0)%pack.size();
	cards.push_back(*num);

	if (*num==card::kresta_six)
		has_k6=this;
	else if (*num==card::kresta_queen)
		has_kqueen=this;

	pack.erase(num);
}


//============================================================================
void player::draw()
{
	WinScreenLock(winLockDontCare);//winLockErase);

#ifndef COLOR_DEPTH_4
	static		RGBColorType	table={0, 64, 127, 16},
								text={0, 232, 205, 21};

	if (os_4_avaiable)
	{
		WinSetBackColorRGB(&table, NULL);
		WinSetTextColorRGB(&text, NULL);
		WinSetForeColorRGB(&text, NULL);
	}

	static		RectangleType	rec={0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	WinFillRectangle(&rec, 0);
#else
	 WinEraseWindow();
#endif

	// счет
	char	str[20];
   FntSetFont(boldFont);
   StrIToA(str, cfg->loss);
   WinDrawChars(str, StrLen(str), 0, 89);
   StrIToA(str, left->cfg->loss);
   WinDrawChars(str, StrLen(str), 0, 10);
   StrIToA(str, right->cfg->loss);
   WinDrawChars(str, StrLen(str), SCREEN_WIDTH-FntCharsWidth(str, StrLen(str)), 10);
	FntSetFont(stdFont);

	// очки	
	if (card::trump_suit!=unknown)
	{
   	WinDrawChars("Очки:", 5, 121, 89);
	   StrIToA(str, cast);
	   WinDrawChars(str, StrLen(str), 145, 89);

		WinDrawChars(card::suit_str[card::trump_suit], 1, 150, 99);
		WinDrawChars("Козырь:", 7, 113, 99);
	}
	
	int i;
	
	// карты игрока слева
	for (i=0; i<left->cards.size(); i++)
		draw_bitmap(SuitBitmapFamily, 0, 20+i*5);

	// карты игрока справа
	for (i=0; i<right->cards.size(); i++)
		draw_bitmap(SuitBitmapFamily, SCREEN_WIDTH-CARD_WIDTH/2, 20+i*5);
		
	// карты игрока
	card_iter	j=cards.begin();
	update_cards_location();
	for (i=cards_shift; j!=cards.end(); i+=cards_step, j++)
		(*j).draw(i>>4, SCREEN_HEIGHT-CARD_HEIGHT);

	// рисуем ходы
	if (CURR_PLAYER.state==wait)
		prev_turn.draw();
	else
		curr_turn.draw();
	
	// имена
	draw_name(0, 99);
	left->draw_name(0, 0);
	right->draw_name(SCREEN_WIDTH-FntCharsWidth(right->cfg->name, StrLen(right->cfg->name)), 0);
	WinScreenUnlock();
}


//============================================================================
void player::draw_name(int x, int y)	const
{
	if (this==&(PRIM_PLAYER))
		WinDrawInvertedChars(cfg->name, StrLen(cfg->name), x, y);
	else
		WinDrawChars(cfg->name, StrLen(cfg->name), x, y);
}



//============================================================================
void player::prikup(vector<card> &pack, const card &jack)
{
	card_iter	i=cards.begin();
	
	while (*i!=jack)	// поиск вальта в картах
		if (++i==cards.end())
			return;
	
	cards.push_back(pack[0]);
	cards.push_back(pack[1]);
	
	g_prefs.primary=g_prefs.current=(this-players);
	left->fellow=right;
	right->fellow=left;
	fellow=NULL;
	
	// если k6 и kd не были розданы, значит они в прикупе
	if (!has_k6)
		has_k6=this;
	if (!has_kqueen)
		has_kqueen=this;
	turn::catch_possible=(has_k6!=has_kqueen && has_k6->fellow!=has_kqueen);
	
	if (!cfg->comp)
	{
		g_prefs.view_in=this-players;
		state=drop;
		p_droped.clear();
		ALL_PLAYERS.cards.sort();
		FrmCustomAlert(PrikupAlert, cfg->name, NULL, NULL);
	}
	else
	{
		if (g_prefs.view_in!=0)
		{
			g_prefs.view_in=0;
			FrmCustomAlert(NextPlayerAlert, players[0].cfg->name, NULL, NULL);
		}
		
		cards.sort();
		card::trump_suit=kresta;
		// назначаем козырь
		if (g_prefs.ai_skill==0)	// чтобы легче было
		{
			for (UInt8 min=8, n=0, l=0; l<4; l++)
				if ( (n=n_on_level((Level)l))<min )
				{
					min=n;
					card::trump_suit=(Level)l;
				}
		}
		else
			for (UInt8 max=0, n=0, l=0; l<4; l++)
				if ( (n=n_on_level((Level)l))>max )
				{
					max=n;
					card::trump_suit=(Level)l;
				}
		set_trump();
		VIEW_PLAYER.draw();
		
		// скидываем прикуп
		ai_drop_prikup();
		ai_go();
	}
}


//============================================================================
void player::ai_drop_prikup()
{
	if (g_prefs.ai_skill==0)	// чтобы легче было
	{
		cast+=(*cards.end()).price();
		cards.pop_back();
		cast+=(*cards.end()).price();
		cards.pop_back();
		return;
	}
			
	card_iter	i;
	UInt8	n[6]={0, 0, 0, 0, 0, 0};	// кол-во карт по мастям (без тузов)

	for (i=cards.begin(); i!=cards.end(); i++)
		if ((*i).force!=ace)
			n[(*i).level]++;
			
	
	// ищем единственную карту
	if (n[0]==1 || n[1]==1 || n[2]==1 || n[3]==1)
	{
		for (i=cards.begin(); (*i).level<=kresta && cards.size()>10; i++)
			if (n[(*i).level]==1)
			{
				n[(*i).level]=0;
				cast+=(*i).price();
				cards.erase(i);
			}
	}
	// ищем 2 единственные карты в масти
	else	if (n[0]==2 || n[1]==2 || n[2]==2 || n[3]==2)
		for (i=cards.begin(); (*i).level<=kresta; i++)
			if (n[(*i).level]==2)
			{
				card_iter	j=i++;
				cast+=(*j).price()+(*i).price();
				cards.erase(j, ++i);
				return;
			}

	// ищем десятки
	for (i=cards.begin(); (*i).level<=kresta && cards.size()>10; i++)
		if ((*i).force==ten)
		{
			cast+=(*i).price();
			cards.erase(i);
		}

	// ищем королей
	for (i=cards.begin(); (*i).level<=kresta && cards.size()>10; i++)
		if ((*i).force==king)
		{
			cast+=(*i).price();
			cards.erase(i);
		}

	// ищем любые 2 карты отличные от туза
	for (i=cards.begin(); (*i).level<=kresta && cards.size()>10; i++)
		if ((*i).force!=ace)
		{
			cast+=(*i).price();
			cards.erase(i);
		}

	// если не помогло
	while (cards.size()>10)
	{
		cast+=(*cards.begin()).price();
		cards.pop_front();
	}
}


//============================================================================
int player::n_on_level(Level l)
{
	int n=0;

	for (card_iter	i=cards.begin(); i!=cards.end(); i++)
		if ((*i).level==l)
			n++;
			
	return n;
}


//============================================================================
void player::update_cards_location()
{
	if (cards.size()>SCREEN_WIDTH/CARD_WIDTH)
	{
		cards_shift=0;
		cards_step=((SCREEN_WIDTH-CARD_WIDTH)<<4)/(cards.size()-1);
	}
	else
	{
		cards_shift=(SCREEN_WIDTH-CARD_WIDTH*cards.size())<<3;
		cards_step=(CARD_WIDTH<<4);
	}
}


//============================================================================
void player::pen_down(int x, int y)
{
	card_iter	pos=cards.end();

	// определение отмеченой карты
	if (!cfg->comp && y>SCREEN_HEIGHT-CARD_HEIGHT)
	{
		card_iter	p=cards.begin();
			
		for (int i=cards_shift; p!=cards.end(); i+=cards_step, p++)
			if (x>=(i>>4) && x<(i>>4)+CARD_WIDTH)
				pos=p;
	}
	
	//----------------------------------------------------
	switch(state)
	{
	case wait:
		state=think;
		VIEW_PLAYER.draw();
		if (cfg->comp)
			ai_go();
		else if (g_prefs.auto_step)
		{
			card_iter	last;
			if (available_moves(pos, last, curr_turn)==1)
				go(pos);
		}
		break;
	
	//----------------------------------------------------
	case think:
		if (pos==cards.end())	// карта не выбрана
			break;
			
		switch (curr_turn.correct_move(*pos))
		{
		case trump:
			if ( (*cards.begin()).level!=trumps && g_prefs.primary!=id )
			{
				FrmCustomAlert(TrumpMoveAlert, players[g_prefs.primary].cfg->name, NULL, NULL);
				return;
			}
			break;
		
		case bad_suit: 
			if ( n_on_level(curr_turn.first) )
			{
				FrmCustomAlert(BadStepAlert, card::suit_str[curr_turn.first], NULL, NULL);
				return;
			}
			break;
		}
			
		go(pos);
		break;

	//----------------------------------------------------------		
	case drop:
		if (pos==cards.end())
			break;

		p_droped.push_back(*pos);
		cast+=(*pos).price();
		cards.erase(pos);
		update_cards_location();
		draw();
		
		if (p_droped.size()==2)
		{
			FormType *frmP = FrmInitForm (GoRoundForm);
			FrmSetControlGroupSelection(frmP, 1, GoRoundKPushButton);
			if (FrmDoDialog (frmP)==GoRoundGoButton)
			{
				card::trump_suit=(Level)(FrmGetControlGroupSelection(frmP, 1)-1);
				set_trump();
     			state=think;
   		}
   		else
   		{
   			cast=0;
				cards.merge(p_droped);
				cards.sort();
				update_cards_location();
   		}
		   FrmDeleteForm (frmP);
		   draw();
		}
		break;
	}
}


//============================================================================
void player::set_trump()
{
   // перевод масти в козыря
	card_iter	i;
   int	j;
   for (j=0; j<N_PLAYERS; j++)	
   	for (i=players[j].cards.begin(); i!=players[j].cards.end(); i++)
   		if ((*i).level==card::trump_suit)
   			(*i).level=trumps;

	ALL_PLAYERS.cards.sort();
}


//============================================================================
void	player::next_player()
{
	g_prefs.current=id;
	
	if (curr_turn.end())	// ход закончен
	{
		// ловим даму
		if (curr_turn.kq_caught())
		{
			VIEW_PLAYER.draw();
			FrmCustomAlert(CatchAlert, has_kqueen->cfg->name, has_k6->cfg->name, NULL);
			has_kqueen->cast=1;
			has_kqueen->add_loss();
			new_round();
			return;
		}
		else if (curr_turn.k6_and_kq)	// если выбыла k6 или kq
			turn::catch_possible=false;

		prev_turn=curr_turn;
		g_prefs.current=curr_turn.winer;

		CURR_PLAYER.get_cast();
		
		if (++g_prefs.n_turns==10)	// партия закончена
		{
			VIEW_PLAYER.draw();
			if (PRIM_PLAYER.cast==60)
				FrmAlert(DrawAlert);
			else if (PRIM_PLAYER.cast<60)
				PRIM_PLAYER.add_loss();
			else
				PRIM_PLAYER.left->add_loss();
			new_round();
			return;		
		}
		curr_turn.clear();

		CURR_PLAYER.state=wait;
	}
	
	CURR_PLAYER.prepare_to_go();
}


//============================================================================
void player::prepare_to_go()
{
	if (cfg->comp)
	{
		VIEW_PLAYER.draw();
		if (state!=wait)
			ai_go();
	}
	else
	{
		if (g_prefs.view_in!=id)
		{
			WinEraseWindow();
			FrmCustomAlert(NextPlayerAlert, cfg->name, NULL, NULL);
			g_prefs.view_in=id;
			if (cards.size()<10 && g_prefs.show_prev)
				state=wait;
		}

		draw();

		if (state!=wait && g_prefs.auto_step)
		{
			card_iter	last, pos;
			if (available_moves(pos, last, curr_turn)==1)
				go(pos);
		}
	}
}


//============================================================================
void player::go(const card_iter &pos)
{
	curr_turn.add_move(this, *pos);
	cards.erase(pos);
	update_cards_location();
	alredy_calc=cfg->comp;
	left->next_player();
}




//============================================================================
void player::get_cast()
{
	cast+=curr_turn.cast;
	if (fellow)
		fellow->cast=cast;
}


//============================================================================
void player::add_loss()
{
	int	add_loss=0;
	if (cast==0)
		add_loss=6;
	else if (cast<30)
		add_loss=4;
	else
		add_loss=2;
	
	char	*loss_str[]={"двух", "четырех", "шести"};
	cfg->loss+=add_loss;
	if (fellow)
	{
		fellow->cfg->loss+=add_loss;
		FrmCustomAlert(TwoLossAlert, cfg->name, fellow->cfg->name, loss_str[add_loss/2-1]);
	}
	else
		FrmCustomAlert(OneLossAlert, cfg->name, loss_str[add_loss/2-1], NULL);
}



//==============================================
// замена стека
static	struct
{
	turn			T;
	card_iter	Curr, Last, Pos;
}stack[50];

//============================================================================
void player::ai_go()
{
	UInt8	i;

	static	UInt8	ai_turns[3][10]=	// ск. ходов на расчет отводится для AI
	{
		{1, 1, 1, 1, 1, 2, 2, 3, 2, 1},	// easy
		{2, 2, 2, 2, 2, 3, 4, 3, 2, 1},	// normal
		{2, 2, 2, 2, 3, 5, 4, 3, 2, 1},	// hard
	};

	// табло прогресса
	progress=0;
	draw_bitmap(ProgressBitmapFamily, 62, 3);
	turns_more=ai_turns[g_prefs.ai_skill][g_prefs.n_turns];
	stack[0].T=curr_turn;
	timer=-1;

	if (turns_more==10-g_prefs.n_turns)
		ai_get_move(1);// идет полный рассчет
	else	// упрощенный рассчет
	{
		// чтобы комп не кидал дам ради очков
		for (i=b_queen; i<k_queen; i++)
			card::points[i]=2;
		if (turn::catch_possible)	// скидывать как можно раньше
			card::points[k_queen]=3;
		// чтобы k6 рано не выкидывал
		card::points[k_six]=(turn::catch_possible ? -20 : -11);
	
		ai_get_move(1);
	
		// отмена
		for (i=b_queen; i<=k_queen; i++)
			card::points[i]=3;
		card::points[k_six]=0;
	}
		
	go(stack[1].Pos);
}


#define t		stack[st].T
#define prev_t	stack[st-1].T
#define next_t	stack[st+1].T
#define curr	stack[st].Curr
#define last	stack[st].Last
#define pos		stack[st].Pos

//============================================================================
void player::ai_get_move(const UInt8 st)
{
	static	score s;	// общий счет
	
	UInt8	n=available_moves(curr, last, prev_t);
	
	if (st==1 && n==1)	// единственный ход
	{
		pos=curr;
		return;
	}

	//---------------------------------------------------------------------------------
	//	Перебераем последовательно все возможные ходы.	
	score	max_cast={-125, -125, -125, -125};
	for (++last; curr!=last; curr++)
		if ( !(*curr).droped )
		{
			//------------------------------------------------------------
			draw_timer();
			if (st==1)
			{
				static	RectangleType	rect={{64, 6}, {0, 5}};
				rect.extent.x=((++progress)<<5)/(n+1);
				WinDrawRectangle(&rect, 0);
			}
			
			//------------------------------------------------------------
			t=prev_t;
			(*curr).droped=true;
			t.add_move(this, *curr);

			//------------------------------------------------------------			
			if (!t.end())
				left->ai_get_move(st+1);
			else
			{
				if (t.kq_caught())
				{
					s.summ=0;
					t.cast=121;
				}
				else
				{
					if (--turns_more)	// не последний ход
					{
						next_t.clear();
						players[t.winer].ai_get_move(st+2);
					}
					else
						s.summ=0;
					turns_more++;
				}
				
				if (s.cast[0]!=121 && s.cast[0]!=-121)	// была поймана дама
				{
					s.cast[t.winer]+=t.cast;
					if (N_PLAYERS==4)
					{
						s.cast[players[t.winer].fellow->id]+=t.cast;
						s.cast[players[t.winer].left->id]-=t.cast;
						s.cast[players[t.winer].right->id]-=t.cast;
					}
					else	// 3 игрока
						if (!players[t.winer].fellow)
						{
							s.cast[players[t.winer].left->id]-=t.cast;
							s.cast[players[t.winer].right->id]-=t.cast;
						}
						else
							if (players[t.winer].fellow==players[t.winer].left)
							{
								s.cast[players[t.winer].fellow->id]+=t.cast;
								s.cast[players[t.winer].right->id]-=t.cast;
							}
							else
							{
								s.cast[players[t.winer].fellow->id]+=t.cast;
								s.cast[players[t.winer].left->id]-=t.cast;
							}
				}
			}
					
			(*curr).droped=false;
			//---------------------------------------------------------------------
			if ( s.cast[id]>max_cast.cast[id] )	// если выигрыш больше
			{
				pos=curr;
				max_cast.summ=s.summ;
			}
		}
	s.summ=max_cast.summ;
//	return pos;
}

#undef	t
#undef	prev_t
#undef	next_t
#undef	curr
#undef	last
#undef	pos



UInt32	player::timer,
			player::progress;
//==================================================================
void player::draw_timer()	const
{
	static	UInt32	TicksPerSecond=SysTicksPerSecond(),
							ticks=0;
	
	if (TimGetTicks()<ticks)
		return;
	ticks=TimGetTicks()+TicksPerSecond;
	timer++;

	static	char	str[10];
	StrPrintF(str, "%d:%d  ", (int)(timer/60), (int)(timer%60));
   WinDrawChars(str, StrLen(str), 73, 20);
}




//==================================================================
int player::available_moves(card_iter &first, card_iter &last, const turn &t)
{
	//	Ищем отрезок в котором находятся возможные ходы и их кол-во
	first=cards.begin();
	while ( (*first).droped )	// обходим все карты помеченые как сброшенные
		first++;

	card_iter	pos=first;
	last=first;
	
	UInt8	n=0;

	if (t.first==unknown)	// этот ход - первый
	{
		if (g_prefs.primary!=id)	// нельзя заходить с козыря
			for (; pos!=cards.end() && (*pos).level!=trumps; pos++)
				if ( !(*pos).droped )
				{
					last=pos;
					n++;
				}

		if (n==0)
			for (; pos!=cards.end(); pos++)
				if ( !(*pos).droped )
				{
					last=pos;
					n++;
				}
	}
	else	// есть масть захода
		for (; pos!=cards.end(); pos++)// ищем первую карту масти
			if ( !(*pos).droped )
				if ( (*pos).level!=t.first )
				{
					last=pos;
					n++;
				}
				else
				{
					first=pos;
					n=0;
					// ищем последнюю карту масти
					for (; pos!=cards.end() && (*pos).level==t.first; pos++)
						if ( !(*pos).droped )
						{
							last=pos;
							n++;
						}		
					break;
				}
	
	return n;
}
