#pragma once
/*	自分のプレイヤー	*/

class Pie_graph_content;
class Spread2D;

/*前方宣言*/
//class BasePlayer;

//********************************************************************************************************************
//		自分
//****************************************************************************************************************
class	MyPlayer : public	BasePlayer
{
private:
	int se_step;
	void (MyPlayer::*RendSE[10])();	// 小物破くSE
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
	//	初期化と解放
	//===============================================
	MyPlayer();
	~MyPlayer();
	void Initialize(iex3DObj **obj);
	void Release();


	//SkillData
	//===============================================
	//	更新と描画
	//===============================================
	void Update();
	void Update_action();
	void Render(iexShader *shader = nullptr, char *name = '\0');

	void Mouse_Update();	//	マウスのポジション
	void Control_all();		//　自分だけ操作追加
	void Update_listener();	// リスナー情報更新

	//===============================================
	//	ゲッター,セッター
	//===============================================
	void Set_action(ACTION_PART part);	// オーバーライド
};