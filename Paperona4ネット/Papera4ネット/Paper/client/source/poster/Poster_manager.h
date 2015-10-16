
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
class BasePlayer;

class Poster_manager
{
private:
	Texture2D *poster_textures[(int)TEAM_COLOR::NONE]; // �|�X�^�[�̊G
	iex3DObj *poster_model; // �N���[���֐��g���p
	int number_of_posters;
	std::vector<Poster*> posters; // �|�X�^�[
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
	/// �v���C���[����̃A�N�V����
	//**************************************************

	// �j���A�\���|�X�^�[�̔ԍ�
	int Can_do(BasePlayer *player, TEAM_COLOR color);
	
	// Can_do�̌�ŌĂ�
	bool Can_rend(TEAM_COLOR color, int poster_num);          bool Can_paste(TEAM_COLOR color, int poster_num);

	// �j��\��
	void Rend_poster(TEAM_COLOR color, int poster_num);
	void Paste_poster(TEAM_COLOR color, int poster_num);

	//**************************************************
	/// Get Set
	//**************************************************

	int Get_numof(){ return number_of_posters; }
	void Set_color(int index, TEAM_COLOR color);
	void Set_animframe(int index, int frame);
};

extern Poster_manager *poster_mng;
