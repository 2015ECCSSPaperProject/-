
#pragma once

#include "iextreme.h"

class BasePlayer;

class Camera : public iexView
{
public:

	//===============================================
	//	�萔
	//===============================================
	enum MODE_PART{ M_DEBUG, M_TPS, M_FPS, M_STOP, M_ZOOM, M_MAX };


private:

	//===============================================
	//	��{�p�����[�^
	//===============================================
	Vector3 pos;
	Vector3 target;
	Vector3 ipos, itarget;	//���z�̈ʒu
	Vector3 angle;

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
			virtual void Initialize(){}
			virtual void Update(){}
		};

		//===========================================
		//	�f�o�b�O���_���[�h
		class Debug : public Base
		{
		public:
			Debug(Camera *me) :Base(me){}

			void Initialize();
			void Update();
		};

		//===========================================
		//	3�l�̎��_���[�h
		class TPS : public Base
		{
		public:
			TPS(Camera *me) :Base(me){}

			void Initialize();
			void Update();
		};

		//===========================================
		//	1�l�̎��_���[�h
		class FPS : public Base
		{
		public:
			FPS(Camera *me) :Base(me){}

			void Initialize();
			void Update();
		};

		//===========================================
		//	�~�܂��Ă��郂�[�h
		class Stop : public Base
		{
		public:
			Stop(Camera *me) :Base(me){}

			void Initialize();
			void Update();
		};

		//===========================================
		//	�v�`�Y�[�����[�h
		class Zoom : public Base
		{
		public:
			Zoom(Camera *me) :Base(me){}

			void Initialize();
			void Update();
		};
	};

	//===============================================
	//	�Ϗ��N���X�ւ̃|�C���^
	//===============================================
	Mode::Base *mode[MODE_PART::M_MAX];
	MODE_PART mode_part;


	//===============================================
	//	�����̃v���C���[
	//===============================================
	BasePlayer *my_player;

public:
	//===============================================
	//	�������Ɖ��
	//===============================================
	Camera();
	~Camera();

	void Initialize(BasePlayer *my);

	//===============================================
	//	�X�V
	//===============================================
	void Update();


	//===============================================
	//	�Q�b�^�[,�Z�b�^�[
	//===============================================

	// ���W
	Vector3 Get_pos(){ return pos; }
	void Get_pos(Vector3 &out){ out = pos; }
	void Set_pos(const Vector3 &p){ pos = p; }

	// ���[�h
	MODE_PART Get_mode(){ return mode_part; }
	void Change_mode(MODE_PART part)
	{
		// ���[�h�ؑ�
		mode_part = part;
		mode[part]->Initialize();
	}



};

// 
extern Camera *camera;
