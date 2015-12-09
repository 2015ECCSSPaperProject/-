#include <process.h>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

using namespace std;

// キューに贈るイベント
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

	// イベントキュー
	queue<Event> EventQueue;
	

	// 生死判定
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
	// キューへ送るデータ
	Event ReceiveData;
	ReceiveData.Nomber = 114514;	// 何番目の紙か？
	ReceiveData.PlayerNomber = 1;	// 何番目人？
	
	// ↑のイベントをキューへプッシュ!
	EventQueue.push(ReceiveData);

	// 空だったら帰れ
	if (EventQueue.empty()){ return; }

	// 先頭のデータを送る
	Event sendEvent = EventQueue.front();

	// データを送ったらポップ
	EventQueue.pop();

}
