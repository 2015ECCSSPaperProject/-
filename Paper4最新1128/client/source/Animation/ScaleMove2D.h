#pragma once

/*	�X�P�[���𗘗p�����A�j���[�V�����@*/

class ScaleMove2D
{
private:
	iex2DObj* handle;
	float scale;				// ���ۂɉ摜�ŏo���X�P�[��
	float default_scale;		// ��{�X�P�[��(�ϓ����钆�S)
	float fluctuation_range;	// �ϓ���
	float add_scale;			// ������
	enum ADD_FLAG
	{
		NONE,		// �������Ȃ�
		INCREASE,	// ���₷
		DECREASE	// ���炷
	}add_flag;
public:
	//�摜�̖��O�@ ��{�摜�T�C�Y�@�ϓ����@���ɑ�������X�P�[��
	ScaleMove2D(char* name, float DefaultScale, float FluctuationRange, float AddScale);
	~ScaleMove2D();
	void Action();
	void Stop();
	void Update();
	void Render(int dstX, int dstY, u32 dwFlag = RS_COPY);

	void SetDefaultScale(float s){ default_scale = s; }
	void SetFluctuationRange(float f){ fluctuation_range = f; }
	void SetAddScale(float s){ add_scale = s; }
};
