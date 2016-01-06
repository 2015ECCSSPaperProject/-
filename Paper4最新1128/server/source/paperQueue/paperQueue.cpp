
#include "paperQueue.h"

// 初期化
void PaperQueue::Clear()
{
	std::lock_guard<std::mutex> lock( mtx );
	// 空になるまでポップ
	for( ;; )
	{
		// 空だったら帰れ
		if( EventQueue.empty() )
		{
			return;
		}// 

		EventQueue.pop();

	}
}

void PaperQueue::Push( PaperData pData )
{
	std::lock_guard<std::mutex> lock( mtx );
	// ↑のイベントをキューへプッシュ!
	EventQueue.push( pData );
}

PaperData PaperQueue::Update()
{
	std::lock_guard<std::mutex> lock( mtx );

	PaperData sendData;
	sendData.ID = -1;	//ダミー
	sendData.from = -1;

	// 空だったら帰れ
	if( EventQueue.empty() )
	{
		return sendData;
	}// -1のダミーを返す

	// 先頭のデータを送る
	sendData = EventQueue.front();

	// データを送ったらポップ
	EventQueue.pop();

	// 情報を返す
	return sendData;
}
