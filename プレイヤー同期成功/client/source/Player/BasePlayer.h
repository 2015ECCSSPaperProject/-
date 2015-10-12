#pragma once

/*	�x�[�X�v���C���[	*/

class BasePlayer
{
public:

	//	����f�X�N
	struct	CONTROL_DESC{

		BYTE moveFlag;			//	���C�t���O

	};
	CONTROL_DESC m_controlDesc;//�@�R���g���[��

protected:

	iexMesh2* obj;
	Vector3 pos;
	Vector3 move;



public:
	BasePlayer();
	~BasePlayer();

	virtual void Update();
	virtual void Render();

	//�Q�b�^�[�E�Z�b�^�[
	Vector3 GetPos(){ return pos; };
	void SetPos(Vector3 p){ pos = p; }
	const CONTROL_DESC& GetDesc()const{ return	m_controlDesc; }
};

extern BasePlayer* player[PLAYER_MAX];