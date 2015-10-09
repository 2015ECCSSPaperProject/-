
#pragma once

#include "iextreme.h"
#include "../Enum_public.h"

class Score;
class Player;

class Poster
{
private:
	Score *score;

	enum class MOTION_NUMBER
	{
		WAITE = 0,
		REND = 1
	};

	TEAM_COLOR force; // ����<�`�[��>
	int POINT_TIME; // �|�C���g�����Z����鎞��
	int ADD_POINT; // ��x�ɉ��Z�����|�C���g
	int timer;

	iex3DObj *model; // 3D���f��
	Vector3 position; // �ʒu
	Vector3 forward; // �|�X�^�[�̌���<�\>
	float angle; // ������ model �ɓn���悤

	struct 
	{
		float min_y, // ������
			max_y; // �����
		float wide; // ���E
		float forward; // �O��
	}range;

	enum class MODE
	{
		WAITE,
		REND,
		MAX
	}mode;
	// ���[�h
	class Mode
	{
	protected:
		Poster *me;
	public:
		Mode(Poster *me) : me(me){}
		virtual ~Mode(){}
		virtual void Initialize(){}
		virtual void Update(){}
		virtual void Render(){}
	}*mode_list[(int)MODE::MAX];
	// �ҋ@��
	class Mode_waite : public Mode
	{
	public:
		Mode_waite(Poster *me) : Mode(me){}
		void Initialize() override;
		void Update() override;
		void Render() override;
	};
	// �j���Ă�
	class Mode_rend : public Mode
	{
	public:
		Mode_rend(Poster *me) :Mode(me){}
		void Initialize() override;
		void Update() override;
		void Render() override;
	};

	void Change_mode(MODE mode);
public:
	Poster();
	~Poster();

	void Initialize(iex3DObj *model, Score *score, int point);
	void Release();
	void Update();
	void Render();

	// �Q�b�^�[�A�Z�b�^�[
	Vector3& Get_pos(){ return position; }

	float Get_angle(){ return angle; }

	void Set_pose(const Vector3& forward, const Vector3& pos); // �p�����v�Z����
	void Set_pose(const float angle, const Vector3& pos); // �p�����v�Z����

	// �v���C���[����̃A�N�V����
	void Do_playeraction(Player *player, TEAM_COLOR color, Texture2D *tex); // �j��\��
	void Rend(Player *player, TEAM_COLOR color, Texture2D *tex); // �j��
	void Paste(Player *player, TEAM_COLOR color, Texture2D *tex); // �\��

	bool Can_do(Player *player, TEAM_COLOR color); // player���|�X�^�[�ɉ����ł�����

	// �e�X�g
	void Change_color(TEAM_COLOR color, Texture2D *tex);
};
