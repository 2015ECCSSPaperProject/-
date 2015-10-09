
#pragma once

#include <vector>

class Player;
class Stage;
class Poster_manager;

class Player_manager
{
private:
	std::vector<Player*> players;
	iex3DObj *obj[2]; // �N���[���֐��g���p
	Poster_manager *poster_mng;

public:
	Player_manager();
	~Player_manager();

	void Initialize(Poster_manager *poster_mng);
	void Release();
	void Update(Stage *stage, Poster_manager *poster_mng);
	void Render();


	// ��
	Player *Get_Player(){ return *(players.begin()); }
};
