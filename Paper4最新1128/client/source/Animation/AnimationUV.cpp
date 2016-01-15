
#include	"AnimationUV.h"

/*
*
*	UV�A�j���[�V�����N���X
*
*/

// �ߋ��̎d�l
//AnimationUV::AnimationUV(char* name, float moveTU, float moveTV, int EndFlame, bool AlphaFlag, int AlphaNear, bool IsRoop)
//{
//	obj = new iexMesh(name);
//
//	tu = 0.0f; tv = 0.0f;
//	moveTu = moveTU, moveTv = moveTV;
//	endFlame = EndFlame;
//	nowFlame = 0;			// �����t���[����0��
//	isAction = false;		// �A�j�����s���邩
//
//	// ��
//	alphaFlag = AlphaFlag;
//	alphaNear = AlphaNear;
//	alpha = 1.0f;
//
//	isRoop = IsRoop;
//}

// �Ȉ�ver
AnimationUV::AnimationUV(char* name, float moveTU, float moveTV, int EndFlame, bool IsRoop )
{
	obj = new iexMesh(name);

	tu = 0.0f; tv = 0.0f;
	moveTu = moveTU, moveTv = moveTV;
	endFlame = EndFlame;
	nowFlame = 0;			// �����t���[����0��
	isAction = false;		// �A�j�����s���邩

	// ��
	alphaFlag = false;
	alphaNear = 0;
	alphaFar = 0;
	alpha = 1.0f;

	isRoop = IsRoop;

}

// ���l��
AnimationUV::AnimationUV(char* name, float moveTU, float moveTV, int EndFlame, bool IsRoop, int AlphaNear, int AlphaFar)
{
	obj = new iexMesh(name);

	tu = 0.0f; tv = 0.0f;
	moveTu = moveTU, moveTv = moveTV;
	endFlame = EndFlame;
	nowFlame = 0;			// �����t���[����0��
	isAction = false;		// �A�j�����s���邩

	// ��
	alphaFlag = true;
	alphaNear = AlphaNear;
	alphaFar = AlphaFar;
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
		//float A = (endFlame - nowFlame);
		//float B =(endFlame - alphaNear);
		//alpha = A / B;
		//alpha = Clamp(alpha, 0.0f, 1.0f);

		// ����A�[�̑O�����Ŕ����ς���
		if (nowFlame >= alphaNear)
		{
		// 100-100=0  100-50=50   0/50
		float A = (float)(endFlame - nowFlame);
		float B =(float)(endFlame - alphaFar);
		alpha = A / B;
		alpha = Clamp(alpha, 0.0f, 1.0f);

		}
		else
		{
		// �ŏ���
		alpha = (float)nowFlame / (float)alphaNear;//   0/30=0   60/30=2   1-(0~1)  

		}

		//alpha = (alphaFar - nowFlame) / (alphaFar - alphaNear);
		alpha = Clamp(alpha, 0.0f, 1.0f);//�w�肳�ꂽ�l�� 0 �` 1 �͈̔͂ɃN�����v���܂�

		
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
		float A = (float)(endFlame - nowFlame);
		float B = (float)(endFlame - alphaNear);
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

// �o���A�[
void AnimationUV::Render_Barrier()
{
	if (isAction == false)return;//���s����ĂȂ��Ȃ�o�Ă��I�I

	// �`��̑O�ɏ��𑗂�
	//�@�V�F�[�_�[�ɕ`�斈UV���W�𑗂�
	shaderD->SetValue("tuAnime", tu);
	shaderD->SetValue("tvAnime", tv);
	shaderD->SetValue("alphaUV", alpha);// �����x

	// �`��
	obj->Render(shaderD, "uvAnime_barrier");

}

// AT
void AnimationUV::Render_AT()
{
	if (isAction == false)return;//���s����ĂȂ��Ȃ�o�Ă��I�I

	// �`��̑O�ɏ��𑗂�
	//�@�V�F�[�_�[�ɕ`�斈UV���W�𑗂�
	shaderD->SetValue("tuAnime", tu);
	shaderD->SetValue("tvAnime", tv);
	shaderD->SetValue("alphaUV", alpha);// �����x

	// �`��
	obj->Render(shaderD, "uvAnime_at");

};