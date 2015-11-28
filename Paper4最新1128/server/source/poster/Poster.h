
#pragma once

#include "iextreme.h"
#include "../../../share_data/Enum_public.h"

class Score;
class BasePlayer;

class Poster
{
private:
	Score *score;

	enum class MOTION_NUMBER
	{
		WAITE = 0,
		REND = 1
	};

	int mynumber;
	int POINT_TIME; // �|�C���g�����Z����鎞��
	int ADD_POINT; // ��x�ɉ��Z�����|�C���g
	int timer;
	int reserve_number;	// �j��郂�[�V�������I������Ƃ�������ԍ�


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
	void Do_playeraction(BasePlayer *player, int number); // �j��\��
	void Rend(int number); // �j��
	void Rend();
	void Paste(int number); // �\��

	bool Can_do(BasePlayer *player, int number); // player���|�X�^�[�ɉ����ł�����
	bool Can_do(BasePlayer *player);

	bool Can_rend(int number); // �j����
	bool Can_rend();

	bool Can_paste(int number); // �\���

	bool Can_dist(const Vector3 &pos, float dist, int number);	// vs���S�C�Ƃ��Ɏg��
	bool Can_dist(const Vector3 &pos, float dist);
	bool Check_dist(const Vector3 &pos, float dist);

	// �e�X�g
	void Change_user(int number);



	//**************************************************
	/// Get Set
	//**************************************************

	int Get_number(){ return mynumber; }
	void Set_number(int num){ mynumber = num; }
	int Get_animation_frame(){ return model->GetFrame(); }
};
