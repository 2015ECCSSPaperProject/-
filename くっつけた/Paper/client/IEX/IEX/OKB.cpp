#include<windows.h>
#include<assert.h>

#include	"OKB.h"

OwatasoKeyBoard::OwatasoKeyBoard()
{
	// キー配列初期化
	for (int i = 0; i < 256; i++)
		info[i] = on_frame[i] = 0;
}

void OwatasoKeyBoard::Update()
{
	// 取得できたかどうかを確認 & infoにキー入力情報格納
	BOOL ret = GetKeyboardState(info);

	// 取得できなかったら強制終了(毎回止まるようであれば、コメントアウトしてください)
	assert(ret);

	//=========================
	//	全キー

	for (int index = 0; index < 240; index++){	// 240以降、有益なキーはないと思う…

		// 対応キーが押されているなら
		if (info[index] & 0x80)
			on_frame[index]++;		// その番号の入力フレームを加算

		else
			on_frame[index] = 0;	// そうでないならフレーム0にする(押されていない)
	}


	//=========================
	// 基本キーのみ
	//BYTE key_list[] =
	//{
	//	KB_ENTER, KB_SPACE,
	//	KB_0, KB_1, KB_2, KB_3, KB_4, KB_5, KB_6, KB_7, KB_8, KB_9,
	//	KB_A, KB_B, KB_C, KB_D, KB_E, KB_F, KB_G, KB_H, KB_I, KB_J, KB_K, KB_L, KB_M, KB_N, KB_O, KB_P, KB_Q, KB_R, KB_S, KB_T, KB_U, KB_V, KB_W, KB_X, KB_Y, KB_Z,
	//	KB_COMMA, KB_DOT, KB_HYPHEN, KB_COLON, KB_SEMICORON, KB_UP, KB_DOWN, KB_LEFT, KB_RIGHT,
	//	0x00
	//};

	//for (int index = 0; key_list[index] != 0x00; index++){

	//	// 対応キーが押されているなら
	//	if (info[key_list[index]] & 0x80)
	//		on_frame[key_list[index]]++;		// その番号の入力フレームを加算

	//	else
	//		on_frame[key_list[index]] = 0;	// そうでないならフレーム0にする(押されていない)

	//}


}

BYTE OwatasoKeyBoard::AnyTRG()
{
	//=========================
	//	全キー

	for (int index = 0; index < 240; index++)	// 240以降、有益なキーはないと思う…
	{
			// 何かのキー入力時間が1フレーム目だったらそれを返す
		if (on_frame[info[index]] == 1)
			return index;
	}

	//=========================
	// 基本キーのみ
	//BYTE key_list[] =
	//{
	//	KB_ENTER, KB_SPACE,
	//	KB_0, KB_1, KB_2, KB_3, KB_4, KB_5, KB_6, KB_7, KB_8, KB_9,
	//	KB_A, KB_B, KB_C, KB_D, KB_E, KB_F, KB_G, KB_H, KB_I, KB_J, KB_K, KB_L, KB_M, KB_N, KB_O, KB_P, KB_Q, KB_R, KB_S, KB_T, KB_U, KB_V, KB_W, KB_X, KB_Y, KB_Z,
	//	KB_COMMA, KB_DOT, KB_HYPHEN, KB_COLON, KB_SEMICORON,
	//	0x00
	//};

	//for (int index = 0; key_list[index] != 0x00; index++){

	//	// 何かのキー入力時間が1フレーム目だったらそれを返す
	//	if (on_frame[key_list[index]] == 1)
	//		return key_list[index];
	//}

	// このフレームで新しく押されたキーはなかったので0(キーコードなし)を返す
	return 0x00;

}


static OwatasoKeyBoard *key_board;

//*****************************************************************************************************************************
//		入力関連初期化・解放
//*****************************************************************************************************************************
void OKB_Init()
{
	key_board = new OwatasoKeyBoard;
}

void OKB_Release()
{
	if (key_board){
		delete key_board;
		key_board = nullptr;
	}
}


//*****************************************************************************************************************************
//		更新
//*****************************************************************************************************************************

void OKB_Update()
{
	key_board->Update();
}


//*****************************************************************************************************************************
//		キーアクセス関数
//*****************************************************************************************************************************

UINT KeyBoard(BYTE KeyCode){ return key_board->on_frame[KeyCode]; }
bool KeyBoardTRG(BYTE KeyCode, UINT frame) { return (key_board->on_frame[KeyCode] == frame); }

BYTE KeyBoardAnyTRG(){ return key_board->AnyTRG(); }