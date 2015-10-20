
#pragma once

#include "iextreme.h"

class Score;
class BasePlayer;

class Poster
{
private:
	Score *score;

	enum class MOTION_NUMBER
	{
		WAITE = 0,
		REND = 1
	};

	int mynumber;
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
	void Do_playeraction(BasePlayer *player, int user_number); // 破る貼る
	void Rend(int user_number); // 破る
	void Paste(int user_number); // 貼る

	bool Can_do(BasePlayer *player, int user_number); // playerがポスターに何かできる状態
	bool Can_rend(int user_number); // 破ける
	bool Can_paste(int user_number); // 貼れる

	// テスト
	void Change_user(int user_number);

	//**************************************************
	/// Get Set
	//**************************************************

	void Set_anim_frame(int frame){ model->SetFrame(frame); }
};
