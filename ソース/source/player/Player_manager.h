
#pragma once

#include <vector>

class Player;
class Stage;
class Poster_manager;

class Player_manager
{
private:
	std::vector<Player*> players;
	iex3DObj *obj[2]; // クローン関数使う用
	Poster_manager *poster_mng;

public:
	Player_manager();
	~Player_manager();

	void Initialize(Poster_manager *poster_mng);
	void Release();
	void Update(Stage *stage, Poster_manager *poster_mng);
	void Render();


	// 仮
	Player *Get_Player(){ return *(players.begin()); }
};
