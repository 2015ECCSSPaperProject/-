
#pragma once

#include <vector>
#include "../Enum_public.h"

struct IDirect3DTexture9;
typedef IDirect3DTexture9 Texture2D;

class iex3DObj;
class Poster;
class Poster_frame;
class Score;
enum class TEAM_COLOR;
class Player;

class Poster_manager
{
private:
	Texture2D *poster_textures[(int)TEAM_COLOR::NONE]; // ポスターの絵
	iex3DObj *poster_model; // クローン関数使う用
	int number_of_posters;
	std::vector<Poster*> posters; // ポスター
	Poster_frame *frame;

	Score *score;
public:
	Poster_manager();
	~Poster_manager();

	void Initialize(Score *score);
	void Release();
	void Update();
	void Render();

	//**************************************************
	/// プレイヤーからのアクション
	//**************************************************

	void Do_poster(Player *player, TEAM_COLOR color); // 破る貼る
	void Rend_poster(Player *player, TEAM_COLOR color); // 破る
	void Paste_poster(Player *player, TEAM_COLOR color); // 貼る
};
