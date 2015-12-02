#pragma once
/*�RD�A�j���[�V�����N���X*/

class Animation3D
{
private:
	iex2DObj* handle;

	float width;//�C���X�g��X�̃T�C�Y
	float Dist;//�A�j���̊Ԋu�T�C�Y
	int ANIMATION_MAX;//�A�j���[�V������
	bool isAction;//�A�j�������s���邩

	int count;
	int flame;
	int ANIME_SPEED;//�X�s�[�h

	bool isRoop;//���[�v���Ă��邩�H

public:
	//�摜�̖��O�@���̉摜��X���@�؂���T�C�Y�@�A�j���[�V�������@�A�j�����x�@���[�v���邩
	Animation3D(char* name, float XSize, float DIST, float MAX_ANIMATION, int SPEED, bool isRoop=false);
	~Animation3D();

	void Action();//�A�j�����s
	void Stop();//�A�j���~�߂�

	void Update();
	void Render(Vector3 pos, float scale = 1.0f, u32 dwFlags=RS_ADD);
};

