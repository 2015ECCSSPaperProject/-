
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
	obj = new iex3DObj("DATA/CHR/teki_full.IEM");
	this->poster_mng = poster_mng;

	// ‚±‚±‚Ç‚¤‚·‚é‚©
	TEAM_COLOR col = TEAM_COLOR::ONE;

	Player *p;

	p = new Player();
	p->Initialize(obj, poster_mng, col);

	players.push_back(p);
}

void Player_manager::Release()
{
	delete obj;

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
