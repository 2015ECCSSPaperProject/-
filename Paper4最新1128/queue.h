#include <process.h>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

using namespace std;

// �L���[�ɑ���C�x���g
class Event
{
public:
	int Nomber;
	int PlayerNomber;
};

class Queue
{
public:
	Queue();
	~Queue();

	void Update();

private:

	// �C�x���g�L���[
	queue<Event> EventQueue;
	

	// ��������
	atomic_bool isAlive;

};

Queue::Queue()
{

}

Queue::~Queue()
{

}

void Queue::Update()
{
	// �L���[�֑���f�[�^
	Event ReceiveData;
	ReceiveData.Nomber = 114514;	// ���Ԗڂ̎����H
	ReceiveData.PlayerNomber = 1;	// ���Ԗڐl�H
	
	// ���̃C�x���g���L���[�փv�b�V��!
	EventQueue.push(ReceiveData);

	// �󂾂�����A��
	if (EventQueue.empty()){ return; }

	// �擪�̃f�[�^�𑗂�
	Event sendEvent = EventQueue.front();

	// �f�[�^�𑗂�����|�b�v
	EventQueue.pop();

}
