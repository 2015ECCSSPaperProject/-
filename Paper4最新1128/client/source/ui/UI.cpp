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

#include "../Animation/AnimationRippleEx.h"

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
	// 自分のプレイヤーを指し示す
	my_player = my;

	// スキルゲージさん 改
	gauge = new Pie_graph_content("DATA/UI/skill/skill_gage_n.png");	//	Skileゲージ
	//gauge->Initialize();
	
	// スキル溜まったらさん
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


	// 円グラフさん
	graph = new Pie_graph;
	graph->Add_content("DATA/UI/graph/red.png");
	graph->Add_content("DATA/UI/graph/blue.png");
	graph->Add_content("DATA/UI/graph/yellow.png");
	graph->Add_content("DATA/UI/graph/green.png");
	graph->Add_content("DATA/UI/graph/purple.png");
	graph->Add_content("DATA/UI/graph/pink.png");

	// その他2D初期化
	//image[IMAGE::TEROP] = new iex2DObj("")
	image[IMAGE::ACTION] = new iex2DObj("DATA/UI/action/anim.png");
	image[IMAGE::NUMBER] = new iex2DObj("DATA/UI/Num.png");
	image[IMAGE::NUMBER_BACK] = new iex2DObj("DATA/UI/NumBack.png");
	image[IMAGE::TAPE] = new iex2DObj("DATA/UI/tape/tape.png");
	image[IMAGE::TAPE_BAR] = new iex2DObj("DATA/UI/tape/tape2.png");
	image[IMAGE::SKILL_GUN] = new iex2DObj("DATA/UI/skill/skill1.png");
	image[IMAGE::SKILL_SYURIKEN] = new iex2DObj("DATA/UI/skill/skill2.png");
	image[IMAGE::SKILL_KABUTO] = new iex2DObj("DATA/UI/skill/skill3.png");
	image[IMAGE::SKILL_FRAME] = new iex2DObj("DATA/UI/skill/skill_gage_frame.png");		// スキルのフレーム
	image[IMAGE::SKILL_SELECT] = new iex2DObj("DATA/UI/skill/skilieSelect2.png");		// skilieSelect
	image[IMAGE::SKILL_ROGO] = new iex2DObj("DATA/UI/skill/skill_rogo.png");		// skill_rogo
	image[IMAGE::SCORE_FRAME] = new iex2DObj("DATA/UI/skill/scoreFrame.png");		// scoreFrame
	image[IMAGE::MANHOLE_FADE] = new iex2DObj("DATA/UI/manhole_fade.png");
	image[IMAGE::ARROW_LEFT] = new iex2DObj("DATA/UI/action/Left_Cursor.png");
	image[IMAGE::ARROW_RIGHT] = new iex2DObj("DATA/UI/action/Right_Cursor.png");
	image[IMAGE::ARROW_UP] = new iex2DObj("DATA/UI/action/Up_Cursor.png");
	image[IMAGE::ARROW_DOWN] = new iex2DObj("DATA/UI/action/Down_Cursor.png");
	image[IMAGE::ARROW_ROLL] = new iex2DObj("DATA/UI/action/rot_Cursor.png");
	image[IMAGE::MARK] = new iex2DObj("DATA/Camera/mark.png");
	image[IMAGE::ACTION_HOLD] = new iex2DObj("DATA/UI/action/ホールド！.png");
	image[IMAGE::ACTION_DRAG] = new iex2DObj("DATA/UI/action/ドラッグ.png");
	action_hold = new AnimationRippleEx("DATA/UI/action/ホールド！.png",
		40, 24, 32, 12.2f, (12.2f / 40.0f), true);
	action_drag = new AnimationRippleEx("DATA/UI/action/ドラッグ.png",
		40, 24, 32, 12.2f, (12.2f / 40.0f), true);

	// スーパー西田タイム
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

	TimerX = 1280;			//Timerを動かす
	SkillX = -600;			//SKILLを動かす
	GraphX = -400;			//GRHPHを動かす
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

	// スキル溜まったらさん
	for (int i = 0; i < SKILL_MAX; i++)
	{
		delete	SkileSphere[i];
		delete SkileRip[i];
	}

	// カウントダウン
	delete	C_Five;
	delete	C_Four;
	delete	C_Three;
	delete	C_Two;
	delete	C_One;

	delete action_drag;
	delete action_hold;
}

void UI::Update()
{
	mode->Update();

	// アクションボタンアニメーション
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

void UI::Render_mark()
{
	bool isRange = false;
	for (int i = 0; i < paper_obj_mng->Get_numof(); i++)
	{
		if (!paper_obj_mng->Can_rend(i))continue;
		float tu[2], tv[2];
		if (my_player->Get_poster_num() == i)
		{
			tu[0] = 0, tu[1] = 1;
			tv[0] = 0, tv[1] = 1;
			isRange = true;
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
				//action_hold->Stop();
				//if (!action_drag->isDoing()) action_drag->Action();
				//action_drag->Render(my_player->Get_pos() + Vector3(0, 12, 0), RS_COPY);
				Billboard::Draw3D(my_player->Get_pos() + Vector3(0, 15, 0), image[IMAGE::ACTION_DRAG], 7, 7, tu, tv, RS_COPY);
			}
			else
			{	
				//action_drag->Stop();
				//if (!action_hold->isDoing()) action_hold->Action();
				//action_hold->Render(my_player->Get_pos() + Vector3(0, 12, 0), RS_COPY);
				Billboard::Draw3D(my_player->Get_pos() + Vector3(0, 15, 0), image[IMAGE::ACTION_HOLD], 7, 7, tu, tv, RS_COPY);
			}
		}
		else
		{
			tu[0] = 0, tu[1] = .5f;
			tv[0] = 0, tv[1] = 1;
			Billboard::Draw3D(paper_obj_mng->Get_pos(i) + Vector3(0, 24, 0), image[IMAGE::MARK], 4, 4, tu, tv, RS_COPY);
		}
	}
	//if (!isRange)
	//{
	//	action_drag->Stop();
	//	action_hold->Stop();
	//}
}

//*****************************************************************************************************************************
//
//		基本、処理はここで

void UI::Mode::Main::Update()
{
	const int second = timer->Get_limit_time() % 60, minutes = timer->Get_limit_time() / 60;
	// 残り５秒前！
	if (minutes == 0 && second == 5){
		if (me->CountTimeNo != 5){
			me->CountTimeNo = 5;	me->C_Five->Action();
		}
	}
	// 残り4秒前！
	if (minutes == 0 && second == 4)
	{
		if (me->CountTimeNo != 4){
			me->CountTimeNo = 4;	me->C_Four->Action();
		}
	}
	// 残り3秒前！
	if (minutes == 0 && second == 3)
	{
		if (me->CountTimeNo != 3){
			me->CountTimeNo = 3;	me->C_Three->Action();
		}
	}
	// 残り2秒前！
	if (minutes == 0 && second == 2)
	{
		if (me->CountTimeNo != 2){
			me->CountTimeNo = 2;	me->C_Two->Action();
		}
	}
	// 残り1秒前！
	if (minutes == 0 && second == 1){
		if (me->CountTimeNo != 1){
			me->CountTimeNo = 1;	me->C_One->Action();
		}
	}


	// カウントダウン更新
	me->C_Five->Update();
	me->C_Four->Update();
	me->C_Three->Update();
	me->C_Two->Update();
	me->C_One->Update();

	//// スキル溜まったら波紋の更新
	//for (int i = 0; i < SKILL_MAX; i++)
	//{
	//	me->SkileSphere[i]->Update();
	//}
}

void UI::Mode::Main::Render()
{
	me->Graph();
	me->SkillGauge();
	me->Action();
	me->TimeLimit();
	me->Telop_render();
	//me->Command();
	me->Manhole_fade();
}


void UI::Telop_render()
{
	// テロップのテープ
	int len = (int)(1024 * tape_len);
	image[IMAGE::TAPE_BAR]->Render(250+(1024-len), 92, len, 128, 0, 0, 1024, 128);
	image[IMAGE::TAPE]->Render(1048, 92, 256, 128, 0, 0, 256, 128);

	if (telopID != seted_ID)
	{
		seted_ID = telopID;
		Append_telop(telopID);
	}

	// テロップリストに何か入っていたら
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

// スコアフラグ
void UI::Graph()
{
	// 最初に画面外からでてくるぜぇぇぇええ！！！
	const int kijun = 0;
	if (GraphX <= kijun)
	{
		GraphX += 14;
	}

	// 後ろのフレーム
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
	// スキルUI
	//image[IMAGE::SKILL_GUN]->Render(28, 560, 32, 32, 0, 0, 32, 32);
	//image[IMAGE::SKILL_SYURIKEN]->Render(76, 560, 32, 32, 0, 0, 32, 32);
	//image[IMAGE::SKILL_KABUTO]->Render(124, 560, 32, 32, 0, 0, 32, 32);
	//image[IMAGE::SKILL_ZENRYOKU]->Render(172, 560, 32, 32, 0, 0, 32, 32);

	//int gage_val = my_player->Get_god_gage() / 10;	// スキルゲージ取得
	//gauge->Render(gage_val, 10);

	// 最初に画面外からでてくるぜぇぇぇええ！！！
	const int kijun = 0;
	if (SkillX <= kijun)
	{
		SkillX += 14;
	}

	// SKILL_ロゴ
	image[IMAGE::SKILL_ROGO]->Render(SkillX, (180 + SKILL_Y));

	//円ゲージ
	for (int i = 0; i < (int)BasePlayer::SKILL::MAX; i++)
	{
		DWORD col;
		float percent;

		// スキルのフレーム追加
		image[IMAGE::SKILL_FRAME]->Render(SkillX + (i * (80)), (200 + SKILL_Y));

		// ゲージのパーセンテージ取得！
		percent = my_player->Get_skill_percentage(i);

		// 解禁されていて 前の％が1.0fじゃなかったとき　初めてMAXになったら波紋
		if (my_player->isUnlockSkill(i))
		{
			if (savePercent[i] != 1.0f)
			{
				if (percent >= 1.0f){
					SkileSphere[i]->Action();
					SkileRip[i]->Action();
				}
			}
		}

		savePercent[i] = percent;	// 前の％をを保存するぜ！！

		// 自分が選択中のスキル
		if (my_player->Get_select_skill() == i)
		{
			col = 0xffffffff;
			//percent = my_player->Get_skill_percentage(i);
			image[SKILL_SELECT]->Render(SkillX + (i * (80)), (200 + SKILL_Y));
		}

		// スキルがたまったら
		if (percent >= 1.0f)
		{
			col = 0xffffffff;
			//percent = my_player->Get_skill_percentage(i);
		}
		// スキルがたまっていない
		else if (percent < 1.0f)
		{
			col = 0xaaaaaaaa;
			//percent = my_player->Get_skill_percentage(i);
		}

		// まだ解禁してないスキル
		if (!my_player->isUnlockSkill(i))
		{
			col = 0xaa111111;
			percent = 1;
			savePercent[i] = 0.99f;
		}
		gauge->Render(percent, SkillX + (i * (80)), (200 + SKILL_Y), 128, 128, i * 128, 0, 128, 128, RS_COPY, col);

		// ￥スキルアイコンや
		image[IMAGE::SKILL_GUN + i]->SetARGB(col);
		image[IMAGE::SKILL_GUN + i]->Render(SkillX + (48 + i * 80), 216 + 32 + SKILL_Y, 32, 32, 0, 0, 32, 32);


	}

	// ↑にActionをしているためレンダーで更新UC
	// スキル溜まったら波紋の更新
	for (int i = 0; i < SKILL_MAX; i++)
	{
		SkileSphere[i]->Update();
		SkileRip[i]->Update();
	}

	// スキル溜まったら
	for (int i = 0; i < SKILL_MAX; i++)
	{
		SkileRip[i]->Render(SkillX + 80 * (float)i, (float)(200 + SKILL_Y), RS_ADD);
		SkileSphere[i]->Render(SkillX + 80 * (float)i, (float)(200 + SKILL_Y), RS_ADD);
	}
}

void UI::Action()
{
	Vector2 src;	// 取ってくる画像の位置
	src.x = 256 * 3;
	src.y = 0;

	if (my_player->manhole_no_haninai)
	{
		src.x = 0;
		src.y = 0;
	}
	else if (my_player->Get_poster_num() != -1)
	{
		src.x = 0;
		src.y = 0;
	}
	image[IMAGE::ACTION]->Render(1032, 482, 256, 256, src.x, src.y, 256, 256);
}

void UI::TimeLimit()
{
	const int second = timer->Get_limit_time() % 60, minutes = timer->Get_limit_time() / 60;
	const int kijun = 1092;
	if (TimerX >= kijun)
	{
		TimerX -= 8;
	}

	// カウントダウンの前
	if (CountTimeNo == -1)
	{
		// 64x64
		//image[IMAGE::NUMBER_BACK]->Render(TimerX, 16);		// 背景
		image[IMAGE::NUMBER]->Render(TimerX, 16, 64, 64, minutes * 64, 0, 64, 64);		// 分
		image[IMAGE::NUMBER]->Render(TimerX + 36, 16, 64, 64, 13 * 64, 0, 64, 64);			// :
		image[IMAGE::NUMBER]->Render(TimerX + 72, 16, 64, 64, second / 10 * 64, 0, 64, 64);	// 秒(10の位)
		image[IMAGE::NUMBER]->Render(TimerX + 108, 16, 64, 64, second % 10 * 64, 0, 64, 64);	// 秒(1の位)
	}
	// カウントダウン描画
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
//		「スタート」の時の委譲
//		よーい…とかの雑用はここで処理

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
	// アニメ更新
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

			// Go!開始　Readyストップ！
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
	me->Graph();
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
	// とりあえずRenderで更新
	TimeUp->Update();
	TimeUpRip->Update();

	me->Graph();
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
//		テロップ関係
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
		moji->Render(378, 94, 1024, 128, 0, 0, 1024, 128);
	}
}
//
//=============================================================================================


//*****************************************************************************************************************************
//
//		マンホール関係
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