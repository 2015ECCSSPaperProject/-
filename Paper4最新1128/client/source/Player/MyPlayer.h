#pragma once
/*	�����̃v���C���[	*/

class Pie_graph_content;
class Spread2D;
class Rend_data;
class HitEffect;

/*�O���錾*/
//class BasePlayer;

//********************************************************************************************************************
//		����
//****************************************************************************************************************
class	MyPlayer : public	BasePlayer
{
public:
	//===============================================
	//	�������Ɖ��
	//===============================================
	MyPlayer();
	~MyPlayer();
	void Initialize(iex3DObj **obj);
	void Release();


	//SkillData
	//===============================================
	//	�X�V�ƕ`��
	//===============================================
	void Update();
	void Update_action();
	void Render(iexShader *shader = nullptr, char *name = '\0');
	void Render_forword();

	void Mouse_Update();	//	�}�E�X�̃|�W�V����
	void Control_all();		//�@������������ǉ�
	void Update_listener();	// ���X�i�[���X�V

	//===============================================
	//	�Q�b�^�[,�Z�b�^�[
	//===============================================
	void Set_action(ACTION_PART part);	// �I�[�o�[���C�h


private:
	//===============================================
	//	�j��}�E�X�̓���
	//===============================================
	Rend_data **command_data;

	// �j�����Ƃ��̃G�t�F�N�g
	HitEffect *hit_effect;

	Vector3 hit_effect_pos;

	int se_step;
	void (MyPlayer::*RendSE[12])();	// �����j��SE
	void RendPosterSE();
	void RendFlyerSE();
	void RendCalendarSE();
	void RendMagazineSE();
	void RendMoneySE();
	void RendSeisyoSE();
	void RendSignSE();
	void RendShinbunSE();
	void RendToileSE();
	void RendZasshiSE();
	void RendShojiSE();
	void RendBalloonSE();
};