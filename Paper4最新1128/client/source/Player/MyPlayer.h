#pragma once
/*	�����̃v���C���[	*/

class Pie_graph_content;
class Spread2D;

/*�O���錾*/
//class BasePlayer;

//********************************************************************************************************************
//		����
//****************************************************************************************************************
class	MyPlayer : public	BasePlayer
{
private:
	int se_step;
	void (MyPlayer::*RendSE[10])();	// �����j��SE
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

	void Mouse_Update();	//	�}�E�X�̃|�W�V����
	void Control_all();		//�@������������ǉ�
	void Update_listener();	// ���X�i�[���X�V

	//===============================================
	//	�Q�b�^�[,�Z�b�^�[
	//===============================================
	void Set_action(ACTION_PART part);	// �I�[�o�[���C�h
};