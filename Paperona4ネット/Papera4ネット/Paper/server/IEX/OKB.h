#ifndef OKB_H_
#define	OKB_H_

enum KEY_CODE
{
	//=============================
	// 基本キーボード
	KB_A		 = 0x41,
	KB_B		 = 0x42,
	KB_C		 = 0x43,
	KB_D		 = 0x44,
	KB_E		 = 0x45,
	KB_F		 = 0x46,
	KB_G		 = 0x47,
	KB_H		 = 0x48,
	KB_I		 = 0x49,
	KB_J		 = 0x4A,
	KB_K		 = 0x4B,
	KB_L		 = 0x4C,
	KB_M		 = 0x4D,
	KB_N		 = 0x4E,
	KB_O		 = 0x4F,
	KB_P		 = 0x50,
	KB_Q		 = 0x51,
	KB_R		 = 0x52,
	KB_S		 = 0x53,
	KB_T		 = 0x54,
	KB_U		 = 0x55,
	KB_V		 = 0x56,
	KB_W		 = 0x57,
	KB_X		 = 0x58,
	KB_Y		 = 0x59,
	KB_Z		 = 0x5A,
	KB_ENTER	 = 0x0D,
	KB_SPACE	 = 0x20,
	KB_LEFT		 = 0x25,	/* ← */
	KB_UP		 = 0x26,	/* ↑ */
	KB_RIGHT	 = 0x27,	/* → */
	KB_DOWN		 = 0x28,	/* ↓ */

	KB_0		 = 0x30,
	KB_1		 = 0x31,
	KB_2		 = 0x32,
	KB_3		 = 0x33,
	KB_4		 = 0x34,
	KB_5		 = 0x35,
	KB_6		 = 0x36,
	KB_7		 = 0x37,
	KB_8		 = 0x38,
	KB_9		 = 0x39,

	KB_COLON	 = 0xBA,	/* : */
	KB_SEMICORON = 0xBB,	/* ; */
	KB_COMMA	 = 0xBC,	/* , */
	KB_HYPHEN	 = 0xBD,	/* - */
	KB_DOT		 = 0xBE,	/* . */
	KB_SLASH	 = 0xBF,	/* / */
	KB_AT_SIGN	 = 0xC0,	/* @ */
	KB_SQUARE_L	 = 0xDB,	/* [ */
	KB_YEN		 = 0xDC,	/* \ */
	KB_SQUARE_R	 = 0xDD,	/* ] */
	KB_CARET	 = 0xDE,	/* ^ */



	//=============================
	// テンキー関連
	KB_NUMPAD0	 = 0x60,	/* 0 */
	KB_NUMPAD1	 = 0x61,	/* 1 */
	KB_NUMPAD2	 = 0x62,	/* 2 */
	KB_NUMPAD3	 = 0x63,	/* 3 */
	KB_NUMPAD4	 = 0x64,	/* 4 */
	KB_NUMPAD5	 = 0x65,	/* 5 */
	KB_NUMPAD6	 = 0x66,	/* 6 */
	KB_NUMPAD7	 = 0x67,	/* 7 */
	KB_NUMPAD8	 = 0x68,	/* 8 */
	KB_NUMPAD9	 = 0x69,	/* 9 */
	KB_MULT		 = 0x6A,	/* * */
	KB_ADD		 = 0x6B,	/* + */
	KB_SUBTRACT	 = 0x6D,	/* - */
	KB_DECIMAL	 = 0x6E,	/* . */
	KB_DIVIDE	 = 0x6F,	/* / */



	//=============================
	// あまり使うことはないかも…
	KB_BACK		 = 0x08,
	KB_TAB		 = 0x09,
	KB_CLEAR	 = 0x0c,

	KB_SHIFT	 = 0x10,
	KB_CTRL		 = 0x11,
	KB_ALT		 = 0x12,
	KB_PAUSE	 = 0x13,
	KB_CAPITAL	 = 0x14,	/* Shift + CapsLock */
	KB_ESC		 = 0x1B,
	KB_NON_CONV	 = 0x1D,	/* 無変換 */

	KB_PGUP		 = 0x21,
	KB_PGDN		 = 0x22,
	KB_END		 = 0x23,
	KB_HOME		 = 0x24,
	KB_PRT_SC	 = 0x2C,
	KB_INSERT	 = 0x2D,
	KB_DELETE	 = 0x2E,

	//=============================
	// マウス(キーではないが)
	MOUSE_LEFT	 = 0x01,
	MOUSE_RIGHT	 = 0x02,
	MOUSE_CENTAR = 0x04		/* ホイールクリック(ぐるぐるではない) */

};

class OwatasoKeyBoard
{
public:
	OwatasoKeyBoard();

	// キーボードの状態を格納
	BYTE info[256];

	// キーボードの押してる時間
	UINT on_frame[256];

	void Update();

	BYTE AnyTRG();
};


//====================================
//		初期化と解放

void OKB_Init();
void OKB_Release();


//====================================
//		キーボード情報更新

void OKB_Update();

// 対応しているキーが押されているフレーム数を返す( 押しっぱなし判定にどうぞ )
UINT KeyBoard(BYTE KeyCode);

// キーのフレームが引数frameと同じになったらtrue、それ以外だとfalse( 押した瞬間判定にどうぞ )
bool KeyBoardTRG(BYTE KeyCode, UINT frame = 1);

// 何かキーが押されたとき、1フレームだけそのキーのキーコードが返ってくる( タイピングゲームとかで使えそう )
BYTE KeyBoardAnyTRG();

/* 入力例

if(KeyBoard(KB_A))
if(KeyBoardTRG(KB_Z, 3))	// 押してから3フレーム目のみtrueが返る

*/

#endif
