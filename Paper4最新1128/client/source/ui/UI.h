#pragma once

class BasePlayer;
class Pie_graph;
class Skill_gauge;

class UI
{
public:
	//===============================================
	//	�萔
	//===============================================
	enum IMAGE
	{
		TEROP,
		NUMBER,
		ACTION,
		TAPE,
		TAPE_BAR,
		SKILL_GUN,
		SKILL_SYURIKEN,
		SKILL_KABUTO,
		SKILL_ZENRYOKU,
		MAX
	};

private:
	//===============================================
	//	�����̎���
	//===============================================
	BasePlayer *my_player;
	iex2DObj *image[IMAGE::MAX];
	Pie_graph *graph;
	Skill_gauge *gauge;

	//===============================================
	//	���ꂼ��`��̊֐�
	//===============================================
	// �֐�����
	void Graph();		// ���ӂ̉~�O���t
	void SkillGauge();	// �����̃X�L���Q�[�W
	void Action();		// �E���̃A�N�V����UI
	void TimeLimit();	// �^�C�����~�b�g

public:
	//===============================================
	//	�������Ɖ��
	//===============================================
	UI();
	void Initialize(BasePlayer *my);
	~UI();

	//===============================================
	//	�X�V�ƕ`��
	//===============================================
	void Update();
	void Render();
};

extern UI *ui;