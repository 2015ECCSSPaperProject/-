#include <process.h>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

using namespace std;

// キューに贈るイベント
class PaperData
{
public:
	int ID;	//何番が
	int from; //誰から
};

class PaperQueue
{
public:

	// 初期化
	static void Clear();

	// キューに詰める
	static void Push(PaperData pData);

	// 更新
	static PaperData Update();

private:

	// PaperDataキュー
	static queue<PaperData> EventQueue;

};
 
// 宣言
queue<PaperData> PaperQueue::EventQueue;

// 初期化
void PaperQueue::Clear()
{
	// 空になるまでポップ
	for (;;)
	{
		// 空だったら帰れ
		if (EventQueue.empty()){ return; }// 

		EventQueue.pop();

	}

}

void PaperQueue::Push(PaperData pData)
{
	// ↑のイベントをキューへプッシュ!
	EventQueue.push(pData);
}

PaperData PaperQueue::Update()
{
	PaperData sendData;
	sendData.ID = -1;	//ダミー
	sendData.from = -1;

	// 空だったら帰れ
	if (EventQueue.empty()){ return sendData; }// -1のダミーを返す

	// 先頭のデータを送る
	sendData = EventQueue.front();

	// データを送ったらポップ
	EventQueue.pop();

	// 情報を返す
	return sendData;
}
