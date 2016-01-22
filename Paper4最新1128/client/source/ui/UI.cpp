#include "iextreme.h"
#include "../Player/BasePlayer.h"
#include "../../../share_data/Enum_public.h"
#include "../Scene/SceneMain.h"
#include "../pie_graph/pie_graph.h"
#include "UI.h"
#include "../paper object/paper object manager.h"
#include "../timer/Timer.h"
#include "../score/Score.h"
#include "../skill_gauge/skill_gauge.h"
#include <assert.h>
#include "../sound/SoundManager.h"
#include "../Animation/AnimationRippleEx.h"
#include "../camera/Camera.h"
#include "../../IEX/OKB.h"
#include "../Scene/SceneSelect.h"

float UI::tape_len;

void UI::Change_mode(int m)
{
	SAFE_DELETE(mode);
	switch (m)
	{
	case (int)SceneMain::MODE::START:
		mode = new Mode::Start(this);
		break;

	case (int)SceneMain::MODE::MAIN:
		mode = new Mode::Main(this);
		break;

	case (int)SceneMain::MODE::END:
		mode = new Mode::End(this);
		break;
	}
	mode->Initialize();
}

UI::UI() :my_player(nullptr), graph(nullptr), gauge(nullptr), mode(nullptr), isYooiDon(false), telopID(-1), seted_ID(-1), m_fade_type(MANHOLE_FADE_TYPE::NONE), manhole_f_scale(1)
{
	tape_len = 0;
	for (int i = 0; i < IMAGE::MAX; i++)image[i] = nullptr;
	fadeM_funk[(int)MANHOLE_FADE_TYPE::NONE] = &UI::fadeM_none;
	fadeM_funk[(int)MANHOLE_FADE_TYPE::F_IN] = &UI::fadeM_in;
	fadeM_funk[(int)MANHOLE_FADE_TYPE::F_OUT] = &UI::fadeM_out;
}

void UI::Initialize(BasePlayer *my)
{
	// �����̃v���C���[���w������
	my_player = my;

	// �X�L���Q�[�W���� ��
	gauge = new Pie_graph_content("DATA/UI/skill/skill_gage_n.png");	//	Skile�Q�[�W
	//gauge->Initialize();
	
	// �X�L�����܂����炳��
	for (int i = 0; i < SKILL_MAX; i++)
	{
		SkileSphere[i] = new AnimationRippleEx("DATA/UI/skill/sphere_n.png",
			12, 1, 2, 0.5f, +(1.5f / 12.0f), false);

		savePercent[i] = 1.0f;
	}
	SkileRip[0] = new AnimationRippleEx("DATA/UI/skill/skill_gage1.png",
		18, 1, 6, 1.0f, +(1.5f / 18.0f), false);
	SkileRip[1] = new AnimationRippleEx("DATA/UI/skill/skill_gage2.png",
		18, 1, 6, 1.0f, +(1.5f / 18.0f), false);
	SkileRip[2] = new AnimationRippleEx("DATA/UI/skill/skill_gage3.png",
		18, 1, 6, 1.0f, +(1.5f / 18.0f), false);




	// �~�O���t����
	graph = new Pie_graph;
	graph->Add_content("DATA/UI/graph/red.png");
	graph->Add_content("DATA/UI/graph/blue.png");
	graph->Add_content("DATA/UI/graph/yellow.png");
	graph->Add_content("DATA/UI/graph/green.png");
	graph->Add_content("DATA/UI/graph/purple.png");
	graph->Add_content("DATA/UI/graph/pink.png");

	// ���̑�2D������
	//image[IMAGE::TEROP] = new iex2DObj("")
	image[IMAGE::ACTION] = new iex2DObj("DATA/UI/action/1.png");
	image[IMAGE::NUMBER] = new iex2DObj("DATA/UI/Num.png");
	image[IMAGE::NUMBER_BACK] = new iex2DObj("DATA/UI/NumBack.png");
	image[IMAGE::TAPE] = new iex2DObj("DATA/UI/tape/tape.png");
	image[IMAGE::TAPE_BAR] = new iex2DObj("DATA/UI/tape/tape2.png");
	image[IMAGE::SKILL_GUN] = new iex2DObj("DATA/UI/skill/skill1.png");
	image[IMAGE::SKILL_SYURIKEN] = new iex2DObj("DATA/UI/skill/skill2.png");
	image[IMAGE::SKILL_KABUTO] = new iex2DObj("DATA/UI/skill/skill3.png");
	image[IMAGE::SKILL_FRAME] = new iex2DObj("DATA/UI/skill/skill_gage_frame.png");		// �X�L���̃t���[��
	image[IMAGE::SKILL_SELECT] = new iex2DObj("DATA/UI/skill/skilieSelect2.png");		// skilieSelect
	image[IMAGE::SKILL_ROGO] = new iex2DObj("DATA/UI/skill/skill_rogo.png");		// skill_rogo
	image[IMAGE::SCORE_FRAME] = new iex2DObj("DATA/UI/skill/scoreFrame.png");		// scoreFrame
	image[IMAGE::MANHOLE_FADE] = new iex2DObj("DATA/UI/manhole_fade.png");
	image[IMAGE::ARROW_LEFT] = new iex2DObj("DATA/UI/action/Left_Cursor.png");
	image[IMAGE::ARROW_RIGHT] = new iex2DObj("DATA/UI/action/Cursor.png");
	image[IMAGE::ARROW_UP] = new iex2DObj("DATA/UI/action/Up_Cursor.png");
	image[IMAGE::ARROW_DOWN] = new iex2DObj("DATA/UI/action/Down_Cursor.png");
	image[IMAGE::ARROW_ROLL] = new iex2DObj("DATA/UI/action/rot_Cursor.png");
	image[IMAGE::MARK] = new iex2DObj("DATA/Camera/mark1.png");
	image[IMAGE::MARK]->SetScale(4.0f);
	image[IMAGE::ACTION_HOLD] = new iex2DObj("DATA/UI/action/�z�[���h�I.png");
	image[IMAGE::ACTION_DRAG] = new iex2DObj("DATA/UI/action/�h���b�O.png");
	action_hold = new AnimationRippleEx("DATA/UI/action/�z�[���h�I.png",
		40, 24, 32, 12.2f, (12.2f / 40.0f), true);
	action_drag = new AnimationRippleEx("DATA/UI/action/�h���b�O.png",
		40, 24, 32, 12.2f, (12.2f / 40.0f), true);

	// �h���b�O���b�v
	DragRip = new AnimationRippleEx("DATA/UI/action/�h���b�O.png",
		8, 1, 2, 7.0f, +(3.5f / 8.0f), false);

	// �X�[�p�[���c�^�C��
	C_Five = new AnimationRippleEx("DATA/UI/call/five.png",
		60, 24, 52, 1.2f, -(0.2f / 60.0f), false);
	C_Four = new AnimationRippleEx("DATA/UI/call/four.png",
		60, 24, 52, 1.2f, -(0.2f / 60.0f), false);
	C_Three = new AnimationRippleEx("DATA/UI/call/three.png",
		60, 24, 52, 1.2f, -(0.2f / 60.0f), false);
	C_Two = new AnimationRippleEx("DATA/UI/call/two.png",
		60, 24, 52, 1.2f, -(0.2f / 60.0f), false);
	C_One = new AnimationRippleEx("DATA/UI/call/one.png",
		60, 24, 52, 1.2f, -(0.2f / 60.0f), false);
	CountTimeNo = -1;

	TimerX = 1280;			//Timer�𓮂���
	SkillX = -600;			//SKILL�𓮂���
	GraphX = -400;			//GRHPH�𓮂���
	Change_mode(SceneMain::MODE::START);
}

UI::~UI()
{
	for (int i = 0; i < IMAGE::MAX; i++)SAFE_DELETE(image[i]);
	delete graph;
	delete gauge;
	SAFE_DELETE(mode);
	for (auto it : List) delete it;
	List.clear();

	// �X�L�����܂����炳��
	for (int i = 0; i < SKILL_MAX; i++)
	{
		delete	SkileSphere[i];
		delete SkileRip[i];
	}

	// �J�E���g�_�E��
	delete	C_Five;
	delete	C_Four;
	delete	C_Three;
	delete	C_Two;
	delete	C_One;

	delete action_drag;
	delete action_hold;
	delete DragRip;

}

void UI::Update()
{
	mode->Update();

	// �A�N�V�����{�^���A�j���[�V����
	//action_hold->Update();
	//action_drag->Update();
}

void UI::Render()
{
	mode->Render();

#ifdef _DEBUG
	Text::Draw(32, 480, 0xff00ffff, "%.1f", my_player->Get_angleY());
	Text::Draw(32, 520, 0xff00ffff, "%.1f", my_player->Get_pos().x);
	Text::Draw(32, 560, 0xff00ffff, "%.1f", my_player->Get_pos().y);
	Text::Draw(32, 600, 0xff00ffff, "%.1f", my_player->Get_pos().z);
#endif
}

///////////////////aaaaaaaaaaaaaaaaaaa
void UI::Render_mark()
{
	// �����_�[�X�e�[�g
 	iexSystem::Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);//������False�ɂ����
	iexSystem::Device->SetRenderState(D3DRS_ZENABLE, FALSE);//������False�ɂ����

	// ���X�V
	
	//float fogRate = (FogFar - pos.z) / (FogFar - FogNear);
	//fogRate = saturate(fogRate);//�w�肳�ꂽ�l�� 0 �` 1 �͈̔͂ɃN�����v���܂�


	float alphaRate ;
	float alphaNear= 150;//�����̉����鋗��
	float alphaFar = 700;//�����������鋗��

	bool isRange = false;
	for (int i = 0; i < paper_obj_mng->Get_numof(); i++)
	{

		/*****************************/
		//		��
		/*****************************/
		Vector3 lenVec = camera->Get_pos() - paper_obj_mng->Get_pos(i);		// ���̋���
		alphaRate = (alphaFar - lenVec.Length()) / (alphaFar - alphaNear);
		alphaRate = alphaRate * 255;
		//�����_��Min~Max�͈̔͂ɗ}����@�Q�`�O
		auto Clamp = [](float val, float Min, float Max){
			return min(Max, max(val, Min));
		};
		alphaRate = Clamp(alphaRate, 0, 255);
		image[IMAGE::MARK]->SetARGB((int)alphaRate, 255, 255, 255);
		/************************************************************/

		if (!paper_obj_mng->Can_rend(i))continue;
		float tu[2], tv[2];
		if (my_player->Get_poster_num() == i)
		{
			static int add = 10;
			static int MinusCol = 0;
			//�A�b�v�ƃ_�E��
			if (MinusCol >= 255)
			{
				add = -10;
			}
			if (MinusCol <= 0)
			{
				add = 10;
			}
			MinusCol += add;

			//���@�z�[���h��
			//�A�b�v�ƃ_�E��
			static float h_add = 0;
			static float h_moveY = 0;

			if (h_moveY >= 2)
			{
				h_add = -0.2f;
			}
			if (h_moveY <= 0)
			{
				h_add = 0.2f;
			}
			h_moveY += h_add;

			//���@�z�[���h��
			//�A�b�v�ƃ_�E��
			static float d_moveX = -4.3f;
			d_moveX += 0.3f;
			if (d_moveX >= 5)
			{
				d_moveX = -4.3f;
			}

			if (my_player->Get_action() == BasePlayer::ACTION_PART::REND_OBJ)
			{
				//action_drag->Stop();
				//action_hold->Stop();
				continue;
			}
			else if (my_player->Get_action() == BasePlayer::ACTION_PART::REND
				|| my_player->Get_controlDesc().controlFlag & (int)PLAYER_CONTROL::LEFT_CLICK
				)
			{
				if (KeyBoardTRG(MOUSE_LEFT))
				{
					DragRip->Action();
				}
				
				// �������@�h���b�O�I�I

				//Vector3 p_pos = my_player->Get_pos();
				//image[IMAGE::ACTION_DRAG]->SetScale(7.0f);
				//image[IMAGE::ACTION_DRAG]->Render3D(p_pos + Vector3(0, 20, 0));
				//image[IMAGE::ACTION]->SetARGB(255, 200, MinusCol, MinusCol);
				//image[IMAGE::ACTION]->Render3D(p_pos + Vector3(0, 20, 0));
				//image[IMAGE::ARROW_RIGHT]->SetScale(5.0f);
				//image[IMAGE::ARROW_RIGHT]->Render3D(p_pos + Vector3(0, 20, 0));

				// �����ꏉ���̂���u
				tu[0] = 0, tu[1] = .5f;
				tv[0] = 0, tv[1] = 1;
				//Billboard::Draw3D(paper_obj_mng->Get_pos(i) + Vector3(0, 24, 0), image[IMAGE::MARK], 4, 4, tu, tv, RS_COPY);
				image[IMAGE::MARK]->Render3D(paper_obj_mng->Get_pos(i)+ Vector3(0, 24, 0));

				iexSystem::Device->SetRenderState(D3DRS_ZENABLE, FALSE);//������False�ɂ����
				// ����B�܂�����
				Vector3 p_pos = my_player->Get_pos();
				image[IMAGE::ACTION_DRAG]->SetScale(7.0f);
				image[IMAGE::ACTION_DRAG]->Render3D(paper_obj_mng->Get_pos(i) + Vector3(0, 24, 0));
				DragRip->Update();
				DragRip->Render(paper_obj_mng->Get_pos(i) + Vector3(0, 24, 0), RS_ADD);					// �g��
				image[IMAGE::ACTION]->SetARGB(255, 200, MinusCol, MinusCol);
				//image[IMAGE::ACTION]->SetAngle(2);
				image[IMAGE::ACTION]->SetScale(3.0f);


				Vector3 ViewVec;
				ViewVec.x = matView._11;
				ViewVec.y = matView._21;
				ViewVec.z = matView._31;

				Vector3 ViewVecZ;
				ViewVecZ.x = matView._13;
				ViewVecZ.y = matView._23;
				ViewVecZ.z = matView._33;
				ViewVecZ.Normalize();

				ViewVec.Normalize();
				image[IMAGE::ACTION]->Render3D(paper_obj_mng->Get_pos(i) + (ViewVec * d_moveX) + Vector3(0, 24, 0) + ViewVecZ * -1);
		
				iexSystem::Device->SetRenderState(D3DRS_ZENABLE, TRUE);//������False�ɂ����

			}
			else
			{	
				//��������@X�̃h���b�O
				d_moveX = -4.3f;
				// �������@�z�[���h�I�I�I�I�I

				// �����ꏉ���̂���u
				tu[0] = 0, tu[1] = .5f;
				tv[0] = 0, tv[1] = 1;
				//Billboard::Draw3D(paper_obj_mng->Get_pos(i) + Vector3(0, 24, 0), image[IMAGE::MARK], 4, 4, tu, tv, RS_COPY);
				image[IMAGE::MARK]->Render3D(paper_obj_mng->Get_pos(i) + Vector3(0, 24, 0));


				iexSystem::Device->SetRenderState(D3DRS_ZENABLE, FALSE);//������False�ɂ����

				// ����B�܂�����
				Vector3 p_pos = my_player->Get_pos();
				image[IMAGE::ACTION_HOLD]->SetScale(7.0f);
				image[IMAGE::ACTION_HOLD]->Render3D(paper_obj_mng->Get_pos(i) + Vector3(0, 24 + h_moveY, 0));
				image[IMAGE::ACTION]->SetARGB(255, 200, MinusCol, MinusCol);
				//image[IMAGE::ACTION]->SetAngle(2);
				image[IMAGE::ACTION]->SetScale(3.0f);

				Vector3 ViewVec;
				ViewVec.x = matView._11;
				ViewVec.y = matView._21;
				ViewVec.z = matView._31;
				ViewVec.Normalize();
				image[IMAGE::ACTION]->Render3D(paper_obj_mng->Get_pos(i) + (ViewVec * -4.2f) + Vector3(0, 24, 0));
				
				iexSystem::Device->SetRenderState(D3DRS_ZENABLE, TRUE);//������False�ɂ����


				//image[IMAGE::ARROW_RIGHT]->SetScale(5.0f);
				//image[IMAGE::ARROW_RIGHT]->Render3D(p_pos + Vector3(20, 20, 0));
			


			}
		}
		else
		{
			// �����ꏉ���̂���u
			tu[0] = 0, tu[1] = .5f;
			tv[0] = 0, tv[1] = 1;
			//Billboard::Draw3D(paper_obj_mng->Get_pos(i) + Vector3(0, 24, 0), image[IMAGE::MARK], 4, 4, tu, tv, RS_COPY);
			//image[IMAGE::MARK]->Render3D(paper_obj_mng->Get_pos(i) + Vector3(0, 24, 0))
			image[IMAGE::MARK]->Render3D(paper_obj_mng->Get_pos(i) + Vector3(0, 24, 0));

		}
	}
	//if (!isRange)
	//{
	//	action_drag->Stop();
	//	action_hold->Stop();
	//}


	// �����_�[�X�e�[�g
	iexSystem::Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);//������False�ɂ����
	iexSystem::Device->SetRenderState(D3DRS_ZENABLE, TRUE);//������False�ɂ����


}

//*****************************************************************************************************************************
//
//		��{�A�����͂�����

void UI::Mode::Main::Update()
{
	const int second = timer->Get_limit_time() % 60, minutes = timer->Get_limit_time() / 60;
	// �c��T�b�O�I
	if (minutes == 0 && second == 5){
		if (me->CountTimeNo != 5){
			me->CountTimeNo = 5;	me->C_Five->Action();
		}
	}
	// �c��4�b�O�I
	if (minutes == 0 && second == 4)
	{
		if (me->CountTimeNo != 4){
			me->CountTimeNo = 4;	me->C_Four->Action();
		}
	}
	// �c��3�b�O�I
	if (minutes == 0 && second == 3)
	{
		if (me->CountTimeNo != 3){
			me->CountTimeNo = 3;	me->C_Three->Action();
		}
	}
	// �c��2�b�O�I
	if (minutes == 0 && second == 2)
	{
		if (me->CountTimeNo != 2){
			me->CountTimeNo = 2;	me->C_Two->Action();
		}
	}
	// �c��1�b�O�I
	if (minutes == 0 && second == 1){
		if (me->CountTimeNo != 1){
			me->CountTimeNo = 1;	me->C_One->Action();
		}
	}


	// �J�E���g�_�E���X�V
	me->C_Five->Update();
	me->C_Four->Update();
	me->C_Three->Update();
	me->C_Two->Update();
	me->C_One->Update();

	//// �X�L�����܂�����g��̍X�V
	//for (int i = 0; i < SKILL_MAX; i++)
	//{
	//	me->SkileSphere[i]->Update();
	//}
}

void UI::Mode::Main::Render()
{
	if (!SCOREATACK_FLAG)me->Graph();
	me->SkillGauge();
	me->Action();
	me->Telop_render();
	me->TimeLimit();
	//me->Command();
	me->Manhole_fade();
}


void UI::Telop_render()
{
	// �e���b�v�̃e�[�v
	int len = (int)(1024 * tape_len);
	image[IMAGE::TAPE_BAR]->Render(TimerX + 250 + (1024 - len) - 1092, 12, len, 128, 0, 0, 1024, 128);
	image[IMAGE::TAPE]->Render(TimerX + 1048 - 1092, 12, 256, 128, 0, 0, 256, 128);

	if (telopID != seted_ID)
	{
		seted_ID = telopID;
		Append_telop(telopID);
	}

	// �e���b�v���X�g�ɉ��������Ă�����
	if (!List.empty())
	{
		std::list<Telop*>::iterator it = List.begin();
		while (it != List.end())
		{
			(*it)->Update();
			(*it)->Render();
			if ((*it)->erase)
			{
				delete (*it);
				it = List.erase(it);
			}
			else it++;
		}
	}
}

// �X�R�A�t���O
void UI::Graph()
{
	// �ŏ��ɉ�ʊO����łĂ��邺�����������I�I�I
	const int kijun = 0;
	if (GraphX <= kijun)
	{
		GraphX += 14;
	}

	// ���̃t���[��
	image[IMAGE::SCORE_FRAME]->Render(5 + GraphX, 5);

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		graph->Set_percent(i, (float)score_mng->Get(i));
	}
	graph->Render(69 + GraphX, 68, 128, 128, 0, 0, 256, 256);
}

static int SKILL_Y = 32;
void UI::SkillGauge()
{
	// �X�L��UI
	//image[IMAGE::SKILL_GUN]->Render(28, 560, 32, 32, 0, 0, 32, 32);
	//image[IMAGE::SKILL_SYURIKEN]->Render(76, 560, 32, 32, 0, 0, 32, 32);
	//image[IMAGE::SKILL_KABUTO]->Render(124, 560, 32, 32, 0, 0, 32, 32);
	//image[IMAGE::SKILL_ZENRYOKU]->Render(172, 560, 32, 32, 0, 0, 32, 32);

	//int gage_val = my_player->Get_god_gage() / 10;	// �X�L���Q�[�W�擾
	//gauge->Render(gage_val, 10);

	// �ŏ��ɉ�ʊO����łĂ��邺�����������I�I�I
	const int kijun = 0;
	if (SkillX <= kijun)
	{
		SkillX += 14;
	}

	// SKILL_���S
	image[IMAGE::SKILL_ROGO]->Render(SkillX, (180 + SKILL_Y));

	//�~�Q�[�W
	for (int i = 0; i < (int)BasePlayer::SKILL::MAX; i++)
	{
		DWORD col;
		float percent;

		// �X�L���̃t���[���ǉ�
		image[IMAGE::SKILL_FRAME]->Render(SkillX + (i * (80)), (200 + SKILL_Y));

		// �Q�[�W�̃p�[�Z���e�[�W�擾�I
		percent = my_player->Get_skill_percentage(i);

		// ���ւ���Ă��� �O�́���1.0f����Ȃ������Ƃ��@���߂�MAX�ɂȂ�����g��
		if (my_player->isUnlockSkill(i))
		{
			if (savePercent[i] != 1.0f)
			{
				if (percent >= 1.0f){
					SkileSphere[i]->Action();
					SkileRip[i]->Action();
					se->Play("�X�L�����܂�");
				}
			}
		}

		savePercent[i] = percent;	// �O�́�����ۑ����邺�I�I

		// �������I�𒆂̃X�L��
		if (my_player->Get_select_skill() == i)
		{
			col = 0xffffffff;
			//percent = my_player->Get_skill_percentage(i);
			image[SKILL_SELECT]->Render(SkillX + (i * (80)), (200 + SKILL_Y));
		}

		// �X�L�������܂�����
		if (percent >= 1.0f)
		{
			col = 0xffffffff;
			//percent = my_player->Get_skill_percentage(i);
		}
		// �X�L�������܂��Ă��Ȃ�
		else if (percent < 1.0f)
		{
			col = 0xaaaaaaaa;
			//percent = my_player->Get_skill_percentage(i);
		}

		// �܂����ւ��ĂȂ��X�L��
		if (!my_player->isUnlockSkill(i))
		{
			col = 0xaa111111;
			percent = 1;
			savePercent[i] = 0.99f;
		}
		gauge->Render(percent, SkillX + (i * (80)), (200 + SKILL_Y), 128, 128, i * 128, 0, 128, 128, RS_COPY, col);

		// ���X�L���A�C�R����
		image[IMAGE::SKILL_GUN + i]->SetARGB(col);
		image[IMAGE::SKILL_GUN + i]->Render(SkillX + (48 + i * 80), 216 + 32 + SKILL_Y, 32, 32, 0, 0, 32, 32);


	}

	// ����Action�����Ă��邽�߃����_�[�ōX�VUC
	// �X�L�����܂�����g��̍X�V
	for (int i = 0; i < SKILL_MAX; i++)
	{
		SkileSphere[i]->Update();
		SkileRip[i]->Update();
	}

	// �X�L�����܂�����
	for (int i = 0; i < SKILL_MAX; i++)
	{
		SkileRip[i]->Render(SkillX + 80 * (float)i, (float)(200 + SKILL_Y), RS_ADD);
		SkileSphere[i]->Render(SkillX + 80 * (float)i, (float)(200 + SKILL_Y), RS_ADD);
	}
}

void UI::Action()
{
	//Vector2 src;	// ����Ă���摜�̈ʒu
	//src.x = 256 * 3;
	//src.y = 0;
	//
	//if (my_player->manhole_no_haninai)
	//{
	//	src.x = 0;
	//	src.y = 0;
	//}
	//else if (my_player->Get_poster_num() != -1)
	//{
	//	src.x = 0;
	//	src.y = 0;
	//}
	//image[IMAGE::ACTION]->Render(1032, 482, 256, 256, src.x, src.y, 256, 256);
}

void UI::TimeLimit()
{
	const int second = timer->Get_limit_time() % 60, minutes = timer->Get_limit_time() / 60;
	const int kijun = 1092;
	if (TimerX >= kijun)
	{
		TimerX -= 8;
	}

	// �J�E���g�_�E���̑O
	if (CountTimeNo == -1)
	{
		// 64x64
		//image[IMAGE::NUMBER_BACK]->Render(TimerX, 16);		// �w�i
		image[IMAGE::NUMBER]->Render(TimerX, 32, 64, 64, minutes * 64, 0, 64, 64);		// ��
		image[IMAGE::NUMBER]->Render(TimerX + 36, 32, 64, 64, 13 * 64, 0, 64, 64);			// :
		image[IMAGE::NUMBER]->Render(TimerX + 72, 32, 64, 64, second / 10 * 64, 0, 64, 64);	// �b(10�̈�)
		image[IMAGE::NUMBER]->Render(TimerX + 108, 32, 64, 64, second % 10 * 64, 0, 64, 64);	// �b(1�̈�)
	}
	// �J�E���g�_�E���`��
	C_Five->Render(498, 182, RS_COPY);
	C_Four->Render(498, 182, RS_COPY);
	C_Three->Render(498, 182, RS_COPY);
	C_Two->Render(498, 182, RS_COPY);
	C_One->Render(498, 182, RS_COPY);
}

void UI::Command()
{
	Vector2 pos(320, 320);
	switch (command)
	{
	case REND_COMMAND::LEFT:
		image[IMAGE::ARROW_LEFT]->Render(pos.x, pos.y, 256, 256, 0, 0, 256, 256);
		break;
	case REND_COMMAND::RIGHT:
		image[IMAGE::ARROW_RIGHT]->Render(pos.x, pos.y, 256, 256, 0, 0, 256, 256);
		break;
	case REND_COMMAND::UP:
		image[IMAGE::ARROW_UP]->Render(pos.x, pos.y, 256, 256, 0, 0, 256, 256);
		break;
	case REND_COMMAND::DOWN:
		image[IMAGE::ARROW_DOWN]->Render(pos.x, pos.y, 256, 256, 0, 0, 256, 256);
		break;
	case REND_COMMAND::LU:
		image[IMAGE::ARROW_LEFT]->Render(pos.x, pos.y, 256, 256, 0, 0, 256, 256);
		break;
	case REND_COMMAND::LD:
		image[IMAGE::ARROW_LEFT]->Render(pos.x, pos.y, 256, 256, 0, 0, 256, 256);
		break;
	case REND_COMMAND::RU:
		image[IMAGE::ARROW_LEFT]->Render(pos.x, pos.y, 256, 256, 0, 0, 256, 256);
		break;
	case REND_COMMAND::RD:
		image[IMAGE::ARROW_LEFT]->Render(pos.x, pos.y, 256, 256, 0, 0, 256, 256);
		break;
	case REND_COMMAND::ROLL:
		image[IMAGE::ARROW_ROLL]->Render(pos.x, pos.y, 256, 256, 0, 0, 256, 256);
		break;
	}
}

//
//=============================================================================================





//*****************************************************************************************************************************
//		�u�X�^�[�g�v�̎��̈Ϗ�
//		��[���c�Ƃ��̎G�p�͂����ŏ���

void UI::Mode::Start::Initialize()
{
	step = 0;
	frame = 0;
	//yooi = new iex2DObj("DATA/UI/call/yo-i.png");
	//don = new iex2DObj("DATA/UI/call/don.png");

	Ready = new AnimationRippleEx("DATA/UI/call/A_Ready.png",
		12, 10, 12, 2.0f, -(1.5f / 12.0f), true);
	Ready->Action();
	GO = new AnimationRippleEx("DATA/UI/call/A_Go.png",
		18, 4, 12, 1.0f, +(0.3f / 18.0f), true);
	GORip = new AnimationRippleEx("DATA/UI/call/A_Go.png",
		18, 1, 2, 1.0f, +(2.0f / 18.0f), false);
}
UI::Mode::Start::~Start()
{
	//delete yooi;
	//delete don;
	delete Ready;
	delete GO;
	delete GORip;
}
void UI::Mode::Start::Update()
{
	// �A�j���X�V
	Ready->Update();
	GO->Update();
	GORip->Update();

	if (++frame >= 150)
	{
		step++;
		frame = 0;

		if (step == 1)
		{
			me->isYooiDon = true;

			// Go!�J�n�@Ready�X�g�b�v�I
			GO->Action();
			GORip->Action();
			Ready->Stop();
		}
		if (step >= 2)
		{
			me->Change_mode((int)SceneMain::MODE::MAIN);
		}
	}
}
void UI::Mode::Start::Render()
{
	//me->TimeLimit();
	if(!SCOREATACK_FLAG)me->Graph();
	me->SkillGauge();
	me->Action();
	YooiDon();
}
void UI::Mode::Start::YooiDon()
{
	//BYTE alpha = (int)(((float)(180-frame) / 150) * 256);
	switch (step)
	{
	case 0:
		//yooi->SetARGB(256, 256, 256, (int)alpha);
		//yooi->Render(128, 182, 1024, 256, 0, 0, 1024, 256);
		Ready->Render(128, 182, RS_COPY);
		break;

	case 1:
		//don->Render(128, 182, 1024, 256, 0, 0, 1024, 256);
		GO->Render(128, 182, RS_COPY);
		GORip->Render(128, 182, RS_ADD);
		break;
	}
}

void UI::Mode::End::Initialize()
{
	//	sokomade = new iex2DObj("DATA/UI/call/sokomade.png");
	TimeUp = new AnimationRippleEx("DATA/UI/call/A_TimeUp.png",
		18, 4, 12, 0.8f, +(0.3f / 18.0f), true);
	TimeUpRip = new AnimationRippleEx("DATA/UI/call/A_TimeUp.png",
		18, 1, 2, 0.8f, +(1.3f / 18.0f), false);

	TimeUp->Action();
	TimeUpRip->Action();
}
UI::Mode::End::~End()
{
	//delete sokomade;
	delete TimeUp;
	delete TimeUpRip;
}
void UI::Mode::End::Update()
{

}
void UI::Mode::End::Render()
{
	// �Ƃ肠����Render�ōX�V
	TimeUp->Update();
	TimeUpRip->Update();

	if(SCOREATACK_FLAG)me->Graph();
	me->SkillGauge();
	me->Action();
	//sokomade->Render(128, 182, 1024, 256, 0, 0, 1024, 256);
	TimeUp->Render(128, 182, RS_COPY);
	TimeUpRip->Render(128, 182, RS_ADD);
}
//
//=============================================================================================





//*****************************************************************************************************************************
//
//		�e���b�v�֌W
void UI::Append_telop(int id)
{
	Telop *set = new Telop;
	List.push_back(set);
}

Telop::Telop() :app_timer(150), erase(false), step(0)
{
	moji = new iex2DObj("DATA/UI/telop/telop.png");
}

Telop::~Telop()
{
	delete moji;
}

void Telop::Update()
{
	switch (step)
	{
	case 0:
		UI::tape_len += .02f;
		if (UI::tape_len >= 1)
		{
			UI::tape_len = 1;
			step++;
		}
		break;
	case 1:
		if (--app_timer < 0) step++;;
		break;
	case 2:
		UI::tape_len -= .04f;
		if (UI::tape_len <= 0)
		{
			UI::tape_len = 0;
			erase = true;
		}
		break;
	}
}

void Telop::Render()
{
	if (step > 0)
	{
		moji->Render(378, 16, 1024, 128, 0, 0, 1024, 128);
	}
}
//
//=============================================================================================


//*****************************************************************************************************************************
//
//		�}���z�[���֌W
void UI::Manhole_fade()
{
	(this->*fadeM_funk[(int)m_fade_type])();
}
void UI::fadeM_none()
{
	//if (KEY(KEY_R3) == 1)manhole_f_scale += .05f;
	//if (KEY(KEY_L3) == 1)manhole_f_scale -= .05f;
	//image[IMAGE::MANHOLE_FADE]->SetScale(manhole_f_scale);
	//image[IMAGE::MANHOLE_FADE]->Render(0, 0, 1280, 720, 0, 0, 1280, 720);
	//image[IMAGE::MANHOLE_FADE]->SetScale(1);
	//image[IMAGE::MANHOLE_FADE]->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, 1280, 720);
	//Text::Draw(320, 320, 0xffffff00, "SCALE : %.3f", manhole_f_scale);
}
void UI::fadeM_in()
{
	if ((manhole_f_scale += .25f) > 15.0f) m_fade_type = MANHOLE_FADE_TYPE::NONE;
	if (manhole_f_scale <= 1.0f)
	{
		iexPolygon::Rect(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, RS_COPY, 0xff000000);
	}
	image[IMAGE::MANHOLE_FADE]->SetScale(manhole_f_scale);
	image[IMAGE::MANHOLE_FADE]->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, 1280, 620);
}
void UI::fadeM_out()
{
	manhole_f_scale -= .4f;
	if (manhole_f_scale <= 1.0f)
	{
		iexPolygon::Rect(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, RS_COPY, 0xff000000);
	}
	else
	{
		image[IMAGE::MANHOLE_FADE]->SetScale(manhole_f_scale);
		image[IMAGE::MANHOLE_FADE]->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, 1280, 620);
	}
}

void UI::SetManholeFade(MANHOLE_FADE_TYPE type)
{
	if (m_fade_type == type) return;
	m_fade_type = type;
	switch (type)
	{
	case MANHOLE_FADE_TYPE::F_IN:
		manhole_f_scale = .25f;
		break;
	case MANHOLE_FADE_TYPE::F_OUT:
		manhole_f_scale = 15.0f;
		break;
	}
}
//
//=============================================================================================

UI *ui;