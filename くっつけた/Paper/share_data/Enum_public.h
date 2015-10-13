
#pragma once

enum class TEAM_COLOR
{
	ONE,
	TWO,
	NONE,
};

enum class PLAYER_IMPUT
{
	UP			 = 0x1, 
	DOWN		 = 0x2, 
	LEFT		 = 0x4, 
	RIGHT		 = 0x8,
	LEFT_CLICK	 = 0x10, 
	RIGHT_CLICK	 = 0x20, 
	CENTER_CLICK = 0x40
};
