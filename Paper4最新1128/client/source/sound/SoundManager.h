#ifndef SOUNDMANAGER_H_
#define SOUNDMANAGER_H_

//********************************************************************************************************************
//
//		�N���X�̐錾
//
//********************************************************************************************************************
//		�T�E���h�}�l�[�W���[
//****************************************************************************************************************

class iexSoundIIDX;


//=======================================================================================
//	�T�E���h���̊Ǘ�
namespace SoundManager
{
	void Initialize();
	void Release();
	void Update();
}



//=======================================================================================
//	�T�E���h���N���X
class SoundBase
{
protected:
	iexSoundIIDX *play_manager;	// iexSound
	int data_no;				// wav�f�[�^�̎��(�ԍ�)


public:
	virtual ~SoundBase();
	enum { NOT_FOUND = -1 };

	//===============================================
	//	�f�[�^����
	//===============================================
	virtual int Find_data_no(char *_ID) = 0;
};



//=======================================================================================
//	SE�Ǘ��N���X
class SE : public SoundBase
{
private:
	int max_count;				// �Z�b�g�����f�[�^��

	int Play_in(int no, float volume, bool loop);

public:

	//===============================================
	//	���f�[�^
	//===============================================
	struct	DATA{
		char *id;					// �Ăяo���ۂɎg�p���鎯�ʃR�[�h
		char *file_name;			// �t�@�C���p�X
		int same_play_max;			// ����̉��𕡐��Đ�����ő�l
		float volume;				// �{�����[��

		int start_num;				// 1��wav�f�[�^������ start_num �` start_num + same_play_max ���̃f�[�^��ێ�
	};


	//===============================================
	//	�������Ɖ��
	//===============================================
	void Initialize();
	//void Release();

	//===============================================
	//	�X		�V
	//===============================================
	void Update();


	//===============================================
	//	��		��
	//===============================================
	int Play(char *_ID, bool loop = false);			// �ȈՔ�
	int Play(char *_ID, float volume, bool loop);	// �{�����[���ݒ��
	void Stop(int no);								// Play�ŕԂ��Ă������l������
	void Stop_all();								// �S���~�߂�


	//===============================================
	//	�f�[�^����
	//===============================================
	int Find_data_no(char *_ID);
};


//=======================================================================================
//	BGM�Ǘ��N���X
class BGM : public SoundBase
{
private:

	int Play_in(int no, float volume, bool loop);

public:
	enum F_TYPE{ FADE_NONE = 0, FADE_IN, FADE_OUT };
	void None(int no);
	void In(int no);
	void Out(int no);
	void(BGM::*Fade_mode_funk[3])(int);

	//===============================================
	//	���f�[�^
	//===============================================
	struct	DATA{
		char *id;					// �Ăяo���ۂɎg�p���鎯�ʃR�[�h
		char *file_name;			// �t�@�C���p�X
		float volume;				// �{�����[��

		float fade_speed;			// �t�F�[�h�X�s�[�h
		F_TYPE fade_type;			// �֐������򂷂�
	};


	//===============================================
	//	�������Ɖ��
	//===============================================
	void Initialize();
	//void Release();

	//===============================================
	//	�X		�V
	//===============================================
	void Update();


	//===============================================
	//	��		��
	//===============================================
	int Play(char *_ID, bool loop = true);				// �ȈՔ�
	int Play(char *_ID, float volume, bool loop);		// �t���ݒ��
	int Fade_in(char *_ID, int speed, bool loop = true);// �t�F�[�h�C��

	void Stop(char *_ID);								// ��~
	void Fade_out(char *_ID, int speed);				// �t�F�[�h�A�E�g
	void Stop_all();									// �S���~�߂�
	void Pause(char *_ID);								// �|�[�Y
	bool isPlay(char *_ID);

	void Set_speed(char *_ID, float speed);				// �Đ����x�ύX

	//===============================================
	//	�f�[�^����
	//===============================================
	int Find_data_no(char *_ID);
};


//=======================================================================================
//	�C�x���g�ł��낢��
class EventBGM
{
public:
	enum MODE
	{
		NONE, 	// �������Ȃ�
		START, 	// �Q�[���n�܂�����Ăяo��
		KOUHAN, // �������� 1 / 4�@���炢�ŌĂяo��
		MAX 
	};

private:
	MODE mode;
	void None(){}
	void Start();
	void Kouhan();
	void(EventBGM::*Mode_funk[MODE::MAX])();

public:

	// ���̎擾
	static EventBGM *getInstance()
	{
		static EventBGM i;
		return &i;
	}

	// ������
	EventBGM();

	// �X�V
	void Update();

	void Set_mode(MODE m)
	{
		mode = m;
	}
};
#define event_bgm ( EventBGM::getInstance() )

//===============================================
//	����
//===============================================
extern SE *se;
extern BGM *bgm;


#endif