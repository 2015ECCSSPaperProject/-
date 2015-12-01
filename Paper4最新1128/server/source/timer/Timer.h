
#pragma once

class Timer
{
public:
	Timer();
	~Timer();

	// Œv‘ªŠJn
	void Start();
	// §ŒÀŠÔ<•b * 1000>
	void Start(int limit);
	void Start(int m, int s, int ms);
	// ‰Šú‰»
	void Reset();
	// Œo‰ßŠÔŒvZ
	int Check();

	// Œo‰ßŠÔæ“¾
	int Get_delta();
	float Get_second();
	void Get_second(int *s,	// •b
		int *ms);	// ƒ~ƒŠ•b
	void Get_second(int *m,	// •ª
		int *s,		// •b
		int *ms);	// ƒ~ƒŠ•b

	// c‚èŠÔæ“¾
	int Get_delta_limit();
	float Get_second_limit();
	void Get_second_limit(int *s,	// •b
		int *ms);		// ƒ~ƒŠ•b
	void Get_second_limit(int *m,	// •ª
		int *s,			// •b
		int *ms);		// ƒ~ƒŠ•b


private:
	int start; // Œv‘ªŠJnŠÔ
	int delta; // Œo‰ßŠÔ<•b/1000>
	int limit; // §ŒÀŠÔ
};

#define LIMIT_TIME 10
extern Timer *timer;
