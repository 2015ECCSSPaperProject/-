#include	"iextreme.h"
#include	"Spread2D.h"

/*	�X�P�[���𗘗p�����A�j���[�V�����@*/

Spread2D::Spread2D(char* name, int XY_SIZE, int Maxcount, int speed, float Add_Scale, D3DCOLOR Start_Col)
{
	handle = new iex2DObj(name);

	COUNT_MAX = Maxcount;
	FLAME_MAX = speed;
	AddScale = Add_Scale;
	SIZE = XY_SIZE;
	//������
	scale = 1.0f;//�����X�P�[��
	StartCol = Start_Col;
	col = StartCol;//�����̐F
	count = 0;//0�ɂ��ǂ�
	flame = 0;
	isAction = false;//�A�j�������s����X�C�b�`

}

Spread2D::~Spread2D()
{
	delete handle;
}

void Spread2D::Action()
{
	isAction = true;//�N��

	scale = 1.0f;//�����X�P�[��
	col = StartCol;//�����̐F
	count = 0;//0�ɂ��ǂ�
	flame = 0;
};

void Spread2D::Update()
{
	if (isAction == false)return;//���s����ĂȂ��Ȃ�o�Ă��I�I

	flame++;
	if (flame >= FLAME_MAX)
	{
		flame = 0;
		count++;

		scale += AddScale;//�g��
		col -= 0x11000000;//�����ɂ��Ă���
	}

	if (count == COUNT_MAX)
	{
		count = 0;//0�ɂ��ǂ�
		flame = 0;
		isAction = false;
	}

};

void Spread2D::Render(float X, float Y, u32 dwFlag)
{
	if (isAction == false)return;//���s����ĂȂ��Ȃ�o�Ă��I�I

	handle->Render_scale(scale, (int)X, (int)Y, SIZE, SIZE, 0, 0, SIZE, SIZE, dwFlag, col);

};