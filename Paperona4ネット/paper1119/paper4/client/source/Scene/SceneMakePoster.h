#pragma once

#include	"../../../share_data/Enum_public.h"

class SceneMakePoster : public Scene
{
public:
	//初期化・解放
	bool Initialize();
	~SceneMakePoster();
	//処理
	void Update();
	void CatchStamp();//　スタンプが近くにあれば選択
	//描画
	void Render();
	void PosterRender();
private:

	iex2DObj* posterScreen;
	Surface* backbuffer;// サーフェイス　

	//　ポスターのサイズとフレーム
	iex2DObj* posterSizePic;
	iex2DObj* posterFramePic;
	float posterScreen_W;//　場所X
	float posterScreen_H;//　場所Y

	iexView* view;
	iex2DObj* backPic;

	//　手
	iex2DObj* cursorPic;
	bool isPush;
	bool isTap;

private:

	/******************************/
	// スタンプの種類
	/******************************/


	//　種類MAX
	//static const int KIND_MAX = 3;
	//　一つの種類のスタンプ数のMAX
	//static const int NO_MAX = 10;

	//　スタンプの画像の2次元配列を作った
	iex2DObj* StampPicture[KIND_MAX][NO_MAX];

	// 種類分のファイル
	Vector3 FileSelectPos[KIND_MAX];
	iex2DObj* FilePic[KIND_MAX];
	void FileChange();//選択したら種類をチェンジ 

	/******************************/
	// スタンプ手帳
	/******************************/
	struct STAMP_DATA
	{
		/*ポジション*/
		float x, y;

		/*その画像のサイズや今つかまれてるか*/
		int size;

	};
	STAMP_DATA StampData[KIND_MAX][NO_MAX];

	int SelectKind;//　今選択してる種類の番号　(ページ)


	/******************************/
	// レイヤー
	/******************************/
	enum { NO_LAYER = -1 };
	//　レイヤーMAX
	//static const int LAYER_MAX = 10;
	//  レイヤー
	struct LAYER_DATA
	{
		/*種類とナンバー*/
		int kind;
		int num;

		float x, y;//	場所
		int size;//		サイズ
		bool isHold;//	掴んでるか
	};

	LAYER_DATA layer_data[LAYER_MAX];


private: //関数

	// スタンプの絵の初期化
	void StampInit();

	// レイヤーの初期化
	void LayerInit();

	//　マウス位置取得
	Vector3 GetMousePos();


	// レイヤーを追加
	void AddLayer(int kind, int no, STAMP_DATA stamp);

	// 背景をレイヤーに追加
	void AddBackLayer(int kind, int no, STAMP_DATA stamp);

	// レイヤーを消す
	void DeleteLayer();

	// レイヤーを動かす
	void MoveLayer();

	// レイヤーを掴む
	void CatchLayer();

	// レイヤーを一番上にする関数
	void ChangeTopLayer(int layer_num);

	// レイヤーを一番下にする関数
	void ChangeBottomLayer(int layer_num);

	// レイヤーを読み込む
	void LoadLayer();

	// レイヤーを書き出す
	void SaveLayer();

};
