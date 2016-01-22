#include	"iextreme.h"

#include	"../../../share_data/Enum_public.h"

#include	"../Net/Socket.h"
#include	"../system/Framework.h"
#include	"../system/Thread.h"
#include	"../../IEX/OKB.h"

#include	"SceneMakePoster.h"
#include	"SceneSelect.h"

#include	"../Net/Socket.h"

/*距離など当たり判定用*/
//****************************************************************
//		座標と座標の距離を返す
//****************************************************************
inline float Length(Vector3 PosA, Vector3 PosB)
{
	//  PosAとPosBとのベクトルを計算
	//（今回は距離を求めるだけなので方向は関係ない）
	Vector3 Vec = PosA - PosB;

	//  ベクトルの長さを三平方の定理から求め、求めたベクトルの長さを返す
	return sqrtf(Vec.x*Vec.x + Vec.y*Vec.y + Vec.z*Vec.z);
}


bool SceneMakePoster::Initialize()
{
	

	//	環境設定
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog(800, 3000, 0);

	view = new iexView();
	view->Set(Vector3(0, 0, -10), Vector3(0, 0, 0));

	//　カーソル
	cursorPic = new iex2DObj("DATA/makePoster/cursor4.png");
	isPush = false;
	isTap = false;
	ShowCursor(FALSE);//　マウスを描画しない

	// バック
	backPic = new iex2DObj("DATA/makePoster/back.png");

	// ポスターの場所
	posterSizePic = new iex2DObj("DATA/makePoster/PosterSize.png");
	posterFramePic = new iex2DObj("DATA/makePoster/Posterframe.png");
	posterScreen_W = 64.0f;
	posterScreen_H = 128.0f;

	// ファイル
	char fileName[128];
	for (int i = 0; i < KIND_MAX; i++)
	{
		
		sprintf(fileName, "DATA/makePoster/file/file_%d.png", i);
		FilePic[i] = new iex2DObj(fileName);

		// ファイルチェンジの目印
		FileSelectPos[i] = Vector3(750, 120 + ((float)i * 196), 0);
	}
	

	//スタンプの初期化
	StampInit();

	//レイヤーの初期化
	LayerInit();

	posterScreen = new iex2DObj(512, 512, IEX2D_RENDERTARGET);
	iexSystem::Device->GetRenderTarget(0, &backbuffer);//★描画先
	
	return true;
}


// スタンプの初期化
void SceneMakePoster::StampInit()
{
	//　イラスト初期化＆スタンプ手帳初期化
	for (int i = 0; i < KIND_MAX; i++)
	{
		for (int no = 0; no < NO_MAX; no++)
		{
			StampPicture[i][no] = NULL;
			//　スタンプのデータも全部０に
			ZeroMemory(&StampData[i][no], sizeof(STAMP_DATA));
		}
	}

	SelectKind = 0;// 今選択してる種類


	//イラストはここで入れていきます。
	StampPicture[KIND_NAME::OBJECT][0] = new iex2DObj("DATA/makePoster/stamp/yen.png");
	//↑の絵をどこに手帳ではどこに表示するかなどの情報を入れる
	StampData[KIND_NAME::OBJECT][0].x = 900;
	StampData[KIND_NAME::OBJECT][0].y = 100;
	StampData[KIND_NAME::OBJECT][0].size = 50;

	//イラストはここで入れていきます。
	StampPicture[KIND_NAME::OBJECT][1] = new iex2DObj("DATA/makePoster/stamp/coffee.png");
	//↑の絵をどこに手帳ではどこに表示するかなどの情報を入れる
	StampData[KIND_NAME::OBJECT][1].x = 900;
	StampData[KIND_NAME::OBJECT][1].y = 300;
	StampData[KIND_NAME::OBJECT][1].size = 80;

	//イラストはここで入れていきます。
	StampPicture[KIND_NAME::OBJECT][2] = new iex2DObj("DATA/makePoster/stamp/gentei.png");
	//↑の絵をどこに手帳ではどこに表示するかなどの情報を入れる
	StampData[KIND_NAME::OBJECT][2].x = 1090;
	StampData[KIND_NAME::OBJECT][2].y = 150;
	StampData[KIND_NAME::OBJECT][2].size = 60;

	//イラストはここで入れていきます。
	StampPicture[KIND_NAME::OBJECT][3] = new iex2DObj("DATA/makePoster/stamp/hamburger.png");
	//↑の絵をどこに手帳ではどこに表示するかなどの情報を入れる
	StampData[KIND_NAME::OBJECT][3].x = 1050;
	StampData[KIND_NAME::OBJECT][3].y = 500;
	StampData[KIND_NAME::OBJECT][3].size = 60;

	//ANIKI
	//イラストはここで入れていきます。
	StampPicture[KIND_NAME::FOOD][0] = new iex2DObj("DATA/makePoster/stamp/aniki.png");
	StampData[KIND_NAME::FOOD][0].x = 1050;
	StampData[KIND_NAME::FOOD][0].y = 450;
	StampData[KIND_NAME::FOOD][0].size = 110;

	//イラストはここで入れていきます。
	StampPicture[KIND_NAME::FOOD][1] = new iex2DObj("DATA/makePoster/stamp/歪みねぇな.png");
	StampData[KIND_NAME::FOOD][1].x = 1000;
	StampData[KIND_NAME::FOOD][1].y = 90;
	StampData[KIND_NAME::FOOD][1].size = 50;

	//イラストはここで入れていきます。
	StampPicture[KIND_NAME::FOOD][2] = new iex2DObj("DATA/makePoster/stamp/蟹.png");
	StampData[KIND_NAME::FOOD][2].x = 1000;
	StampData[KIND_NAME::FOOD][2].y = 170;
	StampData[KIND_NAME::FOOD][2].size = 50;

	//イラストはここで入れていきます。
	StampPicture[KIND_NAME::FOOD][3] = new iex2DObj("DATA/makePoster/stamp/だらしねぇな.png");
	StampData[KIND_NAME::FOOD][3].x = 1000;
	StampData[KIND_NAME::FOOD][3].y = 240;
	StampData[KIND_NAME::FOOD][3].size = 50;


	//背景
	//イラストはここで入れていきます。
	StampPicture[KIND_NAME::BACK ][0] = new iex2DObj("DATA/makePoster/stamp/back/not.png");
	//↑の絵をどこに手帳ではどこに表示するかなどの情報を入れる
	StampData[KIND_NAME::BACK][0].x = 900;
	StampData[KIND_NAME::BACK][0].y = 150;
	StampData[KIND_NAME::BACK][0].size = 90;

	//イラストはここで入れていきます。
	StampPicture[KIND_NAME::BACK][1] = new iex2DObj("DATA/makePoster/stamp/back/tehutehu.png");
	//↑の絵をどこに手帳ではどこに表示するかなどの情報を入れる
	StampData[KIND_NAME::BACK][1].x = 1100;
	StampData[KIND_NAME::BACK][1].y = 150;
	StampData[KIND_NAME::BACK][1].size = 90;

	//イラストはここで入れていきます。
	StampPicture[KIND_NAME::BACK][2] = new iex2DObj("DATA/makePoster/stamp/back/block.png");
	//↑の絵をどこに手帳ではどこに表示するかなどの情報を入れる
	StampData[KIND_NAME::BACK][2].x = 900;
	StampData[KIND_NAME::BACK][2].y = 300;
	StampData[KIND_NAME::BACK][2].size = 90;

	//イラストはここで入れていきます。
	StampPicture[KIND_NAME::BACK][3] = new iex2DObj("DATA/makePoster/stamp/back/midori.png");
	//↑の絵をどこに手帳ではどこに表示するかなどの情報を入れる
	StampData[KIND_NAME::BACK][3].x = 1100;
	StampData[KIND_NAME::BACK][3].y = 300;
	StampData[KIND_NAME::BACK][3].size = 90;

	//イラストはここで入れていきます。
	StampPicture[KIND_NAME::BACK][4] = new iex2DObj("DATA/makePoster/stamp/back/blue.png");
	//↑の絵をどこに手帳ではどこに表示するかなどの情報を入れる
	StampData[KIND_NAME::BACK][4].x = 900;
	StampData[KIND_NAME::BACK][4].y = 450;
	StampData[KIND_NAME::BACK][4].size = 90;

	//イラストはここで入れていきます。
	StampPicture[KIND_NAME::BACK][5] = new iex2DObj("DATA/makePoster/stamp/back/ゲイパレス.png");
	//↑の絵をどこに手帳ではどこに表示するかなどの情報を入れる
	StampData[KIND_NAME::BACK][5].x = 1100;
	StampData[KIND_NAME::BACK][5].y = 450;
	StampData[KIND_NAME::BACK][5].size = 90;


}

// レイヤーの初期化
void SceneMakePoster::LayerInit()
{
	for (int i = 0; i < LAYER_MAX; i++)
	{
		//　レイヤーのデータを全部０に
		ZeroMemory(&layer_data[i], sizeof(LAYER_DATA));

		layer_data[i].kind = NO_LAYER;//もし種類が-1だったら描画しない
	}

	//　背景だけ先に置いておく
	layer_data[(LAYER_MAX - 1)].kind = KIND_NAME::BACK;
	layer_data[(LAYER_MAX - 1)].num = 0;

	layer_data[(LAYER_MAX - 1)].x = posterScreen_W + 256;// 横固定
	layer_data[(LAYER_MAX - 1)].y = posterScreen_H + 256;// 縦固定
	layer_data[(LAYER_MAX - 1)].size = 90;


	//　★前のデータがあればレイヤー読み込み
	LoadLayer();

}


SceneMakePoster::~SceneMakePoster()
{
	//　スクリーン
	SAFE_DELETE(posterScreen);

	SAFE_DELETE(view);

	SAFE_DELETE(backPic);
	SAFE_DELETE(cursorPic);

	SAFE_DELETE(posterSizePic);
	SAFE_DELETE(posterFramePic);

	//　ファイル
	for (int i = 0; i < KIND_MAX; i++)
	{
		SAFE_DELETE(FilePic[i]);
	}

	//　スタンプ
	for (int i = 0; i < KIND_MAX; i++)
	{
		for (int no = 0; no < NO_MAX; no++)
		{
			SAFE_DELETE(StampPicture[i][no]);
		}
	}


}


bool SceneMakePoster::Update()
{
	//　マウスの処理
	isTap = false;//毎回掴む手に戻す
	if (KeyBoard(MOUSE_LEFT))	isPush = true;
	else	isPush = false;
	


	// ファイルのチェンジ
	FileChange();

	//スタンプ手帳からキャッチするぜ
	CatchStamp();

	//レイヤーを掴む&離すぜ
	CatchLayer();

	//つかんだスタンプ動かすぜ
	MoveLayer();

	// (仮)セーブ
	if (KEY_Get(KEY_ENTER)==3)
	{
		SaveLayer();
	}

	if (KEY_Get(KEY_SPACE) == 3)
	{
		MainFrame->ChangeScene(new SceneSelect());
		return true;
	}
	return true;
}

// 選択したらKindSelectをチェンジ
void SceneMakePoster::FileChange()
{
	for (int i = 0; i < KIND_MAX; i++)
	{
		float len = 0;
		len = Length(Vector3(GetMousePos().x, GetMousePos().y, 0),
			Vector3(FileSelectPos[i].x, FileSelectPos[i].y, 0));

		//　選択したフォルダ(種類)にチェンジ
		if (len < 50)
		{
			isTap = true;// 押すアイコンにチェンジ

			if (KeyBoard(MOUSE_LEFT) == 1)
			{
				SelectKind = i;
			}
		}

	}
}

//　手帳のスタンプが近くにあれば選択
void SceneMakePoster::CatchStamp()
{

	// スタンプ
	for (int i = 0; i < NO_MAX; i++)
	{
		//　絵があったら通す
		if (StampPicture[SelectKind][i] != NULL)
		{
			float len = 0;
			len = Length(Vector3(GetMousePos().x, GetMousePos().y, 0),
				Vector3(StampData[SelectKind][i].x, StampData[SelectKind][i].y, 0));
		
			//　近くに持てるやつがあれば
			if (len < StampData[SelectKind][i].size)
			{
				// ★　種類が背景だったらタップへ
				if (SelectKind == KIND_NAME::BACK)
				{
					isTap = true;//アイコンを押すに

					// 近くでボタンを押したら　押す♂
					if (KeyBoard(MOUSE_LEFT) == 1)
					{
						AddBackLayer(SelectKind, i, StampData[SelectKind][i]);// 背景レイヤー追加
					}

				}
				// 近くでボタンを押したら　レイヤーにこの絵を追加
				else if (KeyBoard(MOUSE_LEFT)==1)
				{
					AddLayer(SelectKind, i, StampData[SelectKind][i]);// レイヤー追加
					return;
				}

			}

		}
	}


}

// レイヤー追加
void SceneMakePoster::AddLayer(int kind, int no, STAMP_DATA stamp)
{
	//　空いてるレイヤーを探す
	for (int i = 0; i < LAYER_MAX; i++)
	{

		if (layer_data[i].kind == NO_LAYER)//　レイヤーが空だったらその場所に
		{
			//　その選んだ画像の種類とナンバー
			layer_data[i].kind = kind;
			layer_data[i].num = no;
		
			layer_data[i].x = stamp.x;
			layer_data[i].y = stamp.y;
			layer_data[i].size = stamp.size;
		

			ChangeTopLayer(i);// ★今作ったレイヤーを一番上へ

			return;
		}
		
	}
	
}

// 背景をレイヤー追加
void SceneMakePoster::AddBackLayer(int kind, int no, STAMP_DATA stamp)
{
	//　空いてるレイヤーを探す
	for (int i = 0; i < LAYER_MAX; i++)
	{

		//if (layer_data[i].kind == NO_LAYER)//　レイヤーが空だったらその場所に
		//{
		//	//　その選んだ画像の種類とナンバー
		//	layer_data[i].kind = kind;
		//	layer_data[i].num = no;

		//	layer_data[i].x = posterScreen_W+256;// 横固定
		//	layer_data[i].y = posterScreen_H+256;// 縦固定
		//	layer_data[i].size = stamp.size;


		//	ChangeBottomLayer(i);// ★今作ったレイヤーを一番下へ

		//	return;
		//}


		//　背景は一番下固定で　下の画像をすり替え
		layer_data[(LAYER_MAX-1)].kind = kind;
		layer_data[(LAYER_MAX - 1)].num = no;

		layer_data[(LAYER_MAX - 1)].x = posterScreen_W + 256;// 横固定
		layer_data[(LAYER_MAX - 1)].y = posterScreen_H + 256;// 縦固定
		layer_data[(LAYER_MAX - 1)].size = stamp.size;

		return;

	}

}

// 掴んでるレイヤーを消す
void SceneMakePoster::DeleteLayer()
{
	//　掴まれてるレイヤーを探す
	for (int i = 0; i < LAYER_MAX; i++)
	{

		if (layer_data[i].isHold == true)//　レイヤーが掴まれてる
		{
			//　レイヤーのデータを全部０に
			ZeroMemory(&layer_data[i], sizeof(LAYER_DATA));
			layer_data[i].kind = NO_LAYER;//もし種類が-1だったら描画しない

			return;
		}

	}

}

// 掴んだレイヤー動かす
void SceneMakePoster::MoveLayer()
{
	//　ホールド==trueのやつを動かす
	for (int i = 0; i < LAYER_MAX; i++)
	{
		//　持たれたらついてくるようにする
		if (layer_data[i].isHold == true)
		{
			layer_data[i].x = GetMousePos().x;
			layer_data[i].y = GetMousePos().y;
		}
	}

}

// レイヤーを掴んだり離したり
void SceneMakePoster::CatchLayer()
{
	// レイヤー分回る
	for (int i = 0; i < LAYER_MAX; i++)
	{
		//　空じゃなかったら通す かつ　背景じゃなかったら掴む
		if (layer_data[i].kind != NO_LAYER && layer_data[i].kind != KIND_NAME::BACK )
		{
			float len = 0;
			len = Length(Vector3(GetMousePos().x, GetMousePos().y, 0),
				Vector3(layer_data[i].x, layer_data[i].y, 0));

			//　近くに持てるやつがあれば
			if (len < layer_data[i].size)
			{
				// 近くでボタンを押したら　つかむ
				if (KeyBoard(MOUSE_LEFT) == 1)
				{
					ChangeTopLayer(i);
				//	ChangeBottomLayer(i);
					return;
				}
				// (仮)近くでボタンを離したら　離して置く
				if (KeyBoard(MOUSE_LEFT) == 0)
				{
					//■もしくは範囲外やごみ箱に置くとレイヤーを消す処理
					if (GetMousePos().x<posterScreen_W - (32) || GetMousePos().x>posterScreen_W + 512 + (32) ||
						GetMousePos().y<posterScreen_H - (32) || GetMousePos().y>posterScreen_H + 512 + (32))//(仮)
					{
						DeleteLayer();
					}

					//　消されなかったら全部ファルス(適当
					for (int i = 0; i < LAYER_MAX; i++)
					{
						layer_data[i].isHold = false;
					}

				}

			}

		}
	}



}

// レイヤーを一番上へ
void SceneMakePoster::ChangeTopLayer(int layer_num)
{
	layer_data[layer_num].isHold = true;
	if (layer_num != 0)//レイヤーNOが０だったら先頭なので処理をしない。
	{
		for (int i = layer_num; i > 0; i--)// -1とかないから０で終わる
		{
			LAYER_DATA temp = layer_data[i];//今のレイヤーをコピー

			layer_data[i] = layer_data[i - 1];//一つ手前のレイヤーを現在のレイヤーの場所に移す
			layer_data[i - 1] = temp;//一つ手前のレイヤーを現在のレイヤーの情報にする
		}
	}

}

// レイヤーを一番下へ
void SceneMakePoster::ChangeBottomLayer(int layer_num)
{
	//layer_data[layer_num].isHold = true;
	if (layer_num != (LAYER_MAX-1))//レイヤーNOが10だったら最後尾なので処理をしない。
	{
		for (int i = layer_num; i < (LAYER_MAX-1); i++)// +11とかないから10で終わる
		{
			LAYER_DATA temp = layer_data[i];//今のレイヤーをコピー

			layer_data[i] = layer_data[i + 1];//一つ後ろのレイヤーを現在のレイヤーの場所に移す
			layer_data[i + 1] = temp;//一つ後ろのレイヤーを現在のレイヤーの情報にする
		}
	}
}

// レイヤーを読み込む
void SceneMakePoster::LoadLayer()
{
	/* セーブしたレイヤーの読み込み　*/
	std::ifstream in("DATA/makePoster/text/Layer.txt");
	// バッファに読み込む
	if (in)//そのファイルがあるなら！
	{
		for (int i = 0; i < LAYER_MAX; i++)
		{
			in >> layer_data[i].kind;
			in >> layer_data[i].num;
			in >> layer_data[i].x;
			in >> layer_data[i].y;
			in >> layer_data[i].size;
			in >> layer_data[i].isHold;
		}

		in.close();
	}

}

// レイヤーを書き出す
void SceneMakePoster::SaveLayer()
{

	/* レイヤーのセーブ */
	std::ofstream out("DATA/makePoster/text/Layer.txt");
	for (int i = 0; i < LAYER_MAX; i++)
	{
		out << layer_data[i].kind	<< "\n";
		out << layer_data[i].num	<< "\n";
		out << layer_data[i].x		<< "\n";
		out << layer_data[i].y		<< "\n";
		out << layer_data[i].size	<< "\n";
		out << layer_data[i].isHold << "\n";

	}
//	std::fclose();

}

//******************************************************
//		マウス関連
//******************************************************
Vector3 SceneMakePoster::GetMousePos(){
	Vector3 pos;
	POINT Mpos;
	RECT rc;
	GetCursorPos(&Mpos);
	GetWindowRect(iexSystem::Window, &rc);
	//マウスの取得位置情報をウインドウ内に
	Mpos.x = (Mpos.x) - (rc.left + 8);
	Mpos.y = (Mpos.y) - (rc.top + 35);
	pos = Vector3((float)Mpos.x, (float)Mpos.y, 0.0f);
	return pos;
}

/*-----------------------------------*/
//		描画
/*-----------------------------------*/
void SceneMakePoster::Render()
{
	view->Activate();

	PosterRender();

	//バックバッファ
	iexSystem::Device->SetRenderTarget(0, backbuffer);
	view->Clear();

	//　後ろの背景
	backPic->Render(0, 0, 1280, 720, 0, 0, 1280, 720);

	// ポスターの場所
	posterSizePic->Render((int)posterScreen_W, (int)posterScreen_H, 512, 512, 0, 0, 512, 512);

	//　ファイル
	for (int i = 0; i < KIND_MAX; i++)
	{
		FilePic[i]->Render(700, 20, 512, 1024, 0, 0, 512, 1024);
	}
	//　選択したファイルを一番前に
	FilePic[SelectKind]->Render(700, 20, 512, 1024, 0, 0, 512, 1024);

	// スタンプ手帳の画像
	for (int no = (NO_MAX-1); no >= 0; no--)//■描画順を０が手前に来るように
	{
		//　NULLじゃなかったら描画する
		if (StampPicture[SelectKind][no] != NULL)
		{
			if (SelectKind==KIND_NAME::BACK)//背景は小さく描画
			{
				StampPicture[SelectKind][no]->Render((int)(StampData[SelectKind][no].x - 256 / 4),
					(int)(StampData[SelectKind][no].y - 256 / 4), 512 / 4, 512 / 4, 0, 0, 512, 512);
			}
			else
			{
				StampPicture[SelectKind][no]->Render((int)StampData[SelectKind][no].x - 256,
					(int)StampData[SelectKind][no].y - 256, 512, 512, 0, 0, 512, 512);
			}

		}
	}


	//　レイヤーの画像
	for (int i = (LAYER_MAX - 1); i >= 0; i--)//■描画順を０が手前に来るように
	{
		if (layer_data[i].kind != NO_LAYER)//　レイヤーが空だったら描画しない
		{
			if (layer_data[i].isHold==true)//掴んでいる物しか描画しない！
			{
			//　絵を表示
				StampPicture[layer_data[i].kind][layer_data[i].num]->SetARGB(0x66ffffff);
			StampPicture[layer_data[i].kind][layer_data[i].num]
				->Render((int)layer_data[i].x - 256, (int)layer_data[i].y - 256, 512, 512, 0, 0, 512, 512);
			}

		}

	}

	// ポスターのフレーム
	posterFramePic->Render((int)posterScreen_W, (int)posterScreen_H, 512, 512, 0, 0, 512, 512);

	//カーソルアイコン
	cursorPic->Render((int)GetMousePos().x - (32), (int)GetMousePos().y - (32), 64, 64, isPush * 64, isTap * 64, 64, 64);


	posterScreen->Render((int)posterScreen_W, (int)posterScreen_H, 512, 512, 0, 0, 512, 512);

	//	デバッグ文字
#ifdef _DEBUG

	for (int i = 0; i < LAYER_MAX; i++)
	{
		Text::Draw(10, 80 + (i * 30), 0xff0000ff, "レイヤー[%d]の種類->%d", i, layer_data[i].kind);
		Text::Draw(10, 400 + (i * 30), 0xff0000ff, "レイヤー[%d]のナンバー->%d", i, layer_data[i].num);
	}

	for (int i = 0; i < KIND_MAX; i++)
	{
		cursorPic->Render((int)FileSelectPos[i].x - 32, (int)FileSelectPos[i].y - 32, 64, 64, 0, 0, 64, 64);
	}
	
#endif

}

//　ポスターのテクスチャー絵
void SceneMakePoster::PosterRender()
{
	posterScreen->RenderTarget();
	view->Clear(0xffffffff);
	//　ここからポスターのテクスチャーに移る絵を描く

	//　レイヤーの画像
	for (int i = (LAYER_MAX - 1); i >= 0; i--)//■描画順を０が手前に来るように
	{
		if (layer_data[i].kind != NO_LAYER)//　レイヤーが空だったら描画しない
		{
			//　絵を表示　※ポスターの表示場所分ずらしている
			StampPicture[layer_data[i].kind][layer_data[i].num]
				->Render((int)(layer_data[i].x - 256 - (posterScreen_W)), (int)(layer_data[i].y - 256 - (posterScreen_H)), 512, 512, 0, 0, 512, 512);
		
		}
	}

	// ポスターのフレーム ※クライアントの番号によって色を後で分ける
	posterFramePic->Render(0, 0, 512, 512, 0, 0, 512, 512);

	//カーソルアイコン ※ポスターの表示場所分手もずらしている
	cursorPic->Render((int)(GetMousePos().x - (32) - (posterScreen_W)), (int)(GetMousePos().y - (32) - (posterScreen_H)), 64, 64, isPush * 64, 0, 64, 64);


}