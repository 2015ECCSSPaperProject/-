
#pragma once

// #include "../../../../share_data/Enum_public.h"

class BasePlayer;

class Poster : public Paper_obj_Mesh
{
private:
	enum class MOTION_NUMBER
	{
		WAITE = 0,
	};

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

	void Change_mode(MODE mode);

public:
	Poster();
	Poster( int model_type, iexMesh *model, int point );
	~Poster();

	void Initialize(int model_type, iexMesh *model, int point);
	void Release();
	void Update();
	void Render();

	// �Q�b�^�[�A�Z�b�^�[
	const Vector3& Get_pos(){ return position; }

	float Get_angle(){ return angle; }

	void Set_pose(const Vector3& forward, const Vector3& pos); // �p�����v�Z����
	void Set_pose(const float angle, const Vector3& pos); // �p�����v�Z����

	/* �v���C���[����̃A�N�V���� */

	// player���|�X�^�[�ɉ����ł�����
	bool Can_do(BasePlayer *player);

	// �j����
	bool Can_rend();

	// vs���S�C�Ƃ��Ɏg��
	bool Can_dist(const Vector3 &pos, float dist);
	bool Check_dist(const Vector3 &pos, float dist);

	// �e�X�g
	int Get_animation_frame(){ return 0; }

	//**************************************************

	unsigned int Get_send_data_size();

	void Get_send_data( char *out );

	virtual KIND_PAPER_OBJECT Get_kind();
};



// �|�X�^�[���p�������N���X

class Calendar : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Magazin : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Money : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Seisho : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Shinbun : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Sign : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Toilet_paper : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Zasshi : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Shoji : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};
