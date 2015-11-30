
#pragma once

#include <vector>
#include "../../IEX/iextreme.h"

struct Vector3;

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
	iex3DObj *poster_model; // �N���[���֐��g���p
	int number_of_posters;
	std::vector<Poster*> posters; // �|�X�^�[
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
	/// �v���C���[����̃A�N�V����
	//**************************************************

	// �j���A�\���|�X�^�[�̔ԍ�
	int Can_do(BasePlayer *player, int number);
	int Can_do(BasePlayer *player);

	int Can_dist(const Vector3 &pos, float dist, int number);
	void Can_dist(const Vector3 &pos, float dist, int number, int out[]);	// �͈͓��|�X�^�[�S���Ԃ�ver
	void Can_dist(const Vector3 &pos, float dist, int out[]);

	int Can_targeting(BasePlayer *player, float range_dist, int range_degree);

	// Can_do�̌�ŌĂ�
	bool Can_rend(int number, int poster_num);          bool Can_paste(int number, int poster_num);
	bool Can_rend(int poster_num);

	// �j��\��
	void Rend_poster(int number, int poster_num);
	void Rend_poster(int poster_num);
	void Paste_poster(int number, int poster_num);

	//**************************************************
	/// Get Set
	//**************************************************

	int Get_numof(){ return number_of_posters; }
	int Get_number(int index);
	int Get_animation_frame(int index);

	// �ǉ����܂���1016
	const Vector3& Get_pos(int index);
	float Get_angle(int index);


};

extern Poster_manager *poster_mng;
