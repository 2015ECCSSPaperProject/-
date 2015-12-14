#pragma once

#include <process.h>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

using namespace std;

// �L���[�ɑ���C�x���g
class PaperData
{
public:
	int ID;	//���Ԃ�
	int from; //�N����
};

class PaperQueue
{
public:

	// ������
	void Clear();

	// �L���[�ɋl�߂�
	void Push(PaperData pData);

	// �X�V
	PaperData Update();

private:

	// PaperData�L���[
	queue<PaperData> EventQueue;

};
