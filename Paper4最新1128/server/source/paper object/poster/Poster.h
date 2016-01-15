
#pragma once

// #include "../../../../share_data/Enum_public.h"

class BasePlayer;

class Poster : public Paper_obj_Mesh
{
private:
	enum class MOTION_NUMBER
	{
		WAITE = 0,
	};

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

	void Change_mode(MODE mode);

public:
	Poster();
	Poster( int model_type, iexMesh *model, int point );
	~Poster();

	void Initialize(int model_type, iexMesh *model, int point);
	void Release();
	void Update();
	void Render();

	// ゲッター、セッター
	const Vector3& Get_pos(){ return position; }

	float Get_angle(){ return angle; }

	void Set_pose(const Vector3& forward, const Vector3& pos); // 姿勢を計算する
	void Set_pose(const float angle, const Vector3& pos); // 姿勢を計算する

	/* プレイヤーからのアクション */

	// playerがポスターに何かできる状態
	bool Can_do(BasePlayer *player);

	// 破ける
	bool Can_rend();

	// vs紙鉄砲とかに使う
	bool Can_dist(const Vector3 &pos, float dist);
	bool Check_dist(const Vector3 &pos, float dist);

	// テスト
	int Get_animation_frame(){ return 0; }

	//**************************************************

	unsigned int Get_send_data_size();

	void Get_send_data( char *out );

	virtual KIND_PAPER_OBJECT Get_kind();
};



// ポスターを継承したクラス

class Calendar : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Magazin : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Money : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Seisho : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Shinbun : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Sign : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Toilet_paper : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Zasshi : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};

class Shoji : public Poster
{
public:
	KIND_PAPER_OBJECT Get_kind();
};
