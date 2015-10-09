
#pragma once

#include "iextreme.h"
#include "../Enum_public.h"

class Score;
class Player;

class Poster
{
private:
	Score *score;

	enum class MOTION_NUMBER
	{
		WAITE = 0,
		REND = 1
	};

	TEAM_COLOR force; // 勢力<チーム>
	int POINT_TIME; // ポイントが加算される時間
	int ADD_POINT; // 一度に加算されるポイント
	int timer;

	iex3DObj *model; // 3Dモデル
	Vector3 position; // 位置
	Vector3 forward; // ポスターの向き<表>
	float angle; // 向きを model に渡すよう

	struct 
	{
		float min_y, // 下方向
			max_y; // 上方向
		float wide; // 左右
		float forward; // 前方
	}range;

	enum class MODE
	{
		WAITE,
		REND,
		MAX
	}mode;
	// モード
	class Mode
	{
	protected:
		Poster *me;
	public:
		Mode(Poster *me) : me(me){}
		virtual ~Mode(){}
		virtual void Initialize(){}
		virtual void Update(){}
		virtual void Render(){}
	}*mode_list[(int)MODE::MAX];
	// 待機中
	class Mode_waite : public Mode
	{
	public:
		Mode_waite(Poster *me) : Mode(me){}
		void Initialize() override;
		void Update() override;
		void Render() override;
	};
	// 破られてる
	class Mode_rend : public Mode
	{
	public:
		Mode_rend(Poster *me) :Mode(me){}
		void Initialize() override;
		void Update() override;
		void Render() override;
	};

	void Change_mode(MODE mode);
public:
	Poster();
	~Poster();

	void Initialize(iex3DObj *model, Score *score, int point);
	void Release();
	void Update();
	void Render();

	// ゲッター、セッター
	Vector3& Get_pos(){ return position; }

	float Get_angle(){ return angle; }

	void Set_pose(const Vector3& forward, const Vector3& pos); // 姿勢を計算する
	void Set_pose(const float angle, const Vector3& pos); // 姿勢を計算する

	// プレイヤーからのアクション
	void Do_playeraction(Player *player, TEAM_COLOR color, Texture2D *tex); // 破る貼る
	void Rend(Player *player, TEAM_COLOR color, Texture2D *tex); // 破る
	void Paste(Player *player, TEAM_COLOR color, Texture2D *tex); // 貼る

	bool Can_do(Player *player, TEAM_COLOR color); // playerがポスターに何かできる状態

	// テスト
	void Change_color(TEAM_COLOR color, Texture2D *tex);
};
