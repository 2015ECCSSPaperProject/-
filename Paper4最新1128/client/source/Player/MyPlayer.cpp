#include	"iextreme.h"
#include	"../system/system.h"
#include	"BasePlayer.h"
#include	"MyPlayer.h"
#include	"../ui/UI.h"
#include	"../../IEX/OKB.h"
#include	"../../../share_data/Enum_public.h"
#include	"../Sound/SoundManager.h"
#include "../pie_graph/pie_graph.h"
#include "../Animation/Spread2D.h"
#include	"../Manhole/Manhole.h"
#include	"../Barrier/Barrier.h"
#include	"../paper object/paper object manager.h"
#include	"../paper object/Rend data.h"

static const int KASAMASHI = 10;

//****************************************************************************************************************
//
//  初期化
//
//****************************************************************************************************************
MyPlayer::MyPlayer() :BasePlayer()
{
	//skillGage = new Pie_graph_content("DATA/skillGage/SpiritCircle_gage.png");	//	ゲージ
	//skillGage->Add_content("DATA/skillGage/SpiritCircle_UNDER.png");

	// 関数ポインタ初期化
	RendSE[KIND_PAPER_OBJECT::POSTER] = &MyPlayer::RendPosterSE;
	RendSE[KIND_PAPER_OBJECT::FLYER] = &MyPlayer::RendFlyerSE;
	RendSE[KIND_PAPER_OBJECT::CALENDAR] = &MyPlayer::RendCalendarSE;
	RendSE[KIND_PAPER_OBJECT::MAGAZIN] = &MyPlayer::RendMagazineSE;
	RendSE[KIND_PAPER_OBJECT::MONEY] = &MyPlayer::RendMoneySE;
	RendSE[KIND_PAPER_OBJECT::SEISHO] = &MyPlayer::RendSeisyoSE;
	RendSE[KIND_PAPER_OBJECT::SHINBUN] = &MyPlayer::RendShinbunSE;
	RendSE[KIND_PAPER_OBJECT::SIGN] = &MyPlayer::RendSignSE;
	RendSE[KIND_PAPER_OBJECT::TOILET_PAPER] = &MyPlayer::RendToileSE;
	RendSE[KIND_PAPER_OBJECT::ZASSHI] = &MyPlayer::RendZasshiSE;
	RendSE[KIND_PAPER_OBJECT::SHOJI] = &MyPlayer::RendShojiSE;
}

MyPlayer::~MyPlayer()
{

}

void MyPlayer::Initialize(iex3DObj **obj)
{
	BasePlayer::Initialize(obj);
	isMyNunber = true;
	se_step = 0;

	// ATフィールド展開
	se->Play("AT", true);

	// 破るマウスの動きの初期化
	const int num = 4;
	Rend_data::Movedata data[num]=
	{
		{ 80000, 0 },
		{ -80000, 0 },
		{ 80000, 0 },
		{ -80000, 0 }
	};
		// 後々ここを可変長にする
	rend_data = new Rend_data(num, data);
}

void MyPlayer::Release()
{
	BasePlayer::Release();
}




//*************************************************************************************************************************
//		更新
//*************************************************************************************************************************

void MyPlayer::Update()
{
	/*入力受付処理*/
	Control_all();

	// Cキーのトグル
	//m_controlDesc.controlFlag &= ((BYTE)PLAYER_CONTROL::TRG_C ^ 0xff);
	//if (toggle_c)
	//	m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::TRG_C;

	/*マウスの処理*/
	if (ui->isStart())Mouse_Update();

	BasePlayer::Update();
	Update_action();

	// リスナー情報
	Update_listener();

	//if (rend_data->Move_check(m_controlDesc.mouseX, m_controlDesc.mouseY))
	//{
	//	se->Play("手裏剣");
	//	rend_data->Reset();
	//}
}

void MyPlayer::Update_action()
{
	// 継承先のアクションクラスを使えないのでここで無理やり更新

	switch (action_part)
	{
	case ACTION_PART::REND:
		// 破くモーションのフレーム
		if (models[(int)model_part]->GetParam(0) == 1)
		{
			// ここでキューにプッシュ

		}
		break;

	case ACTION_PART::MANHOLE:
		if (models[(int)model_part]->GetParam(0) == 1)
		{
			if (se_receive == -1)
			{
				se_receive = se->Play((isManhole) ? "のぼる" : "落ちる");
			}
		}
		if (models[(int)model_part]->GetParam(0) == 2)
		{
			if (ui->GetManholeFade() != UI::MANHOLE_FADE_TYPE::F_OUT)ui->SetManholeFade(UI::MANHOLE_FADE_TYPE::F_OUT);
		}
		break;

	case ACTION_PART::REND_OBJ:
		if (kind_paper_obj != -1)(this->*RendSE[kind_paper_obj])();	// それぞれのモーションに合わせて音を出す
		break;
	}
}


/*	操作　*/
void MyPlayer::Control_all()
{
	// 初期化
	m_controlDesc.moveFlag &= 0x00000000;
	m_controlDesc.controlFlag &= 0x00000000;

	static int skill_wait = 0;
	if (m_controlDesc.skillFlag != 0)
	{
		if (--skill_wait <= 0){ skill_wait = 0; m_controlDesc.skillFlag &= 0x00000000; }
	}

	if (ui->isStart())
	{
		if (KEY_Get(KEY_UP) == 1)
		{
			m_controlDesc.moveFlag |= (BYTE)PLAYER_IMPUT::UP;
		}
		else if (KEY_Get(KEY_DOWN) == 1)
		{
			m_controlDesc.moveFlag |= (BYTE)PLAYER_IMPUT::DOWN;
		}
		if (KEY_Get(KEY_RIGHT) == 1)
		{
			m_controlDesc.moveFlag |= (BYTE)PLAYER_IMPUT::RIGHT;
		}
		else if (KEY_Get(KEY_LEFT) == 1)
		{
			m_controlDesc.moveFlag |= (BYTE)PLAYER_IMPUT::LEFT;
		}

		//if (GetAsyncKeyState(0x01) & 0x8000)
		if (KeyBoard(MOUSE_LEFT))
		{
			m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::LEFT_CLICK;
		}

		else if (KeyBoard(MOUSE_RIGHT))
		{
			m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::RIGHT_CLICK;
		}
		else if (KeyBoard(MOUSE_CENTAR))
		{
			m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::ATTACK_BUTTON;
		}

		if (KeyBoard(KB_SPACE))
		{
			m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::SPACE;
		}

		// 右クリックで必殺技を発動させるか
		if (m_controlDesc.controlFlag & (int)PLAYER_CONTROL::RIGHT_CLICK)
		{
			if (action_part != ACTION_PART::MOVE) return;
			// ゲージが溜まってたら
			if (skill_data[(int)select_skill].wait_time <= 0)
			{
				const int FLAG[] =
				{
					(int)PLAYER_SKILL::GUN,
					(int)PLAYER_SKILL::SYURIKEN,
					(int)PLAYER_SKILL::KABUTO,
				};

				m_controlDesc.skillFlag |= FLAG[(int)select_skill];
				// スキル撃ったのでクールタイム設定
				skill_data[(int)select_skill].wait_time = skill_data[(int)select_skill].cool_time;
				skill_wait = 3;	// 1フレームだけしか送らなかったらたまに反応しないので3フレームぐらい送る
			}
			//SPI_GET_WHEELSCROLL
		}
	}
}

float MOUSE_SENS = 0.8f;	// マウスの感度

/*	マウス更新	*/
void MyPlayer::Mouse_Update()
{
	//	キー設定	カーソル
	GetCursorPos(&mousePos);

	//　画面の真ん中へ移動
	RECT rc;
	GetWindowRect(iexSystem::Window, &rc);

	mousePos.x -= rc.left + iexSystem::ScreenWidth / 2;
	mousePos.y -= rc.top + iexSystem::ScreenHeight / 2;
	//if (mousePos.x < -128) mousePos.x = -128;
	//if (mousePos.x >  127) mousePos.x = 127;
	//if (mousePos.y < -128) mousePos.y = -128;
	//if (mousePos.y >  127) mousePos.y = 127;
	mousePos.x = (LONG)((float)mousePos.x * MOUSE_SENS);
	mousePos.y = (LONG)((float)mousePos.y * MOUSE_SENS);
	if (GetActiveWindow() == IEX_GetWindow())
	{
		ShowCursor(FALSE);
		SetCursorPos(rc.left + iexSystem::ScreenWidth / 2, rc.top + iexSystem::ScreenHeight / 2);
	}

	//　マウスの動きをデスクに渡す
	m_controlDesc.mouseX = (float)mousePos.x * 1000;
	m_controlDesc.mouseY = (float)mousePos.y * 1000;

	// ホイール
	if (Get_wheel_flag() == WHEEL_FLAG::DOWN)
	{
		if (++select_skill < (int)SKILL::MAX)
		{
			if (skill_data[select_skill].unlock) return;
		}
		select_skill = 0;
	}
	else if (Get_wheel_flag() == WHEEL_FLAG::UP)
	{
		if (--select_skill < 0)
		{
			for (int i = (int)SKILL::MAX - 1;; i--)
			{
				if (skill_data[i].unlock) { select_skill = i; break; }
			}
		}
	}
}

void MyPlayer::Update_listener()
{
	const Vector3 front(models[(int)model_part]->TransMatrix._31, models[(int)model_part]->TransMatrix._32, models[(int)model_part]->TransMatrix._33);
	const Vector3 up(models[(int)model_part]->TransMatrix._21, models[(int)model_part]->TransMatrix._22, models[(int)model_part]->TransMatrix._23);

	se->Set_listener(pos, front, up, move);
}


void Get_2dpos(Vector3 *out, const Vector3 &pos)
{
	Matrix m = matView * matProjection;

	out->x =
		pos.x * m._11 +
		pos.y * m._21 +
		pos.z * m._31 +
		1 * m._41;

	out->y =
		pos.x * m._12 +
		pos.y * m._22 +
		pos.z * m._32 +
		1 * m._42;

	float w =
		pos.x * m._14 +
		pos.y * m._24 +
		pos.z * m._34 +
		1 * m._44;

	if (w == 0)
	{
		out->x = 0;
		out->y = 0;
	}
	else
	{
		out->x /= w;
		out->y /= w;
	}
}


Vector3 Get2Dpos1280x720(const Vector3 &pos)
{
	//ラムダ式Min~Maxの範囲に抑える　
	auto Clamp = [](float val, float Min, float Max){
		return min(Max, max(val, Min));
	};

	Vector3 out;

	Get_2dpos(&out, pos);//日高君の作ったやつを使用

	out.x = Clamp(out.x, -1.0f, 1.0f);
	out.y = Clamp(out.y, -1.0f, 1.0f);

	//1280　720　のとり方
	out.x = (out.x + 1) * 640;
	out.y = (((out.y*-1) + 1) * 360);

	out.z = 0.0f;

	return out;
}

void MyPlayer::RendPosterSE()
{
	switch (se_step)
	{
	case 0:
		// 破き始め
		if (models[(int)model_part]->GetParam(0) == 1)
		{
			if (poster_num != -1)
			{
				const Vector3 pos2D = Get2Dpos1280x720(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 20, 0));
				Number_Effect::SetNum(pos2D.x, pos2D.y, 1 * KASAMASHI, 4);
			}
			se_receive = se->Play("破る");
			se_step = 99;	// ステップ終わり
		}
		break;
	}
}
void MyPlayer::RendFlyerSE()
{
	switch (se_step)
	{
	case 0:
		// 破き始め
		if (models[(int)model_part]->GetParam(0) == 1)
		{
			if (poster_num != -1)
			{
				const Vector3 pos2D = Get2Dpos1280x720(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 20, 0));
				Number_Effect::SetNum(pos2D.x, pos2D.y, 5 * KASAMASHI, 2);
			}
			se_receive = se->Play("破る");
			se_step = 99;	// ステップ終わり
		}
		break;
	}
}
void MyPlayer::RendCalendarSE()
{
	switch (se_step)
	{
	case 0:
		// 破き始め
		if (models[(int)model_part]->GetParam(5) == 1)
		{
			se_receive = se->Play("カレンダー破り", true);
			se_step++;
		}
		break;
	case 1:
		// 破き終わり
		if (models[(int)model_part]->GetParam(5) == 2)
		{
			se->Stop("カレンダー破り", se_receive);
			se_step++;
		}
		break;
	case 2:
		// 雄叫び
		if (models[(int)model_part]->GetParam(5) == 3)
		{
			if (poster_num != -1)
			{
				const Vector3 pos2D = Get2Dpos1280x720(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 10, 0));
				Number_Effect::SetNum(pos2D.x, pos2D.y, 5 * KASAMASHI, 3);
			}
			se_step = 99;	// ステップ終わり
		}
	}
}
void MyPlayer::RendMagazineSE()
{
	switch (se_step)
	{
	case 0:
		// 最初から鳴らす
		se_receive=se->Play("マガジン破り");
		se_step++;

		break;
	case 1:
		// 破き
		if (models[(int)model_part]->GetParam(5) == 1)
		{
			if (poster_num != -1)
			{
				const Vector3 pos2D = Get2Dpos1280x720(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 10, 0));
				Number_Effect::SetNum(pos2D.x, pos2D.y, 2 * KASAMASHI, 1);
			}
			se->Stop("マガジン破り", se_receive);
			se->Play("マガジン破り2");
			se_step = 99;	// ステップ終わり
		}
		break;
	}
}
void MyPlayer::RendMoneySE()
{
	switch (se_step)
	{
	case 0:
		// 破き始め
		if (models[(int)model_part]->GetParam(5) == 1)
		{
			se_receive = se->Play("お金破り", true);
			se_step++;
		}
		break;
	case 1:
		// 破き終わり
		if (models[(int)model_part]->GetParam(5) == 2)
		{
			se->Stop("お金破り", se_receive);
			se_step++;
		}
		break;
	case 2:
		// 投げ捨て
		if (models[(int)model_part]->GetParam(5) == 3)
		{
			if (poster_num != -1)
			{
				const Vector3 pos2D = Get2Dpos1280x720(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 10, 0));
				Number_Effect::SetNum(pos2D.x, pos2D.y, 3 * KASAMASHI, 1);
			}
			se->Play("サイン破り2");
			se_step = 99;	// ステップ終わり
		}
	}
}
void MyPlayer::RendSeisyoSE()
{
	switch (se_step)
	{
	case 0:
		// 最初から鳴らす
		se_receive = se->Play("聖書破り");
		se_step++;
		break;

	case 1:
		// 上に投げ
		if (models[(int)model_part]->GetParam(5) == 1)
		{
			se->Play("聖書破り2");
			se_step++;	// ステップ終わり
		}
		break;

	case 2:
		// 破き
		if (models[(int)model_part]->GetParam(5) == 2)
		{
			if (poster_num != -1)
			{
				const Vector3 pos2D = Get2Dpos1280x720(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 10, 0));
				Number_Effect::SetNum(pos2D.x, pos2D.y, 3 * KASAMASHI, 1);
			}
			se->Stop("聖書破り", se_receive);
			se->Play("聖書破り3");
			se_step = 99;	// ステップ終わり
		}
		break;
	}
}
void MyPlayer::RendSignSE()
{
	switch (se_step)
	{
	case 0:
		// 破き始め
		if (models[(int)model_part]->GetParam(5) == 1)
		{
			if (poster_num != -1)
			{
				const Vector3 pos2D = Get2Dpos1280x720(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 10, 0));
				Number_Effect::SetNum(pos2D.x, pos2D.y, 3 * KASAMASHI, 1);
			}
			se->Play("サイン破り");
			se_step++;
		}
		break;
	case 1:
		// 破き終わり
		if (models[(int)model_part]->GetParam(5) == 2)
		{
			//se->Play("サイン破り2");
			se->Play("短い破り");
			se_step = 99;	// ステップ終わり
		}
		break;
	}
}
void MyPlayer::RendShinbunSE()
{
	switch (se_step)
	{
	case 0:
		// 紙の音的な
		se_step++;
		break;

	case 1:
		// 破き始め
		if (models[(int)model_part]->GetParam(5) == 1)
		{
			if (poster_num != -1)
			{
				const Vector3 pos2D = Get2Dpos1280x720(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 10, 0));
				Number_Effect::SetNum(pos2D.x, pos2D.y, 4 * KASAMASHI, 1);
			}
			se->Play("新聞破り");
			se_step++;
		}
		break;
	case 2:
		// 破き終わり
		if (models[(int)model_part]->GetParam(5) == 2)
		{
			se->Play("新聞破り2");
			se_step = 99;	// ステップ終わり
		}
		break;
	}
}
void MyPlayer::RendToileSE()
{
	switch (se_step)
	{
	case 0:
		// 最初から鳴らす
		se_receive = se->Play("トイレ破り");
		se_step++;

		break;
	case 1:
		// 破き
		if (models[(int)model_part]->GetParam(5) == 1)
		{
			if (poster_num != -1)
			{
				const Vector3 pos2D = Get2Dpos1280x720(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 10, 0));
				Number_Effect::SetNum(pos2D.x, pos2D.y, 3 * KASAMASHI, 1);
			}
			se->Stop("トイレ破り", se_receive);
			se->Play("トイレ破り2");
			se_step = 99;	// ステップ終わり
		}
		break;
	}
}
void MyPlayer::RendZasshiSE()
{
	switch (se_step)
	{
	case 0:
		// 破き始め
		if (models[(int)model_part]->GetParam(5) == 1)
		{
			if (poster_num != -1)
			{
				const Vector3 pos2D = Get2Dpos1280x720(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 10, 0));
				Number_Effect::SetNum(pos2D.x, pos2D.y, 3 * KASAMASHI, 1);
			}
			se_receive = se->Play("短い破り");
			se_step = 99;	// ステップ終わり
		}
		break;
	}
}

void MyPlayer::RendShojiSE()
{
	switch (se_step)
	{
	case 0:
		// 破き始め
		if (models[(int)model_part]->GetParam(5) == 1)
		{
			se_receive = se->Play("カレンダー破り", true);
			se_step++;
		}
		break;
	case 1:
		// 破き終わり
		if (models[(int)model_part]->GetParam(5) == 2)
		{
			se->Stop("カレンダー破り", se_receive);
			se_step++;
		}
		break;
	case 2:
		// 蹴り
		if (models[(int)model_part]->GetParam(5) == 3)
		{
			if (poster_num != -1)
			{
				const Vector3 pos2D = Get2Dpos1280x720(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 10, 0));
				Number_Effect::SetNum(pos2D.x, pos2D.y, 3 * KASAMASHI, 1);
			}
			se->Play("サイン破り");
			se_step = 99;	// ステップ終わり
		}
	}
}

void MyPlayer::Set_action(ACTION_PART part)
{
	if (action_part != part)
	{
		if (action_part == ACTION_PART::MANHOLE && part == ACTION_PART::MOVE)
		{
			isManhole ^= 1;
			event_bgm->Set_manhole(isManhole);
			ui->SetManholeFade(UI::MANHOLE_FADE_TYPE::F_IN);
		}
		if (part == ACTION_PART::REND_OBJ) se_step = 0;
		Change_action(part);
	}
}

//*************************************************************************************************************************
//		描画
//*************************************************************************************************************************
void MyPlayer::Render(iexShader *shader, char *name)
{
	BasePlayer::Render(shader, name); 
	//Text::Draw(320, 480, 0xff00ffff, "%.3f", m_controlDesc.mouseX);
	//Text::Draw(320, 520, 0xff00ffff, "%.3f", m_controlDesc.mouseY);
	//Text::Draw(32, 560, 0xff00ffff, "%.1f",pos.y);
	//Text::Draw(32, 600, 0xff00ffff, "%.1f",pos.z);
	//DeferredManager.ForwardBigin();
	//if(manhole_no_haninai)
	//Text::Draw(320, 600, 0xffffffff, "マンホール範囲内");

	//円ゲージ
	//float persent =1.0f - ((float)skill_data[(int)SKILL::GUN].wait_time / (float)skill_data[(int)SKILL::GUN].cool_time);
	//Text::Draw(32, 420, 0xff00ffff, "aaa%f", persent);
	//skillGage->Render(persent, 0, 300, 128, 128, 0, 0, 128, 128);
	//DeferredManager.ForwardEnd();
}