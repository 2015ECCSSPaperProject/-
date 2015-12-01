
#pragma once

struct IDirect3DTexture9;
typedef IDirect3DTexture9 Texture2D;

class iex3DObj;
class Poster;
//class Poster_frame;
class Score;
class BasePlayer;

class Poster_manager
{
private:
	iex3DObj *poster_model; // クローン関数使う用
	int number_of_posters;
	std::vector<Poster*> posters; // ポスター
	//Poster_frame *frame;

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
	int Can_do(BasePlayer *player, int number);
	
	// Can_doの後で呼ぶ
	bool Can_rend(int number, int poster_num);          bool Can_paste(int number, int poster_num);

	// 破る貼る
	void Rend_poster(int number, int poster_num);
	void Paste_poster(int number, int poster_num);

	//**************************************************
	/// Get Set
	//**************************************************

	int Get_numof(){ return number_of_posters; }
	void Set_user(int index, int user_number);
	void Set_animframe(int index, int frame);
};

extern Poster_manager *poster_mng;
