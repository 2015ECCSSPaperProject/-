
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
	RIGHT_CLICK = 0x2,
	SPACE = 0x4,
	TRG_C = 0x8,	// CÉLÅ[âüÇµÇΩèuä‘
	ATTACK_BUTTON = 0x10
};

enum class REND_FLAG
{
	RIGHT = 0x1,
	DOWN = 0x2,
	LEFT = 0x4,
	UP = 0x8
};