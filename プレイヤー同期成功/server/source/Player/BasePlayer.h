#pragma once

/*	�x�[�X�v���C���[	*/

class BasePlayer
{
protected:

	iexMesh* obj;
	Vector3 pos;
	Vector3 move;



public:
	BasePlayer();
	~BasePlayer();

	virtual void Update();
	virtual void Render();

	//�Q�b�^�[�E�Z�b�^�[
	Vector3 GetPos(){ return pos; };

};

extern BasePlayer* player[PLAYER_MAX];