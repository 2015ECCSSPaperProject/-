
#include	"iextreme.h"
#include	"../Enum_public.h"
//#include	"../poster/Poster_manager.h"
#include	"Player_manager.h"
#include	"Player.h"

Player_manager::Player_manager()
{
	players.clear();
	players.resize(0);
}

Player_manager::~Player_manager()
{
	Release();
}

void Player_manager::Initialize(Poster_manager *poster_mng)
{
	obj[0] = new iex3DObj("DATA/CHR/player/run.IEM");

	this->poster_mng = poster_mng;

	// ‚±‚±‚Ç‚¤‚·‚é‚©
	TEAM_COLOR col = TEAM_COLOR::ONE;

	Player *p;

	p = new Player();
	(col == TEAM_COLOR::ONE) ? p->Initialize(obj[0], poster_mng, col) : p->Initialize(obj[1], poster_mng, col);

	players.push_back(p);
}

void Player_manager::Release()
{
	delete obj[0];

	for (auto it : players) delete it;

	players.clear();
}

void Player_manager::Update(Stage *stage, Poster_manager *postermng)
{
	for (auto it : players)
	{
		it->Update(stage);
	}
}

void Player_manager::Render()
{
	for (auto it : players)
	{
		it->Render();
	}
}
