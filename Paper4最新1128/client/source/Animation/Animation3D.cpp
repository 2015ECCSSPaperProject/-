#include	"iextreme.h"
#include	"Animation3D.h"

/*�RD�A�j���[�V����*/

Animation3D::Animation3D(char* name, float XSize, float DIST, float MAX_ANIMATION, int SPEED, bool isROOP)
{
	handle = new iex2DObj(name);

	width = XSize;//�C���X�g��X�̃T�C�Y
	//Dist=DIST;//�A�j���̊Ԋu�T�C�Y
	ANIMATION_MAX = (int)MAX_ANIMATION;//�A�j���[�V������
	ANIME_SPEED = SPEED;//���x
	isAction = false;//�A�j�������s���邩

	count = 0;
	flame = 0;

	Dist = (float)DIST / width;

	isRoop = isROOP;//���[�v�������H
}

Animation3D::~Animation3D()
{
	delete handle;
}

void Animation3D::Action()
{
	isAction = true;//�N��
	count = 0;//0�ɂ��ǂ�
	flame = 0;
};

void Animation3D::Stop()
{
	isAction = false;//�~�߂�
	count = 0;//0�ɂ��ǂ�
	flame = 0;
};

void Animation3D::Update()
{
	if (isAction == false)return;//���s����ĂȂ��Ȃ�o�Ă��I�I
	
		flame++;
		if (flame >= ANIME_SPEED)
		{
			flame = 0;
			count++;
		}

		if (count == ANIMATION_MAX)
		{
			count = 0;//0�ɂ��ǂ�
			flame = 0;

			if (isRoop==false)//���[�v����Ȃ�������
			{
				isAction = false;
			}
			
		}
	
}

void Animation3D::Render(Vector3 pos, float scale, u32 dwFlags)
{
	if (isAction == false)return;//���s����ĂȂ��Ȃ�o�Ă��I�I

	float tu[2], tv[2];
	tu[0] = (Dist*count);
	tu[1] = (Dist*count) + Dist;
	tv[0] = 0.0f;
	tv[1] = 1.0f;

	Vector3 up = Vector3(0, 0, 1);
	Vector3 right = Vector3(1, 0, 0);

	//Billboard::Draw_VecBoard(up, right, pos, handle, scale, scale, tu, tv, dwFlags);
	//Billboard::Draw_MatBoard(matView, pos, handle, scale, scale, tu, tv, dwFlags);

	// �|�W�V�����@�Ɓ@�摜�@�Ɓ@�T�C�Y1.0f�@�Ɓ@0~1
	Billboard::Draw3D(pos, handle, scale, scale, tu, tv, dwFlags);

}