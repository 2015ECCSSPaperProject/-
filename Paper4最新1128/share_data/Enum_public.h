
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
	//TRG_C = 0x1 << 3,	// C�L�[�������u��
	ATTACK_BUTTON = 0x1 << 3
};

// �K�E�Z
enum class PLAYER_SKILL
{
	GUN = 0x1,
	SYURIKEN = 0x1 << 1,
	KABUTO = 0x1 << 2,
	ZENRYOKU = 0x1 << 3
};


enum class REND_FLAG
{
	RIGHT = 0x1,
	DOWN = 0x2,
	LEFT = 0x4,
	UP = 0x8
};


#define LAYER_MAX	10
#define KIND_MAX	3
#define NO_MAX		10
//�@�X�^���v�̎�ނɖ��O��t����ƕ֗�
enum KIND_NAME
{
	OBJECT = 0, FOOD = 1, BACK = 2
};
