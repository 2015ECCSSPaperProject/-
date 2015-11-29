
#pragma once

#include <vector>
#include "iextreme.h"

struct Vector3;

struct IDirect3DTexture9;
typedef IDirect3DTexture9 Texture2D;

class iex3DObj;
class Poster;
//class Poster_frame;
class BasePlayer;

class Poster_mng
{
private:
	iex3DObj *poster_model; // クローン関数使う用
	int number_of_posters;
	std::vector<Poster*> posters; // ポスター
	//Poster_frame *frame;

	void Load_poster_pos(char *filename);

public:
	Poster_mng();
	~Poster_mng();

	void Initialize();
	void Release();
	void Update();
	void Render();

	//**************************************************
	/// プレイヤーからのアクション
	//**************************************************

	// 破れる、貼れるポスターの番号
	int Can_do(BasePlayer *player, int number);
	int Can_do(BasePlayer *player);

	int Can_dist(const Vector3 &pos, float dist, int number);
	void Can_dist(const Vector3 &pos, float dist, int number, int out[]);	// 範囲内ポスター全部返すver
	void Can_dist(const Vector3 &pos, float dist, int out[]);

	// Can_doの後で呼ぶ
	bool Can_rend(int number, int poster_num);          bool Can_paste(int number, int poster_num);
	bool Can_rend(int poster_num);

	// 破る貼る
	void Rend_poster(int number, int poster_num);
	void Rend_poster(int poster_num);
	void Paste_poster(int number, int poster_num);

	//**************************************************
	/// Get Set
	//**************************************************

	int Get_numof(){ return number_of_posters; }
	int Get_number(int index);
	int Get_animation_frame(int index);

	// 追加しました1016
	const Vector3& Get_pos(int index);
	float Get_angle(int index);


};
