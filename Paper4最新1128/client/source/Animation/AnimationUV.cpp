
#include	"AnimationUV.h"

/*
*
*	UV�A�j���[�V�����N���X
*
*/

AnimationUV::AnimationUV(char* name, float moveTU, float moveTV, int EndFlame, bool AlphaFlag, int AlphaNear, bool IsRoop)
{
	obj = new iexMesh(name);

	tu = 0.0f; tv = 0.0f;
	moveTu = moveTU, moveTv = moveTV;
	endFlame = EndFlame;
	nowFlame = 0;			// �����t���[����0��
	isAction = false;		// �A�j�����s���邩

	// ��
	alphaFlag = AlphaFlag;
	alphaNear = AlphaNear;
	alpha = 1.0f;

	isRoop = IsRoop;
}

AnimationUV::~AnimationUV()
{
	delete obj;
}


void  AnimationUV::Action()
{
	isAction = true;//�N��
	tu = 0.0f; tv = 0.0f;// ������uv�ɖ߂�
	nowFlame = 0;// �ŏ��̃t���[����
}

void AnimationUV::Stop()
{
	isAction = false;//�~�߂�
	nowFlame = 0;
};


void AnimationUV::Update(Vector3 pos, Vector3 angle, float scale)
{
	if (isAction == false)return;//���s����ĂȂ��Ȃ�o�Ă��I�I

	// UV�X�V
	tu += moveTu;
	tv += moveTv;

	if (isRoop == false)//���[�v����Ȃ�������
	{
		// �t���[���X�V
		nowFlame++;
		if (nowFlame >= endFlame)
		{
			isAction = false;
		}
	}

	// �����x�X�V
	if (alphaFlag == true)
	{
		//�����_��Min~Max�͈̔͂ɗ}����@�Q�`�O
		auto Clamp = [](float val, float Min, float Max){
			return min(Max, max(val, Min));
		};

		// 0�������@1���s�����ɂ���悤�ɐݒ�
		float A = (endFlame - nowFlame);
		float B =(endFlame - alphaNear);
		alpha = A / B;
		alpha = Clamp(alpha, 0.0f, 1.0f);

	}

	// ���b�V���̍X�V
	obj->SetScale(scale);
	obj->SetAngle(angle);
	obj->SetPos(pos);
	obj->Update();


}


void AnimationUV::Update(Vector3 pos, Vector3 angle, Vector3 scale)
{
	if (isAction == false)return;//���s����ĂȂ��Ȃ�o�Ă��I�I

	// UV�X�V
	tu += moveTu;
	tv += moveTv;

	if (isRoop == false)//���[�v����Ȃ�������
	{
		// �t���[���X�V
		nowFlame++;
		if (nowFlame >= endFlame)
		{
			isAction = false;
		}
	}

	// �����x�X�V
	if (alphaFlag == true)
	{
		//�����_��Min~Max�͈̔͂ɗ}����@�Q�`�O
		auto Clamp = [](float val, float Min, float Max){
			return min(Max, max(val, Min));
		};

		// 0�������@1���s�����ɂ���悤�ɐݒ�
		float A = (endFlame - nowFlame);
		float B = (endFlame - alphaNear);
		alpha = A / B;
		alpha = Clamp(alpha, 0.0f, 1.0f);

	}

	// ���b�V���̍X�V
	obj->SetScale(scale);
	obj->SetAngle(angle);
	obj->SetPos(pos);
	obj->Update();

}

void AnimationUV::Render()
{
	if (isAction == false)return;//���s����ĂȂ��Ȃ�o�Ă��I�I

	// �`��̑O�ɏ��𑗂�
	//�@�V�F�[�_�[�ɕ`�斈UV���W�𑗂�
	shaderD->SetValue("tuAnime", tu);
	shaderD->SetValue("tvAnime", tv);
	shaderD->SetValue("alphaUV", alpha);// �����x

	// �`��
	obj->Render(shaderD, "uvAnime");

}


