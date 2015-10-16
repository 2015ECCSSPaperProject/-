#pragma once

enum EventType
{
	MazeRotateRight,
	MazeRotateLeft,
	Ready,
	Start,
	Goal,
	TimeOver,
	WinChallenger,
	WinDefender,
	Disconnect,

	PosterUpdate,
};

#pragma pack(1)
struct Event
{
	EventType type;
	int id;
	int param1, param2;
};
#pragma pack()
