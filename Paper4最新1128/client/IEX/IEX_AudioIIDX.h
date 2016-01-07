//*****************************************************************************
//
//		オーディオ関連
//
//*****************************************************************************

#define	STR_NORMAL	0
#define	STR_FADEIN	1
#define	STR_FADEOUT	2

#define	TYPE_WAV	0
#define	TYPE_OGG	1

enum class DXA_FX
{
	DXAFX_OFF = 0,		// エフェクトオフ
	DXAFX_CHORUS = 1 << 0,	// コーラス
	DXAFX_COMPRESSOR = 1 << 1,	// コンプレッサ
	DXAFX_DISTORTION = 1 << 2,	// ディスト―ション
	DXAFX_ECHO = 1 << 3,	// エコー
	DXAFX_FLANGER = 1 << 4,	// フランジ
	DXAFX_GARGLE = 1 << 5,	// ガーグル
	DXAFX_ENVREVERB = 1 << 6,	// 環境リバーブ
	DXAFX_PARAMEQ = 1 << 7,	// パラメトリックイコライザ
	DXAFX_WAVESREVERB = 1 << 8,	// ミュージックリバーブ(16bitのオーディオフォーマットのみ)
	DXAFX_MAX = 1 << 9
};

class fstSoundBuffer
{
protected:
	LPBYTE LoadWAV(LPSTR fname, LPDWORD size, LPWAVEFORMATEX wfx);
	LPDIRECTSOUNDBUFFER8	lpBuf;
	LPDIRECTSOUND3DBUFFER8	lpBuf3D;

	DWORD PlayCursor;
	DWORD BufferSize;
	WAVEFORMATEX format;

	// Loadするときに使う変数(ローカル⇒メンバ変数)
	WAVEFORMATEX	wfx;
	LPBYTE			lpWBuf;
	DWORD			size;

	char			wav_file_path[128];	/*	WAVEファイルへのパス*/
	bool			loop_flag;

public:
	enum LOAD_FLAG{ OPEN, COPY };

	fstSoundBuffer(LPDIRECTSOUND8 lpDS, char* filename, bool b3D);
	fstSoundBuffer(){}
	~fstSoundBuffer();

	// 同じのを複数読み込みたいときに使う
	static void Create_and_copy(LPDIRECTSOUND8 lpDS, char* filename, bool b3D, fstSoundBuffer **buffers, int dst, int count);

	LPDIRECTSOUNDBUFFER8 GetBuf(){ return lpBuf; }
	void Play(bool loop, DWORD cursor = 0);
	void Stop();
	void Pause();
	void PauseOff();

	void	SetVolume(int volume);
	int		GetVolume();
	void	SetPan(int pan);
	int		GetPan();
	void	SetFrequency(int pitch);
	int		GetFrequency();
	bool	isPlay();
	void	SetSpeed(float speed);
	float	GetSpeed();

	DWORD GetPlayCursor();
	void SetPlayCursor(DWORD new_position);
	DWORD GetPlayFrame();
	int GetPlaySecond();
	void SetPlaySecond(int sec);
	DWORD GetSize();
	int GetLengthSecond();

	void Get_file_path(char out[]){ sprintf_s(out, sizeof(out), "%s", wav_file_path); }


	//------------------------------------------------------
	//	サウンドエフェクト
	//------------------------------------------------------
	void SetFX(DXA_FX flag);

	//------------------------------------------------------
	//	3D関係
	//------------------------------------------------------
	void SetDist(float max_dist = DS3D_DEFAULTMAXDISTANCE, float min_dist = DS3D_DEFAULTMINDISTANCE);	// 最大超えるともう聞こえない、最少は普通に1とかでいいと思う
	void SetPos(const Vector3 &pos);																	// 音が発生する座標
	void SetFront(const Vector3 &front);																// 音の向き
	void SetRange(int degreeIn = DS3D_DEFAULTCONEANGLE);												// 音の範囲内角度の設定(度)
	void SetOutRange_volume(int out_vol = DS3D_DEFAULTCONEOUTSIDEVOLUME);								// 角度範囲外の場合の音(-10000(最少)～0(最大))
	void SetMove(const Vector3 &move);																	// 普通は動かないけどドップラー効果で使う
	void SetAll3D(float max_dist, float min_dist, const Vector3 &pos, const Vector3 &front, int degreeIn, int out_vol, const Vector3 &move);
};

class fstStreamSound
{
private:
	static const int STRSECOND = 1;
	int		rate;
	LPDIRECTSOUNDBUFFER	lpStream;		// ストリーム用二次バッファ
	DSBPOSITIONNOTIFY	pn[3];

	HANDLE	hStrThread;
	unsigned long dwThreadId;
	unsigned long dwThrdParam;

	FILE*	hStrFile;
	OggVorbis_File	vf;
	DWORD StrSize;
	DWORD StrPos;
	DWORD LoopPtr;

	BYTE mode;
	int	param;
	int volume;

	void Initialize(LPDIRECTSOUND lpDS, int rate);

	BOOL	OGGRead(LPBYTE dst, unsigned long size);

public:
	BYTE	type;
	HANDLE	hEvent[3];
	LPDIRECTSOUNDNOTIFY	lpStrNotify;


	fstStreamSound(LPDIRECTSOUND lpDS, LPSTR filename, BYTE mode, int param);
	~fstStreamSound();

	BOOL	SetBlockOGG(int block);
	BOOL	SetBlockWAV(int block);

	BOOL	SetWAV(LPDIRECTSOUND lpDS, char* filename);
	BOOL	SetOGG(LPDIRECTSOUND lpDS, char* filename);

	void	Stop();
	void	SetVolume(int volume);
	int		GetVolume(){ return volume; }

	void SetMode(BYTE mode, int param);
	BYTE GetMode(){ return mode; }
	int GetParam(){ return param; }

};

typedef fstStreamSound DSSTREAMIIDX, *LPDSSTREAMIIDX;




//*****************************************************************************
//
//		サウンド管理の基底(継承で使うだけで、ゲーム作る人は使用しない)
//
//*****************************************************************************
class fstSoundBase
{
protected:
	static const int WavNum = 128;	// バッファ最大値

	HWND	hWndWAV;

	LPDIRECTSOUND8			lpDS;		// DirectSoundオブジェクト
	LPDIRECTSOUNDBUFFER	lpPrimary;		// 一次バッファ

	LPDIRECTSOUND3DLISTENER8 lp3DListener;	// 3Dサウンド上で、音源と聞こえる人との関係でいうと聞こえる人の情報

public:
	//------------------------------------------------------
	//	初期化・解放
	//------------------------------------------------------
	fstSoundBase();
	virtual ~fstSoundBase();

	//------------------------------------------------------
	// ※3Dサウンド限定 リスナー情報設定
	//------------------------------------------------------
	void SetListenerPos(const Vector3 &pos);
	void SetListenerOrientation(const Vector3 &fornt, const Vector3 &up);
	void SetListenerMove(const Vector3 &move);
	void SetListenerAll(const Vector3 &pos, const Vector3 &fornt, const Vector3 &up, const Vector3 &velocity);
	void UpdateListener();	// 3Dサウンドを使用する場合、毎回これを呼び出してください
};


//*****************************************************************************
//
//		サウンド管理(SE用)
//
//*****************************************************************************

class fstSoundSE : public fstSoundBase
{
private:
	struct SEData
	{
		bool b3D;					// 3Dフラグ
		fstSoundBuffer* buffer;	// サウンドバッファ
	};
	std::vector<SEData*> data[WavNum];

public:
	fstSoundSE();
	~fstSoundSE();

	//------------------------------------------------------------------------------------------------------------------------------------------------------
	//	引数…ID:番号、 num_of_play_simultaneously:同時再生可能数(あんまり多いとメモリ食うのでほどほどに。5～10前後)、　ファイル名、　3Dサウンドフラグ
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	void Set(int ID, int num_of_play_simultaneously, char* filename, bool b3D = false);

	/* 再生 */
	int Play(int ID, bool loop = false);

	/* 停止:noにはPlayで返ってきた値を自分で入れる */
	void Stop(int ID, int no);
	void AllStop();

	/* 各サウンドパラメータ設定・取得関数 */
	void	SetVolume(int ID, int volume);
	void	SetVolume(int ID, float volume);	// 0～1
	int		GetVolume(int ID);
	void	SetPan(int ID, int pan);
	int		GetPan(int ID);
	void	SetFrequency(int ID, int pitch);
	void	SetFrequency(int ID, int no, int pitch);
	int		GetFrequency(int ID, int no);
	void	SetSpeed(int ID, float speed);
	void	SetSpeed(int ID, int no, float speed);
	bool	isPlay(int ID, int no);
	void	SetFX(DXA_FX flag);
	void	SetFX(int ID, DXA_FX flag);


	//---------------------------------------------------------------
	// ※以下の関数を呼ぶにはSetの引数のb3Dをtrueにする必要があります
	//---------------------------------------------------------------
	int Play(int ID, const Vector3 &pos, const Vector3 &front = Vector3(0, 0, -1), const Vector3 &move = Vector3(0, 0, 0), bool loop = false);// 音源情報設定しつつ再生
	void SetDist(int ID, int no, float max_dist = DS3D_DEFAULTMAXDISTANCE, float min_dist = DS3D_DEFAULTMINDISTANCE);				// 最大超えるともう聞こえない、最少は普通に0とかでいいと思う
	void SetPos(int ID, int no, const Vector3 &pos);																				// 音が発生する座標
	void SetFront(int ID, int no, const Vector3 &front);																			// 音の向き
	void SetRange(int ID, int no, int degreeIn = DS3D_DEFAULTCONEANGLE);															// 音の範囲内角度の設定(度)
	void SetOutRange_volume(int ID, int no, int out_vol = DS3D_DEFAULTCONEOUTSIDEVOLUME);											// 角度範囲外の場合の音(-10000(最少)～0(最大))
	void SetMove(int ID, int no, const Vector3 &move);																				// 普通は動かないけどドップラー効果で使う
	void SetAll3D(int ID, int no, float max_dist, float min_dist, const Vector3 &pos, const Vector3 &front, int degreeIn, int out_vol, const Vector3 &move);
};



//*****************************************************************************
//
//		サウンド管理(BGM用)	※このクラスのみ、毎フレームUpdateを呼び出してください
//
//*****************************************************************************

class fstSoundBGM : public fstSoundBase
{
private:
	static const int MinVolume = -2500;	// 音量最少値

	enum class MODE
	{
		NONE, FADE_IN, FADE_OUT, MAX
	};
	void None(int no);
	void In(int no);
	void Out(int no);
	void(fstSoundBGM::*Fade_funk[(int)MODE::MAX])(int);

	struct BGMData
	{
		bool b3D;			// 3Dサウンドフラグ
		MODE fade_mode;		// フェードインかアウトか何もしないか
		float volume;		// 音量(0～1)
		float fade_speed;		// フェード速度
		fstSoundBuffer* buffer;
	};
	std::vector<BGMData*> data;

public:
	fstSoundBGM();
	~fstSoundBGM();

	//////////////////////////////////////////////
	/* ※毎フレームUpdateを呼び出してください！ */
	void Update();
	//////////////////////////////////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------
	//	引数…ID:番号、ファイル名、3Dサウンドフラグ(BGMの場合、使う機会はほぼない)
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	void Set(int ID, char* filename, bool b3D = false);

	/* 再生 */
	void Play(int ID, bool loop = true, DWORD cursor = 0);

	/* 停止 */
	void Stop(int ID);
	void AllStop();
	void Pause(int ID);

	/* フェード関係(BGMとSEで分けた要因の1つ) */
	void FadeOut(int ID, float fade_speed);	// fade_speedは0～1の範囲。0.01とかの値になる
	void FadeIn(int ID, float fade_speed, bool loop = true, DWORD cursor = 0);

	enum class CROSS_FADE_TYPE
	{
		NORMAL,			// 普通に最初から再生
		END_OF_ETERNITY	// EoE形式(インさせるBGMをアウトさせるBGMと同じカーソルから再生させる)
	};
	void CrossFade(int inID, int outID, float fade_speed, CROSS_FADE_TYPE type = CROSS_FADE_TYPE::NORMAL, bool loop = true);
	void CrossFade(int inID, int outID, float in_speed, float out_speed, CROSS_FADE_TYPE type, bool loop);

	/* 各サウンドパラメータ設定・取得関数 */
	void	SetVolume(int ID, int volume);
	void	SetVolume(int ID, float volume);	// 0～1
	int		GetVolume(int ID);
	void	SetPan(int ID, int pan);
	int		GetPan(int ID);
	void	SetFrequency(int ID, int pitch);
	int		GetFrequency(int ID, int no);
	void	SetSpeed(int ID, float speed);
	void	SetSpeed(int ID, int no, float speed);
	bool	isPlay(int ID);
	DWORD	GetPlayCursor(int ID);
	DWORD	GetPlayFrame(int ID);
	int 	GetPlaySecond(int ID);
	void	SetPlaySecond(int ID, int sec);
	DWORD	GetSize(int ID);
	int		GetLengthSecond(int ID);
	void	SetFX(DXA_FX flag);
	void	SetFX(int ID, DXA_FX flag);

	fstStreamSound* PlayStream(char* filename);
	fstStreamSound* PlayStream(char* filename, BYTE mode, int param);


	//---------------------------------------------------------------
	// ※以下の関数を呼ぶにはSetの引数のb3Dをtrueにする必要があります
	//---------------------------------------------------------------
	void Play(int ID, const Vector3 &pos, const Vector3 &front = Vector3(0, 0, -1), const Vector3 &move = Vector3(0, 0, 0), bool loop = true);// 音源情報設定しつつ再生
	void SetDist(int ID, int no, float max_dist = DS3D_DEFAULTMAXDISTANCE, float min_dist = DS3D_DEFAULTMINDISTANCE);				// 最大超えるともう聞こえない、最少は普通に0とかでいいと思う
	void SetPos(int ID, int no, const Vector3 &pos);																				// 音が発生する座標
	void SetFront(int ID, int no, const Vector3 &front);																			// 音の向き
	void SetRange(int ID, int no, int degreeIn = DS3D_DEFAULTCONEANGLE);															// 音の範囲内角度の設定(度)
	void SetOutRange_volume(int ID, int no, int out_vol = DS3D_DEFAULTCONEOUTSIDEVOLUME);											// 角度範囲外の場合の音(-10000(最少)～0(最大))
	void SetMove(int ID, int no, const Vector3 &move);																				// 普通は動かないけどドップラー効果で使う
	void SetAll3D(int ID, int no, float max_dist, float min_dist, const Vector3 &pos, const Vector3 &front, int degreeIn, int out_vol, const Vector3 &move);
};
