#include "iextreme.h"
#include "../Player/BasePlayer.h"

#include "../Scene/SceneMain.h"
#include "../pie_graph/pie_graph.h"
#include "UI.h"

#include "../timer/Timer.h"
#include "../score/Score.h"
#include "../skill_gauge/skill_gauge.h"
#include <assert.h>

float UI::tape_len;

UI::UI() :my_player(nullptr), graph(nullptr), gauge(nullptr), mode(nullptr), isYooiDon(false), telopID(-1), seted_ID(-1)
{
	tape_len = 0;
	for (int i = 0; i < IMAGE::MAX; i++)image[i] = nullptr;
}

void UI::Initialize(BasePlayer *my)
{
	// 自分のプレイヤーを指し示す
	my_player = my;

	// スキルゲージさん
	gauge = new Pie_graph_content("DATA/UI/skill/skill_gage.png");	//	ゲージ
	//gauge->Initialize();

	// 円グラフさん
	graph = new Pie_graph;
	graph->Add_content("DATA/UI/graph/red.png");
	graph->Add_content("DATA/UI/graph/blue.png");
	graph->Add_content("DATA/UI/graph/green.png");
	graph->Add_content("DATA/UI/graph/yellow.png");
	graph->Add_content("DATA/UI/graph/purple.png");
	graph->Add_content("DATA/UI/graph/pink.png");

	// その他2D初期化
	//image[IMAGE::TEROP] = new iex2DObj("")
	image[IMAGE::ACTION] = new iex2DObj("DATA/UI/action/anim.png");
	image[IMAGE::NUMBER] = new iex2DObj("DATA/UI/Num.png");
	image[IMAGE::TAPE] = new iex2DObj("DATA/UI/tape/tape.png");
	image[IMAGE::TAPE_BAR] = new iex2DObj("DATA/UI/tape/tape2.png");
	image[IMAGE::SKILL_GUN] = new iex2DObj("DATA/UI/skill/skill1.png");
	image[IMAGE::SKILL_SYURIKEN] = new iex2DObj("DATA/UI/skill/skill2.png");
	image[IMAGE::SKILL_KABUTO] = new iex2DObj("DATA/UI/skill/skill3.png");
	image[IMAGE::SKILL_ZENRYOKU] = new iex2DObj("DATA/UI/skill/skill4.png");

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
}

void UI::Update()
{
	mode->Update();
}

void UI::Render()
{
	mode->Render();
}



//*****************************************************************************************************************************
//
//		基本、処理はここで

void UI::Mode::Main::Update()
{

}

void UI::Mode::Main::Render()
{
	me->Graph();
	me->SkillGauge();
	me->Action();
	me->TimeLimit();
	me->Telop_render();
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

void UI::Graph()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		graph->Set_percent(i, (float)score_mng->Get(i));
	}
	graph->Render(16, 16, 128, 128, 0, 0, 256, 256);
}

void UI::SkillGauge()
{
	// スキルUI
	//image[IMAGE::SKILL_GUN]->Render(28, 560, 32, 32, 0, 0, 32, 32);
	//image[IMAGE::SKILL_SYURIKEN]->Render(76, 560, 32, 32, 0, 0, 32, 32);
	//image[IMAGE::SKILL_KABUTO]->Render(124, 560, 32, 32, 0, 0, 32, 32);
	//image[IMAGE::SKILL_ZENRYOKU]->Render(172, 560, 32, 32, 0, 0, 32, 32);

	//int gage_val = my_player->Get_god_gage() / 10;	// スキルゲージ取得
	//gauge->Render(gage_val, 10);

	//円ゲージ
	for (int i = 0; i < (int)BasePlayer::SKILL::MAX; i++)
	{
		DWORD col;
		float percent;

		// 自分が選択中のスキル
		if (my_player->Get_select_skill() == i)
		{
			col = 0xffffffff;
			percent = my_player->Get_skill_percentage(i);
		}

		// 解禁したスキル
		else if (my_player->isUnlockSkill(i))
		{
			col = 0xcccccccc;
			percent = my_player->Get_skill_percentage(i);
		}

		// まだ解禁してないスキル
		else
		{
			col = 0x55555555;
			percent = 1;
		}
		gauge->Render(percent, 16+i*80, 200, 64, 64, 0, 0, 64, 64, RS_COPY, col);
		image[IMAGE::SKILL_GUN + i]->Render(32 + i*80, 216, 32, 32, 0, 0, 32, 32, RS_COPY, col);
	}
}

void UI::Action()
{
	Vector2 src;	// 取ってくる画像の位置
	src.x = 0;
	src.y = 0;

	image[IMAGE::ACTION]->Render(1032, 482, 256, 256, src.x, src.y, 256, 256);
}

void UI::TimeLimit()
{
	const int second = timer->Get_limit_time() % 60, minutes = timer->Get_limit_time() / 60;
	const int kijun = 1092;
	// 64x64
	image[IMAGE::NUMBER]->Render(kijun, 16, 64, 64, minutes * 64, 0, 64, 64);		// 分
	image[IMAGE::NUMBER]->Render(kijun+36, 16, 64, 64, 13 * 64, 0, 64, 64);			// :
	image[IMAGE::NUMBER]->Render(kijun+72, 16, 64, 64, second/10 * 64, 0, 64, 64);	// 秒(10の位)
	image[IMAGE::NUMBER]->Render(kijun+108, 16, 64, 64, second%10 * 64, 0, 64, 64);	// 秒(1の位)
}

//
//=============================================================================================





//*****************************************************************************************************************************
//
//		よーい…とかの雑用はここで処理

void UI::Mode::Start::Initialize()
{
	step = 0;
	frame = 0;
	yooi = new iex2DObj("DATA/UI/call/yo-i.png");
	don = new iex2DObj("DATA/UI/call/don.png");
}
UI::Mode::Start::~Start()
{
	delete yooi;
	delete don;
}
void UI::Mode::Start::Update()
{
	if (++frame >= 150)
	{
		step++;
		frame = 0;

		if (step == 1)
		{
			me->isYooiDon = true;
		}
		if (step >= 2)
		{
			me->Change_mode(SceneMain::MODE::MAIN);
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
	BYTE alpha = (int)(((float)(180-frame) / 150) * 256);
	switch (step)
	{
	case 0:
		yooi->Render(128, 182, 1024, 256, 0, 0, 1024, 256, RS_COPY, (0x00ffffff | (alpha << 24)));
		break;

	case 1:
		don->Render(128, 182, 1024, 256, 0, 0, 1024, 256);
		break;
	}
}

void UI::Mode::End::Initialize()
{
	sokomade = new iex2DObj("DATA/UI/call/sokomade.png");
}
UI::Mode::End::~End()
{
	delete sokomade;
}
void UI::Mode::End::Update()
{

}
void UI::Mode::End::Render()
{
	me->Graph();
	me->SkillGauge();
	me->Action();
	sokomade->Render(128, 182, 1024, 256, 0, 0, 1024, 256);
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

UI *ui;