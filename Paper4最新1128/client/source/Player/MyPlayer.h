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
public:
	//===============================================
	//	�������Ɖ��
	//===============================================
	MyPlayer();
	~MyPlayer();
	//void Initialize(iex3DObj *obj, iex3DObj *die);
	void Release();


	//SkillData
	//===============================================
	//	�X�V�ƕ`��
	//===============================================
	void Update();
	void Render(iexShader *shader = nullptr, char *name = '\0');

	void Mouse_Update();	//	�}�E�X�̃|�W�V����
	void Control_all();		//�@������������ǉ�
	void Update_listener();	// ���X�i�[���X�V

	//===============================================
	//	�Q�b�^�[,�Z�b�^�[
	//===============================================


private:

	//Pie_graph_content* skillGage;
	



};