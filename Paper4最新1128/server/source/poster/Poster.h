
#pragma once

#include "iextreme.h"
#include "../../../share_data/Enum_public.h"

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
	int reserve_number;	// 破れるモーションが終わったとき張られる番号


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
	void Do_playeraction(BasePlayer *player, int number); // 破る貼る
	void Rend(int number); // 破る
	void Rend();
	void Paste(int number); // 貼る

	bool Can_do(BasePlayer *player, int number); // playerがポスターに何かできる状態
	bool Can_do(BasePlayer *player);

	bool Can_rend(int number); // 破ける
	bool Can_rend();

	bool Can_paste(int number); // 貼れる

	bool Can_dist(const Vector3 &pos, float dist, int number);	// vs紙鉄砲とかに使う
	bool Can_dist(const Vector3 &pos, float dist);
	bool Check_dist(const Vector3 &pos, float dist);

	// テスト
	void Change_user(int number);



	//**************************************************
	/// Get Set
	//**************************************************

	int Get_number(){ return mynumber; }
	void Set_number(int num){ mynumber = num; }
	int Get_animation_frame(){ return model->GetFrame(); }
};
