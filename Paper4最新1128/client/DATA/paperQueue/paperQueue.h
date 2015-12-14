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
	static void Clear();

	// �L���[�ɋl�߂�
	static void Push(PaperData pData);

	// �X�V
	static PaperData Update();

private:

	// PaperData�L���[
	static queue<PaperData> EventQueue;

};
 
// �錾
queue<PaperData> PaperQueue::EventQueue;

// ������
void PaperQueue::Clear()
{
	// ��ɂȂ�܂Ń|�b�v
	for (;;)
	{
		// �󂾂�����A��
		if (EventQueue.empty()){ return; }// 

		EventQueue.pop();

	}

}

void PaperQueue::Push(PaperData pData)
{
	// ���̃C�x���g���L���[�փv�b�V��!
	EventQueue.push(pData);
}

PaperData PaperQueue::Update()
{
	PaperData sendData;
	sendData.ID = -1;	//�_�~�[
	sendData.from = -1;

	// �󂾂�����A��
	if (EventQueue.empty()){ return sendData; }// -1�̃_�~�[��Ԃ�

	// �擪�̃f�[�^�𑗂�
	sendData = EventQueue.front();

	// �f�[�^�𑗂�����|�b�v
	EventQueue.pop();

	// ����Ԃ�
	return sendData;
}
