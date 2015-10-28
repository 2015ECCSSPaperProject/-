#pragma once
/*	�l�b�g�v���C���[�B	*/

/*�O���錾*/
class BasePlayer;

//********************************************************************************************************************
//		net
//****************************************************************************************************************
class	NetPlayer : public	BasePlayer
{
public:
	//==============================================================================
	// �ǉ�����
	//===============================================
	//	�萔
	//===============================================
	enum class ACTION_PART{ MOVE, MOVE_FPS, ATTACK, PASTE, REND, DIE, RESPAWN, PLANE, GUN, MAX };
	enum class DO_FLAG{ NONE, ATTACK, PASTE, REND, MAX };

protected:
	//===============================================
	//	�Ϗ��N���X�ւ̃|�C���^
	//===============================================
	//ActionBase *action[(unsigned int)ACTION_PART::MAX];
	ACTION_PART action_part;	// ���݂̃v���C���[�̃��[�h

	DO_FLAG do_flag;			// Z�L�[�������牽�����邩


	//===============================================
	//	���̑�
	//===============================================
	enum class CAMERA_MODE{ TPS, FPS };
	CAMERA_MODE		camera_mode;

public:
	NetPlayer(int id);
	~NetPlayer();
	void Initialize(iex3DObj *obj);
	void Release();

	void Update();
	void Render();



	//===========================================
	//	�ړ����(TPS)
	class ActionMove : public ActionBase<NetPlayer>
	{
	public:
		ActionMove(NetPlayer*me) :ActionBase(me){}

		void Initialize();
		void Update(const CONTROL_DESC &_ControlDesc);
	};

	//===========================================
	//	�ړ����(FPS)
	class ActionMoveFPS : public ActionBase<NetPlayer>
	{
	public:
		ActionMoveFPS(NetPlayer*me) :ActionBase(me){}

		void Initialize();
		void Update(const CONTROL_DESC &_ControlDesc);
	};

	//===========================================
	//	�U�����
	class ActionAttack : public ActionBase<NetPlayer>
	{
	public:
		ActionAttack(NetPlayer*me) :ActionBase(me){}

		void Initialize();
		void Update(const CONTROL_DESC &_ControlDesc);
	};

	//===========================================
	//	�|�X�^�[�\��t�����
	class ActionPaste : public ActionBase<NetPlayer>
	{
	private:
		int timer;
	public:
		ActionPaste(NetPlayer*me) :ActionBase(me){}

		void Initialize();
		void Update(const CONTROL_DESC &_ControlDesc);
	};

	//===========================================
	//	�|�X�^�[�j����
	class ActionRend : public ActionBase<NetPlayer>
	{
	private:
		bool rended;
	public:
		ActionRend(NetPlayer*me) :ActionBase(me){}

		void Initialize();
		void Update(const CONTROL_DESC &_ControlDesc);
	};

	//===========================================
	//	����ł���
	class ActionDie : public ActionBase<NetPlayer>
	{
	private:
		BYTE die_frame;
		BYTE flashing;

	public:
		ActionDie(NetPlayer*me) :ActionBase(me){}

		void Initialize();
		void Update(const CONTROL_DESC &_ControlDesc);
	};

	//===========================================
	//	���X�|�[�����
	class ActionRespawn : public ActionBase<NetPlayer>
	{
	private:
		BYTE invincible_time;
		BYTE flashing;

	public:
		ActionRespawn(NetPlayer*me) :ActionBase(me){}

		void Initialize();
		void Update(const CONTROL_DESC &_ControlDesc);
	};

	//===========================================
	//	���Ђ��[�����
	class ActionHikouki : public ActionBase<NetPlayer>
	{
	public:
		ActionHikouki(NetPlayer*me) :ActionBase(me){}

		void Initialize();
		void Update(const CONTROL_DESC &_ControlDesc);
	};

	//===========================================
	//	���S�C���
	class ActionGun : public ActionBase<NetPlayer>
	{
	public:
		ActionGun(NetPlayer*me) :ActionBase(me){}

		void Initialize();
		void Update(const CONTROL_DESC &_ControlDesc);
	};

	ActionBase<NetPlayer> **action;



	// �v���C���[�̃��[�h
	int Get_action()override
	{
		return (int)action_part;
	}
	void Change_action(int part) override
	{
		Change_action((ACTION_PART)part);
	}
	void Change_action(ACTION_PART part)
	{
		if (action_part > ACTION_PART::MAX)
		{
			MessageBox(0, "�T�[�o�[�̃A�N�V�����擾���̒l����������", null, MB_OK);
		}
		action_part = part;
		action[(unsigned int)part]->Initialize();
	}

	// Z�L�[�ŉ������邩
	DO_FLAG Get_do_flag(){ return do_flag; }
	void Set_do_flag(DO_FLAG flag){ do_flag = flag; }

};
