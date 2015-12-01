
#pragma once

class Timer
{
public:
	Timer();
	~Timer();

	// �v���J�n
	void Start();
	// ��������<�b * 1000>
	void Start(int limit);
	void Start(int m, int s, int ms);
	// ������
	void Reset();
	// �o�ߎ��Ԍv�Z
	int Check();

	// �o�ߎ��Ԏ擾
	int Get_delta();
	float Get_second();
	void Get_second(int *s,	// �b
		int *ms);	// �~���b
	void Get_second(int *m,	// ��
		int *s,		// �b
		int *ms);	// �~���b

	// �c�莞�Ԏ擾
	int Get_delta_limit();
	float Get_second_limit();
	void Get_second_limit(int *s,	// �b
		int *ms);		// �~���b
	void Get_second_limit(int *m,	// ��
		int *s,			// �b
		int *ms);		// �~���b


private:
	int start; // �v���J�n����
	int delta; // �o�ߎ���<�b/1000>
	int limit; // ��������
};

#define LIMIT_TIME 10
extern Timer *timer;
