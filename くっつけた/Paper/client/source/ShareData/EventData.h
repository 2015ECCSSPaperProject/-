#pragma once

enum EventType
{
	CharacterGoFront,
	CharacterGoBack,
	CharacterRotateRight,
	CharacterRotateLeft,
	MazeRotateRight,
	MazeRotateLeft,
	Ready,
	Start,
	Goal,
	TimeOver,
	WinChallenger,
	WinDefender,
	Disconnect,
};

#pragma pack(1)
struct Event
{
	EventType type;
	int id;
	int param1, param2;
};
#pragma pack()
