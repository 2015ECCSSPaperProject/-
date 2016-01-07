//*****************************************************************************
//
//		�I�[�f�B�I�֘A
//
//*****************************************************************************

#define	STR_NORMAL	0
#define	STR_FADEIN	1
#define	STR_FADEOUT	2

#define	TYPE_WAV	0
#define	TYPE_OGG	1

enum class DXA_FX
{
	DXAFX_OFF = 0,		// �G�t�F�N�g�I�t
	DXAFX_CHORUS = 1 << 0,	// �R�[���X
	DXAFX_COMPRESSOR = 1 << 1,	// �R���v���b�T
	DXAFX_DISTORTION = 1 << 2,	// �f�B�X�g�\�V����
	DXAFX_ECHO = 1 << 3,	// �G�R�[
	DXAFX_FLANGER = 1 << 4,	// �t�����W
	DXAFX_GARGLE = 1 << 5,	// �K�[�O��
	DXAFX_ENVREVERB = 1 << 6,	// �����o�[�u
	DXAFX_PARAMEQ = 1 << 7,	// �p�����g���b�N�C�R���C�U
	DXAFX_WAVESREVERB = 1 << 8,	// �~���[�W�b�N���o�[�u(16bit�̃I�[�f�B�I�t�H�[�}�b�g�̂�)
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

	// Load����Ƃ��Ɏg���ϐ�(���[�J���˃����o�ϐ�)
	WAVEFORMATEX	wfx;
	LPBYTE			lpWBuf;
	DWORD			size;

	char			wav_file_path[128];	/*	WAVE�t�@�C���ւ̃p�X*/
	bool			loop_flag;

public:
	enum LOAD_FLAG{ OPEN, COPY };

	fstSoundBuffer(LPDIRECTSOUND8 lpDS, char* filename, bool b3D);
	fstSoundBuffer(){}
	~fstSoundBuffer();

	// �����̂𕡐��ǂݍ��݂����Ƃ��Ɏg��
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
	//	�T�E���h�G�t�F�N�g
	//------------------------------------------------------
	void SetFX(DXA_FX flag);

	//------------------------------------------------------
	//	3D�֌W
	//------------------------------------------------------
	void SetDist(float max_dist = DS3D_DEFAULTMAXDISTANCE, float min_dist = DS3D_DEFAULTMINDISTANCE);	// �ő咴����Ƃ����������Ȃ��A�ŏ��͕��ʂ�1�Ƃ��ł����Ǝv��
	void SetPos(const Vector3 &pos);																	// ��������������W
	void SetFront(const Vector3 &front);																// ���̌���
	void SetRange(int degreeIn = DS3D_DEFAULTCONEANGLE);												// ���͈͓̔��p�x�̐ݒ�(�x)
	void SetOutRange_volume(int out_vol = DS3D_DEFAULTCONEOUTSIDEVOLUME);								// �p�x�͈͊O�̏ꍇ�̉�(-10000(�ŏ�)�`0(�ő�))
	void SetMove(const Vector3 &move);																	// ���ʂ͓����Ȃ����ǃh�b�v���[���ʂŎg��
	void SetAll3D(float max_dist, float min_dist, const Vector3 &pos, const Vector3 &front, int degreeIn, int out_vol, const Vector3 &move);
};

class fstStreamSound
{
private:
	static const int STRSECOND = 1;
	int		rate;
	LPDIRECTSOUNDBUFFER	lpStream;		// �X�g���[���p�񎟃o�b�t�@
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
//		�T�E���h�Ǘ��̊��(�p���Ŏg�������ŁA�Q�[�����l�͎g�p���Ȃ�)
//
//*****************************************************************************
class fstSoundBase
{
protected:
	static const int WavNum = 128;	// �o�b�t�@�ő�l

	HWND	hWndWAV;

	LPDIRECTSOUND8			lpDS;		// DirectSound�I�u�W�F�N�g
	LPDIRECTSOUNDBUFFER	lpPrimary;		// �ꎟ�o�b�t�@

	LPDIRECTSOUND3DLISTENER8 lp3DListener;	// 3D�T�E���h��ŁA�����ƕ�������l�Ƃ̊֌W�ł����ƕ�������l�̏��

public:
	//------------------------------------------------------
	//	�������E���
	//------------------------------------------------------
	fstSoundBase();
	virtual ~fstSoundBase();

	//------------------------------------------------------
	// ��3D�T�E���h���� ���X�i�[���ݒ�
	//------------------------------------------------------
	void SetListenerPos(const Vector3 &pos);
	void SetListenerOrientation(const Vector3 &fornt, const Vector3 &up);
	void SetListenerMove(const Vector3 &move);
	void SetListenerAll(const Vector3 &pos, const Vector3 &fornt, const Vector3 &up, const Vector3 &velocity);
	void UpdateListener();	// 3D�T�E���h���g�p����ꍇ�A���񂱂���Ăяo���Ă�������
};


//*****************************************************************************
//
//		�T�E���h�Ǘ�(SE�p)
//
//*****************************************************************************

class fstSoundSE : public fstSoundBase
{
private:
	struct SEData
	{
		bool b3D;					// 3D�t���O
		fstSoundBuffer* buffer;	// �T�E���h�o�b�t�@
	};
	std::vector<SEData*> data[WavNum];

public:
	fstSoundSE();
	~fstSoundSE();

	//------------------------------------------------------------------------------------------------------------------------------------------------------
	//	�����cID:�ԍ��A num_of_play_simultaneously:�����Đ��\��(����܂葽���ƃ������H���̂łقǂقǂɁB5�`10�O��)�A�@�t�@�C�����A�@3D�T�E���h�t���O
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	void Set(int ID, int num_of_play_simultaneously, char* filename, bool b3D = false);

	/* �Đ� */
	int Play(int ID, bool loop = false);

	/* ��~:no�ɂ�Play�ŕԂ��Ă����l�������œ���� */
	void Stop(int ID, int no);
	void AllStop();

	/* �e�T�E���h�p�����[�^�ݒ�E�擾�֐� */
	void	SetVolume(int ID, int volume);
	void	SetVolume(int ID, float volume);	// 0�`1
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
	// ���ȉ��̊֐����ĂԂɂ�Set�̈�����b3D��true�ɂ���K�v������܂�
	//---------------------------------------------------------------
	int Play(int ID, const Vector3 &pos, const Vector3 &front = Vector3(0, 0, -1), const Vector3 &move = Vector3(0, 0, 0), bool loop = false);// �������ݒ肵�Đ�
	void SetDist(int ID, int no, float max_dist = DS3D_DEFAULTMAXDISTANCE, float min_dist = DS3D_DEFAULTMINDISTANCE);				// �ő咴����Ƃ����������Ȃ��A�ŏ��͕��ʂ�0�Ƃ��ł����Ǝv��
	void SetPos(int ID, int no, const Vector3 &pos);																				// ��������������W
	void SetFront(int ID, int no, const Vector3 &front);																			// ���̌���
	void SetRange(int ID, int no, int degreeIn = DS3D_DEFAULTCONEANGLE);															// ���͈͓̔��p�x�̐ݒ�(�x)
	void SetOutRange_volume(int ID, int no, int out_vol = DS3D_DEFAULTCONEOUTSIDEVOLUME);											// �p�x�͈͊O�̏ꍇ�̉�(-10000(�ŏ�)�`0(�ő�))
	void SetMove(int ID, int no, const Vector3 &move);																				// ���ʂ͓����Ȃ����ǃh�b�v���[���ʂŎg��
	void SetAll3D(int ID, int no, float max_dist, float min_dist, const Vector3 &pos, const Vector3 &front, int degreeIn, int out_vol, const Vector3 &move);
};



//*****************************************************************************
//
//		�T�E���h�Ǘ�(BGM�p)	�����̃N���X�̂݁A���t���[��Update���Ăяo���Ă�������
//
//*****************************************************************************

class fstSoundBGM : public fstSoundBase
{
private:
	static const int MinVolume = -2500;	// ���ʍŏ��l

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
		bool b3D;			// 3D�T�E���h�t���O
		MODE fade_mode;		// �t�F�[�h�C�����A�E�g���������Ȃ���
		float volume;		// ����(0�`1)
		float fade_speed;		// �t�F�[�h���x
		fstSoundBuffer* buffer;
	};
	std::vector<BGMData*> data;

public:
	fstSoundBGM();
	~fstSoundBGM();

	//////////////////////////////////////////////
	/* �����t���[��Update���Ăяo���Ă��������I */
	void Update();
	//////////////////////////////////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------
	//	�����cID:�ԍ��A�t�@�C�����A3D�T�E���h�t���O(BGM�̏ꍇ�A�g���@��͂قڂȂ�)
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	void Set(int ID, char* filename, bool b3D = false);

	/* �Đ� */
	void Play(int ID, bool loop = true, DWORD cursor = 0);

	/* ��~ */
	void Stop(int ID);
	void AllStop();
	void Pause(int ID);

	/* �t�F�[�h�֌W(BGM��SE�ŕ������v����1��) */
	void FadeOut(int ID, float fade_speed);	// fade_speed��0�`1�͈̔́B0.01�Ƃ��̒l�ɂȂ�
	void FadeIn(int ID, float fade_speed, bool loop = true, DWORD cursor = 0);

	enum class CROSS_FADE_TYPE
	{
		NORMAL,			// ���ʂɍŏ�����Đ�
		END_OF_ETERNITY	// EoE�`��(�C��������BGM���A�E�g������BGM�Ɠ����J�[�\������Đ�������)
	};
	void CrossFade(int inID, int outID, float fade_speed, CROSS_FADE_TYPE type = CROSS_FADE_TYPE::NORMAL, bool loop = true);
	void CrossFade(int inID, int outID, float in_speed, float out_speed, CROSS_FADE_TYPE type, bool loop);

	/* �e�T�E���h�p�����[�^�ݒ�E�擾�֐� */
	void	SetVolume(int ID, int volume);
	void	SetVolume(int ID, float volume);	// 0�`1
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
	// ���ȉ��̊֐����ĂԂɂ�Set�̈�����b3D��true�ɂ���K�v������܂�
	//---------------------------------------------------------------
	void Play(int ID, const Vector3 &pos, const Vector3 &front = Vector3(0, 0, -1), const Vector3 &move = Vector3(0, 0, 0), bool loop = true);// �������ݒ肵�Đ�
	void SetDist(int ID, int no, float max_dist = DS3D_DEFAULTMAXDISTANCE, float min_dist = DS3D_DEFAULTMINDISTANCE);				// �ő咴����Ƃ����������Ȃ��A�ŏ��͕��ʂ�0�Ƃ��ł����Ǝv��
	void SetPos(int ID, int no, const Vector3 &pos);																				// ��������������W
	void SetFront(int ID, int no, const Vector3 &front);																			// ���̌���
	void SetRange(int ID, int no, int degreeIn = DS3D_DEFAULTCONEANGLE);															// ���͈͓̔��p�x�̐ݒ�(�x)
	void SetOutRange_volume(int ID, int no, int out_vol = DS3D_DEFAULTCONEOUTSIDEVOLUME);											// �p�x�͈͊O�̏ꍇ�̉�(-10000(�ŏ�)�`0(�ő�))
	void SetMove(int ID, int no, const Vector3 &move);																				// ���ʂ͓����Ȃ����ǃh�b�v���[���ʂŎg��
	void SetAll3D(int ID, int no, float max_dist, float min_dist, const Vector3 &pos, const Vector3 &front, int degreeIn, int out_vol, const Vector3 &move);
};
