
#pragma once

#include "iextreme.h"

class BasePlayer;
class EffectCamera;

class Camera : public iexView
{
public:
	//===============================================
	//	�萔
	//===============================================
	enum MODE{ M_FIX, M_PAN, M_SLERP, M_DEBUG, M_TPS, M_FPS, M_RESPAWN, M_ZOOM, M_TARGET, M_THROUGH, M_SYURIKEN, M_MAX };
	enum class FOV_TYPE{ DEFAULT, HARD };

private:

	//===============================================
	//	��{�p�����[�^
	//===============================================
	Vector3 pos;
	Vector3 target;
	Vector3 ipos, itarget;	//���z�̈ʒu
	Vector3 angle;
	float slerp_percentage;	// �⊮����

	iexMesh *collision_stage;


	//------------------------------------------------------
	//		���e�֌W
	//------------------------------------------------------
	struct{
		float fovY, Near, Far;
	}parth;


	//===============================================
	//	�J�����Ϗ��N���X(�C���i�[�N���X)
	//===============================================
	class Mode
	{
	public:

		//===========================================
		//	���[�h���N���X
		class Base
		{
		protected:
			Camera *me;
			float dist;	// �J�������v���C���̋���
			const static float DIST;
			void Collision();

		public:
			Base(Camera* me) :me(me){}
			virtual void Initialize(const Vector3 &pos, const Vector3 &target) {}
			virtual void Update(){}
		};

		//===========================================
		//	�Œ莋�_
		class Fix : public Base
		{
		public:
			Fix(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};

		//===========================================
		//	�p��(���W�Œ�A�v���C���[�ǐ�)
		class Pan : public Base
		{
		public:
			Pan(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};

		//===========================================
		//	�⊮�J����
		class Slerp : public Base
		{
		public:
			Slerp(Camera *me) :Base(me){}

			//void Initialize(const Vector3 &Pos, const Vector3 &Target);
			void Update();
		};

		//===========================================
		//	�f�o�b�O���_���[�h
		class Debug : public Base
		{
		public:
			Debug(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};

		//===========================================
		//	3�l�̎��_���[�h
		class TPS : public Base
		{
		private:
			float iangle;
		public:
			TPS(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};

		//===========================================
		//	1�l�̎��_���[�h
		class FPS : public Base
		{
		public:
			FPS(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};

		//===========================================
		//	�~�܂��Ă��郂�[�h
		class Respawn : public Base
		{
		public:
			Respawn(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};

		//===========================================
		//	�v�`�Y�[�����[�h
		class Zoom : public Base
		{
		public:
			Zoom(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};

		//===========================================
		//	�^�[�Q�b�g���[�h
		class Target : public Base
		{
		public:
			Target(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};

		//===========================================
		//	�����Ƃ��ʂ蔲�����[�h
		class Through : public Base
		{
		public:
			Through(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};

		//===========================================
		//	�����Ƃ��ʂ蔲�����[�h
		class Syuriken : public Base
		{
		public:
			Syuriken(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};
	};

	//===============================================
	//	�Ϗ��N���X�ւ̃|�C���^
	//===============================================
	Mode::Base *mode[MODE::M_MAX];
	MODE mode_part;
	EffectCamera *effect_camera;

	//===============================================
	//	�����̃v���C���[
	//===============================================
	BasePlayer *my_player;

public:
	//------------------------------------------------------
	//	�X�N���v�g���s���
	//------------------------------------------------------
	bool scriptON;

	//===============================================
	//	�������Ɖ��
	//===============================================
	Camera();
	~Camera();

	void Initialize(BasePlayer *my);

	//===============================================
	//	�X�V�E�`��
	//===============================================
	void Update();
	void Render();

	//===============================================
	//	�Q�b�^�[,�Z�b�^�[
	//===============================================

	// ���W
	Vector3 Get_pos(){ return pos; }
	void Get_pos(Vector3 &out){ out = pos; }
	void Set_pos(const Vector3 &p){ pos = p; }

	Vector3 Get_target(){ return target; }

	// ���[�h
	MODE Get_mode(){ return mode_part; }
	void Change_mode(MODE part)
	{
		Change_mode(part, this->pos, this->target);
	}
	void Change_mode(MODE part, const Vector3 &pos, const Vector3 &target)
	{
		// ���[�h�ؑ�
		mode_part = part;
		mode[(int)part]->Initialize(pos, target);
	}

	BasePlayer *Get_my_player(){ return my_player; }


	//------------------------------------------------------
	//	Slerp�Ƀ��[�h�`�F���W����ۂɌĂяo������
	//------------------------------------------------------
	void Set_slerp(const Vector3 &Pos, const Vector3 &Target, const Vector3 &Next_pos, const Vector3 Next_target, float percentage)
	{
		pos = Pos;
		target = Target;
		ipos = Next_pos;
		itarget = Next_target;
		slerp_percentage = percentage;
	}

	void Set_slerp(Vector3 &Next_pos, const Vector3 Next_target, float percentage)
	{
		Set_slerp(pos, target, Next_pos, Next_target, percentage);
	}

};

#include	"../textLoader\textLoader.h"

class EffectCamera : public textLoader
{
private:
	enum class APPOINT
	{
		NONE,			// ���W���̂܂�
		DIRECT,			// Pos���ڎw��^
		SOME_ONE,		// �N����Pos
		SOME_ONE_COOD	// �N����Pos�{Vector3
	};

	//------------------------------------------------------
	//	�X�N���v�g�֌W
	//------------------------------------------------------
	int wait_timer;		// �R�}���h�ҋ@����
	char message[256];	// �R�}���h�̖��O�H
	unsigned long ptr;	// 

	bool In_event(char *command);
	void Out_event();
	bool Jump(char *label_name);
	void Change_camera_mode();
	void Setting_camera(Camera::MODE mode);
	void Getting_targeter(Vector3 *out);
	void Getting_targeter_coodinate(Vector3 *out);

	//------------------------------------------------------
	//	���䂷��J��������
	//------------------------------------------------------
	Camera *camera;


public:

	//------------------------------------------------------
	//	�������E���
	//------------------------------------------------------
	EffectCamera();
	~EffectCamera();
	void Initialize(Camera *me);

	//------------------------------------------------------
	//	�X�V
	//------------------------------------------------------
	void Update();

	//------------------------------------------------------
	//	�s���Z�b�g
	//------------------------------------------------------
	bool Set_pattern(int pat);
};

// 
extern Camera *camera;
