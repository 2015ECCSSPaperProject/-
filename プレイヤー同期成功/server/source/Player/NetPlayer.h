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

public:
	NetPlayer(int id);
	~NetPlayer();
	void Initialize(iexMesh *obj);
	void Update();
	void Render();
};
