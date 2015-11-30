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

class SoundBufferIIDX
{
protected:
	LPBYTE LoadWAV(LPSTR fname, LPDWORD size, LPWAVEFORMATEX wfx);
	LPDIRECTSOUNDBUFFER		lpBuf;
	LPDIRECTSOUND3DBUFFER	lpBuf3D;

	DWORD PlayCursor;
	DWORD BufferSize;
	WAVEFORMATEX format;

	// Loadするときに使う変数(ローカル⇒メンバ変数)
	WAVEFORMATEX	wfx;
	LPBYTE			lpWBuf;
	DWORD			size;

	char			wav_file_path[128];	/*	WAVEファイルへのパス*/

public:
	enum LOAD_FLAG{ OPEN, COPY };
	
	SoundBufferIIDX(LPDIRECTSOUND lpDS, char* filename, bool b3D);
	SoundBufferIIDX(){}
	~SoundBufferIIDX();

	// 引数のbuffers[]を**buffersに変えたらバグ直った！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	static void Create_and_copy(LPDIRECTSOUND lpDS, char* filename, bool b3D, SoundBufferIIDX **buffers, int dst, int count);

	LPDIRECTSOUNDBUFFER GetBuf(){ return lpBuf; }
	void Play(BOOL loop);
	void Stop();
	void Pause();

	void	SetVolume(int volume);
	int		GetVolume();
	void	SetPan(int pan);
	int		GetPan();
	void	SetFrequency(int pitch);
	int		GetFrequency();
	BOOL	isPlay();
	void	SetSpeed(float speed);
	float	GetSpeed();

	DWORD GetPlayCursor();
	DWORD GetPlayFrame();
	int GetPlaySecond();
	void SetPlaySecond(int sec);
	DWORD GetSize();
	int GetLengthSecond();

	void Get_file_path(char out[]){ sprintf_s(out, sizeof(out), "%s", wav_file_path); }


	//------------------------------------------------------
	//	3D関係
	//------------------------------------------------------
	void Set_dist(float max_dist = DS3D_DEFAULTMAXDISTANCE, float min_dist = DS3D_DEFAULTMINDISTANCE);	// 最大超えるともう聞こえない、最少は普通に1とかでいいと思う
	void Set_pos(const Vector3 &pos);																	// 音が発生する座標
	void Set_front(const Vector3 &front);																// 音の向き
	void Set_range(int degreeIn = DS3D_DEFAULTCONEANGLE);												// 音の範囲内角度の設定(度)
	void Set_outRange_volume(int out_vol = DS3D_DEFAULTCONEOUTSIDEVOLUME);								// 角度範囲外の場合の音(-10000(最少)～0(最大))
	void Set_move(const Vector3 &move);																	// 普通は動かないけどドップラー効果で使う
	void Set_all3D(float max_dist, float min_dist, const Vector3 &pos, const Vector3 &front, int degreeIn, int out_vol, const Vector3 &move);
};

class iexStreamSoundIIDX
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


	iexStreamSoundIIDX(LPDIRECTSOUND lpDS, LPSTR filename, BYTE mode, int param);
	~iexStreamSoundIIDX();

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

typedef iexStreamSoundIIDX DSSTREAM_IIDX, *LPDSSTREAM_IIDX;

class iexSoundIIDX
{
protected:
	static const int WavNum = 128;

	HWND	hWndWAV;

	LPDIRECTSOUND8			lpDS;		// DirectSoundオブジェクト
	LPDIRECTSOUNDBUFFER	lpPrimary;		// 一次バッファ

	LPDIRECTSOUND3DLISTENER lp3DListener;

	SoundBufferIIDX* buffer[WavNum];

public:
	iexSoundIIDX();
	virtual ~iexSoundIIDX();

	virtual void Set(int no, char* filename, bool b3D);
	virtual void Set_copy(char* filename, int dst, int count, bool b3D);			// Cloneみたいに複製
	virtual void Play(int no, BOOL loop = FALSE);
	void Stop(int no);
	void Pause(int no);



	void	SetVolume(int no, int volume);
	int		GetVolume(int no);
	void	SetPan(int no, int pan);
	int		GetPan(int no);
	void	SetFrequency(int no, int pitch);
	int		GetFrequency(int no);
	void	SetSpeed(int no, float speed);
	float	GetSpeed(int no);

	BOOL	isPlay(int no);
	DWORD	GetPlayCursor(int no);
	DWORD	GetPlayFrame(int no);
	int 	GetPlaySecond(int no);
	void	SetPlaySecond(int no, int sec);
	DWORD	GetSize(int no);
	int		GetLengthSecond(int no);

	iexStreamSoundIIDX* PlayStream(char* filename);
	iexStreamSoundIIDX* PlayStream(char* filename, BYTE mode, int param);

};


class iex3DSoundIIDX : public iexSoundIIDX
{
public:
	iex3DSoundIIDX();
	~iex3DSoundIIDX();

	void Set_listener_pos(const Vector3 &pos);
	void Set_listener_orientation(const Vector3 &fornt, const Vector3 &up);
	void Set_listener_move(const Vector3 &move);
	void Set_roll_off_factor(float pow);
	void Set_listener_all(const Vector3 &pos, const Vector3 &fornt, const Vector3 &up, const Vector3 &velocity);
	void Update();	// 毎回これを呼び出してください


	//=======================================================
	//	サウンド制御関数
	virtual void Play(int no, BOOL loop = FALSE){ iexSoundIIDX::Play(no, loop); }
	void Play(int no, const Vector3 &pos, const Vector3 &front = Vector3(0, 0, -1), BOOL loop = FALSE);

	void Set_dist(int no, float max_dist = DS3D_DEFAULTMAXDISTANCE, float min_dist = DS3D_DEFAULTMINDISTANCE);	// 最大超えるともう聞こえない、最少は普通に1とかでいいと思う
	void Set_pos(int no, const Vector3 &pos);																	// 音が発生する座標
	void Set_front(int no, const Vector3 &front);																// 音の向き
	void Set_range(int no, int degreeIn = DS3D_DEFAULTCONEANGLE);												// 音の範囲内角度の設定(度)
	void Set_outRange_volume(int no, int out_vol = DS3D_DEFAULTCONEOUTSIDEVOLUME);								// 角度範囲外の場合の音(-10000(最少)～0(最大))
	void Set_move(int no, const Vector3 &move);																	// 普通は動かないけどドップラー効果で使う
	void Set_all3D(int no, float max_dist, float min_dist, const Vector3 &pos, const Vector3 &front, int degreeIn, int out_vol, const Vector3 &move);
};