
#pragma once

enum class PLAYER_IMPUT
{
	UP = 0x1,
	DOWN = 0x2,
	LEFT = 0x4,
	RIGHT = 0x8,
};

enum class PLAYER_CONTROL
{
	LEFT_CLICK = 0x1,
	RIGHT_CLICK = 0x1 << 1,
	SPACE = 0x1 << 2,
	ATTACK_BUTTON = 0x1 << 3,
	RENDING = 0x1 << 4,	// 小物を破ってる途中
	AMBULANCE = 0x1 << 5
};

// 必殺技
enum class PLAYER_SKILL
{
	GUN = 0x1,
	SYURIKEN = 0x1 << 1,
	KABUTO = 0x1 << 2,
};


enum class PLAYER_FLAG	// フラグ全般にする
{
	// クライアントで使用するフラグ
	REND = 0x1,

	// サーバーで使用するフラグ
	BARRIER = 0x1,
};


#define LAYER_MAX	10
#define KIND_MAX	3
#define NO_MAX		10
//　スタンプの種類に名前を付けると便利
enum KIND_NAME
{
	OBJECT = 0, FOOD = 1, BACK = 2
};

// ペーパーオブジェクトの種類
enum KIND_PAPER_OBJECT
{
	POSTER = 0,
	FLYER = 1,
	CALENDAR = 2,
	MAGAZIN = 3,
	MONEY = 4,
	SEISHO = 5,
	SHINBUN = 6,
	SIGN = 7,
	TOILET_PAPER = 8,
	ZASSHI = 9,
	SHOJI = 10,
	HUUSENN = 11,
	KIND_MAX_PAPER_OBJ = 12
};


// テロップID
enum class TELOP_ID
{
	FLYER=1,		// フライヤーが降ってきた
	AREA_OPEN=2,	// 新たなエリアが解放された
	BALLOON=3		// 風船が飛んできた
};