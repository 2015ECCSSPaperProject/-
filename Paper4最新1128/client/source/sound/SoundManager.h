#pragma once

//********************************************************************************************************************
//
//		クラスの宣言
//
//********************************************************************************************************************
//		サウンドマネージャー(グローバル実体で関数呼び出し)
//****************************************************************************************************************

#include <map>
#include	"../../IEX/IEX_AudioIIDX.h"

class iexSoundBase;
class fstSoundSE;
class fstSoundBGM;

namespace SoundManager
{
	void Initialize();
	void Release();
	void Update();
	void SetFX(DXA_FX flag);
}

class SE_Manager
{
private:
	std::map<LPSTR, int> ID;	// char*型で番号を振り分ける

	fstSoundSE *play_manager;	// iexSound

	// データ検索
	//int Find_data_no(char *_ID);

	int Play_in(int data_num, bool loop);
	int Play_in(int data_num, float volume, bool loop);
	int Play_in(int data_num, const Vector3 &pos, const Vector3 &front, const Vector3 &move, bool loop);

public:

	//===============================================
	//	音データ
	//===============================================
	struct	DATA{
		LPSTR id;					// 呼び出す際に使用する識別コード
		LPSTR file_name;			// ファイルパス
		int play_simultaneously;	// 同一の音を複数再生する最大値
		bool b3D;					// 3Dサウンドフラグ
	};


	//===============================================
	//	初期化と解放
	//===============================================
	SE_Manager(){}
	~SE_Manager();
	void Initialize();

	//===============================================
	//	更		新
	//===============================================
	void Update();


	//===============================================
	//	処		理
	//===============================================
	int Play(LPSTR _ID, bool loop = false);																											// 簡易版
	int Play(LPSTR _ID, float volume, bool loop);																									// ボリューム設定版
	int Play(LPSTR _ID, const Vector3 &pos, const Vector3 &front = Vector3(0, 0, -1), const Vector3 &move = Vector3(0, 0, 0), bool loop = false);	// 3D設定版
	void Stop(LPSTR _ID, int no);																													// Playで返ってきた数値を入れる
	void Stop_all();																																// 全部止める
	bool isPlay(char *ID, int no);
	void SetFX(DXA_FX flag){ play_manager->SetFX(flag); }

	void Set_pos(LPSTR _ID, int no, const Vector3 &pos);
	void Set_listener(const Vector3 &pos, const Vector3 &front, const Vector3 &up, const Vector3 &move);											// リスナー情報
};

class BGM_Manager
{
private:
	std::map<LPSTR, int> ID;	// char*型で番号を振り分ける

	fstSoundBGM *play_manager;	// iexSound

	void Play_in(int data_num, bool loop);
	void Play_in(int data_num, float volume, bool loop);
	void Play_in(int data_num, const Vector3 &pos, const Vector3 &front, const Vector3 &move, bool loop);

public:

	int effect_no = 0;

	//===============================================
	//	音データ
	//===============================================
	struct	DATA{
		LPSTR id;					// 呼び出す際に使用する識別コード
		LPSTR file_name;			// ファイルパス
		bool b3D;
	};


	//===============================================
	//	初期化と解放
	//===============================================
	void Initialize();
	~BGM_Manager();

	//===============================================
	//	更		新
	//===============================================
	void Update();


	//===============================================
	//	処		理
	//===============================================
	void Play(LPSTR _ID, bool loop = true);																											// 簡易版
	void Play(LPSTR _ID, float volume, bool loop);																									// ボリューム設定版
	void Play(LPSTR _ID, const Vector3 &pos, const Vector3 &front = Vector3(0, 0, -1), const Vector3 &move = Vector3(0, 0, 0), bool loop = true);	// 3D設定版
	void Stop(LPSTR _ID);																															// Playで返ってきた数値を入れる
	void Stop_all();																																// 全部止める
	bool isPlay(LPSTR _ID);
	void Set_volume(LPSTR _ID, int vol);
	void Set_speed(LPSTR _ID, float speed);
	int Get_volume(LPSTR _ID){ return play_manager->GetVolume(ID[_ID]); }
	void Fade_in(LPSTR _ID, float fade_speed, bool loop = true);
	void Fade_out(LPSTR _ID, float fade_speed);
	void Fade_stop(LPSTR _ID){ play_manager->FadeStop(ID[_ID]); }
	void Cross_fade(LPSTR inID, LPSTR outID, float fade_speed = .0075f, bool loop = true);
	void Cross_fade(LPSTR inID, LPSTR outID, float in_speed, float out_speed, bool loop);
	void SetFX(DXA_FX flag){ play_manager->SetFX(flag); }

	void Set_listener(const Vector3 &pos, const Vector3 &front, const Vector3 &up, const Vector3 &move);											// リスナー情報
};

//=======================================================================================
//	イベントでいろいろ
class EventBGM
{
public:
	enum MODE
	{
		NONE, 	// 何もしない
		START, 	// ゲーム始まったら呼び出す
		KOUHAN, // 制限時間 1 / 4　ぐらいで呼び出す
		END,	// ゲーム終わったら呼び出す
		MAX
	};

private:
	char mainBGM[64];
	int step;
	int frame;
	int mainVolume;

	MODE mode;
	bool isKouhan;
	void None();
	void Start();
	void Kouhan();
	void End();
	void(EventBGM::*Mode_funk[MODE::MAX])();

public:

	// 実体取得
	static EventBGM *getInstance()
	{
		static EventBGM i;
		return &i;
	}

	// 初期化
	EventBGM();
	void Initialize(char *mainBGMname);

	// 更新
	void Update();

	void Set_mode(MODE m)
	{
		mode = m;
	}
	MODE Get_mode(){ return mode; }
	void Set_manhole(bool in);
};
#define event_bgm ( EventBGM::getInstance() )

//===============================================
//	実体(関数呼び出しはこれらを使う)
//===============================================
extern SE_Manager *se;
extern BGM_Manager *bgm;