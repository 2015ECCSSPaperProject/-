
#pragma once

#include <vector>
#include "../../../share_data/Enum_public.h"

struct Vector3;

struct IDirect3DTexture9;
typedef IDirect3DTexture9 Texture2D;

class iex3DObj;
class Poster;
class Poster_frame;
class Score;
enum class TEAM_COLOR;
class BasePlayer;

class Poster_manager
{
private:
	Texture2D *poster_textures[(int)TEAM_COLOR::NONE]; // ポスターの絵
	iex3DObj *poster_model; // クローン関数使う用
	int number_of_posters;
	std::vector<Poster*> posters; // ポスター
	Poster_frame *frame;

	Score *score;

	void Load_poster_pos(char *filename);

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

	// 破れる、貼れるポスターの番号
	int Can_do(BasePlayer *player, TEAM_COLOR color);
	
	// Can_doの後で呼ぶ
	bool Can_rend(TEAM_COLOR color, int poster_num);          bool Can_paste(TEAM_COLOR color, int poster_num);

	// 破る貼る
	void Rend_poster(TEAM_COLOR color, int poster_num);
	void Paste_poster(TEAM_COLOR color, int poster_num);

	//**************************************************
	/// Get Set
	//**************************************************

	int Get_numof(){ return number_of_posters; }
	TEAM_COLOR Get_color(int index);
	int Get_animation_frame(int index);

	// 追加しました1016
	const Vector3& Get_pos(int index);
	float Get_angle(int index);


};

extern Poster_manager *poster_mng;
