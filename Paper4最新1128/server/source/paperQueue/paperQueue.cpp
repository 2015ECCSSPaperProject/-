
#include "paperQueue.h"

// 初期化
void PaperQueue::Clear()
{
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
	// ↑のイベントをキューへプッシュ!
	EventQueue.push( pData );
}

PaperData PaperQueue::Update()
{
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
