#pragma once

/*	�X�P�[���𗘗p�����A�j���[�V�����@*/

class Spread2D
{
private:
	iex2DObj* handle;
	int SIZE;//�摜�̏c��
	float scale;
	float AddScale;
	D3DCOLOR col;
	D3DCOLOR StartCol;
	//�A�j���ɕK�v�ȕϐ�
	int COUNT_MAX;
	int FLAME_MAX;
	int count;
	int flame;

	bool isAction;
public:
	//�摜�̖��O�@ �摜�T�C�Y�������`����@�ő�J�E���g���@�t���[���̑��x�@���ɑ�����X�P�[�� �ŏ��̐F
	Spread2D(char* name, int XY_SIZE, int Maxcount, int Flame, float Add_Scale, D3DCOLOR Start_Col=0xffffffff);
	~Spread2D();

	void Action();

	void Update();
	void Render(float X, float Y, u32 dwFlag=RS_COPY);

};
