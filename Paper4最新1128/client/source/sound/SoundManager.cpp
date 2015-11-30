
#include	"iextreme.h"
#include	"../IEX/IEX_AudioIIDX.h"
#include	"SoundManager.h"

SoundBase::~SoundBase(){ SAFE_DELETE(play_manager); }

//********************************************************************************************************
//
//									���̊Ǘ�
//
//********************************************************************************************************
void SoundManager::Initialize()
{
	se = new SE, se->Initialize();
	bgm = new BGM, bgm->Initialize();
}

void SoundManager::Release()
{
	delete se;
	delete bgm;
}

void SoundManager::Update()
{
	se->Update();
	bgm->Update();
}

void SoundManager::Set_listener(const Vector3 &pos, const Vector3 &front, const Vector3 &up, const Vector3 &move)
{
	se->Set_listener(pos, front, up, move);
	bgm->Set_listener(pos, front, up, move);
}

void SoundBase::Set_listener(const Vector3 &pos, const Vector3 &front, const Vector3 &up, const Vector3 &move)
{
	play_manager->Set_listener_all(pos, front, up, move);
}


//********************************************************************************************************
//
//									S	E
//
//********************************************************************************************************

//*********************************************************************************************
//		�p�����[�^�̐ݒ�
//*********************************************************************************************
//	SE�f�[�^(text�œǂݍ��ނ̂��ǂ���������Ȃ�)
SE::DATA	all_se_data[] = 
{
	{ "���s", "DATA/Sound/SE/run.wav", 6, 1.0f, true },
	{ "�\��", "DATA/Sound/SE/paper-haru.wav", 6, 1.0f, true },
	{ "�j��", "DATA/Sound/SE/����j�鉹.wav", 6, 1.0f, true },
	{ "�j��2", "DATA/Sound/SE/�j��1.wav", 6, 1.0f, true },
	{ "���S�C", "DATA/Sound/SE/gun.wav", 6, 1.0f, true },
	{ "������", "DATA/Sound/SE/fall.wav", 1, 1.0f, false },
	{ "�z�C�b�X��", "DATA/Sound/SE/whistle.wav", 1, 1.0f, false },
	{ "�j��\��", "DATA/Sound/SE/cursor3.wav", 1, 1.0f, false },
	{ "�W�����v", "DATA/Sound/SE/jump.wav", 6, 1.0f, true },
	{ "END", nullptr }
};


//=============================================================================================
//		��	��	��
void SE::Initialize()
{
	play_manager = new iex3DSoundIIDX;

	data_no = 0;
	int set_num = 0;
	max_count = 0;

	while (true)
	{
		if (strcmp(all_se_data[data_no].id, "END") == 0) break;	// �I�[
		
		// ��ʒu�ۑ�
		all_se_data[data_no].start_num = set_num;

		play_manager->Set_copy(all_se_data[data_no].file_name, all_se_data[data_no].start_num, all_se_data[data_no].same_play_max, all_se_data[data_no].b3D);
		set_num += all_se_data[data_no].same_play_max;

		data_no++;
	}

	max_count = set_num;
}
//
//=============================================================================================



//=============================================================================================
//		��		��
//void SE::Release()
//{
//	delete play_manager;
//}
//
//=============================================================================================



//=============================================================================================
//		�X		�V
void SE::Update()
{
	play_manager->Update();
}
//
//=============================================================================================



//=============================================================================================
//		��		��
int SE::Play_in(int no, float volume, bool loop)
{
	int played = -1;	// �f�[�^������Ȃ����Đ��̈斞�^���Ȃ�-1���Ԃ�

	if (no != NOT_FOUND)
	{
		// �Đ����Ă��Ȃ��̈��T��
		for (int found = all_se_data[no].start_num; found < all_se_data[no].start_num + all_se_data[no].same_play_max; found++)
		{
			if (!play_manager->isPlay(found)){	// ���������I
				const int vol = (int)(-3000 * (1.0f - volume));
				play_manager->SetVolume(found, vol);
				play_manager->Play(found, loop);
				played = found;
				break;	// ���������̂Ń��[�v�����܂�(�����̖Y���ƔߎS)
			}
		}
	}

	return played;
	//return data_num;
}
int SE::Play_in(int data_num, const Vector3 &pos, const Vector3 &front, bool loop)
{
	int play_num = -1;						// ���ۂɃZ�b�g�����ԍ�

	if (data_num != -1)
	{
		// �Đ����Ă��Ȃ��̈��T��
		for (play_num = all_se_data[data_num].start_num; play_num < all_se_data[data_num].start_num + all_se_data[data_num].same_play_max; play_num++)
		{
			if (!play_manager->isPlay(play_num)){	// ���������I
				play_manager->Play(play_num, pos, front, loop);
				break;	// ���������̂Ń��[�v�����܂�(�����̖Y���ƔߎS)
			}
		}
	}

	return play_num;
}

int SE::Play(char *_ID, bool loop)
{
	const int data_num = Find_data_no(_ID);	// ��؂邽�߂�ID

	return Play_in(data_num, all_se_data[data_num].volume, loop);
}

int SE::Play(char *_ID, const Vector3 &pos, const Vector3 &front, bool loop)
{
	const int data_num = Find_data_no(_ID);
	return Play_in(data_num, pos, front, loop);
}

void SE::Stop(int no)
{
	play_manager->Stop(no);
}

void SE::Stop_all()
{
	for (int i = 0; i < max_count; i++) play_manager->Stop(i);
}

bool SE::isPlay(char *_ID)
{
	const int no = Find_data_no(_ID);	// ��؂邽�߂�ID

	if (no != NOT_FOUND)
	{
		// �Đ����Ă��Ȃ��̈��T��
		for (int found = all_se_data[no].start_num; found < all_se_data[no].start_num + all_se_data[no].same_play_max; found++)
		{
			if (play_manager->isPlay(found)){	// ���������I
				return true;
			}
		}
	}
	return false;
}
void SE::Set_data(int no, const Vector3 &pos, const Vector3 &front, const Vector3 &move)
{
	play_manager->Set_pos(no, pos);
	play_manager->Set_front(no, front);
	play_manager->Set_move(no, move);
}
//
//=============================================================================================


//=============================================================================================
//		�h�c��v�f�[�^����
int	SE::Find_data_no(char *_ID)
{
	int	result = NOT_FOUND;
	// �f�[�^�������[�v
	for (int i = 0; i < data_no; i++)
	{
		// ����
		if (strcmp(all_se_data[i].id, _ID) != 0) continue;

		// ����
		result = i;
		// ���[�v�𔲂���
		break;
	}

	return result;
}
//
//=============================================================================================




//********************************************************************************************************
//
//									B	G	M
//
//********************************************************************************************************

//*********************************************************************************************
//		�p�����[�^�̐ݒ�
//*********************************************************************************************
//	BGM�f�[�^(text�œǂݍ��ނ̂��ǂ���������Ȃ�)
BGM::DATA	all_bgm_data[] =
{
	{ "�t���C�n�C", "DATA/Sound/BGM/�t���C�E�n�C.wav", .5f, BGM::FADE_NONE },
	{ "END", nullptr }
};


//=============================================================================================
//		��	��	��
void BGM::Initialize()
{
	play_manager = new iex3DSoundIIDX;

	data_no = 0;

	while (true)
	{
		if (strcmp(all_bgm_data[data_no].id, "END") == 0) break;	// �I�[

		all_bgm_data[data_no].volume = all_bgm_data[data_no].max_vol;
		play_manager->Set(data_no, all_bgm_data[data_no].file_name, false);
		const int vol = (int)(-3000 * (1.0f - all_bgm_data[data_no].volume));
		play_manager->SetVolume(data_no, vol);
		data_no++;
	}


	// �֐��|�C���^
	Fade_mode_funk[FADE_NONE] = &BGM::None;
	Fade_mode_funk[FADE_IN] = &BGM::In;
	Fade_mode_funk[FADE_OUT] = &BGM::Out;
}
//
//=============================================================================================



//=============================================================================================
//		��		��
//void BGM::Release()
//{
//	delete play_manager;
//}
//
//=============================================================================================



//=============================================================================================
//		�X		�V
void BGM::Update()
{
	for (int i = 0; i < BGM::data_no; i++)
	{
		(this->*Fade_mode_funk[all_bgm_data[i].fade_type])(i);	// �t�F�[�h�֐����s
	}
	play_manager->Update();
}
//
//=============================================================================================



//=============================================================================================
//		��		��
void BGM::None(int no)
{}

void BGM::In(int no)
{
	// �{�����[���グ�Ă���
	all_bgm_data[no].volume += all_bgm_data[no].fade_speed;
	const int vol = (int)(-3000 * (1.0f - all_bgm_data[no].volume));

	// �t�F�[�h���������I
	if (all_bgm_data[no].volume > all_bgm_data[no].max_vol)
	{
		all_bgm_data[no].volume = all_bgm_data[no].max_vol;
		all_bgm_data[no].fade_type = FADE_NONE;
	}

	play_manager->SetVolume(no, vol);
}

void BGM::Out(int no)
{
	// �{�����[�������Ă���
	all_bgm_data[no].volume -= all_bgm_data[no].fade_speed;
	const int vol = (int)(-3000 * (1.0f - all_bgm_data[no].volume));

	// �t�F�[�h���������I
	if (all_bgm_data[no].volume < 0.1f)
	{
		all_bgm_data[no].volume = 0.0f;
		all_bgm_data[no].fade_type = FADE_NONE;
		play_manager->Stop(no);
	}

	play_manager->SetVolume(no, vol);
}

int BGM::Play_in(int no, float volume, bool loop)
{
	if (no != NOT_FOUND)
	{
		const int vol = (int)(-3000 * (1.0f - volume));
		play_manager->SetVolume(no, vol);
		play_manager->SetSpeed(no, 1.0f);
		play_manager->Play(no, loop);
	}

	return no;
}

int BGM::Play(char *_ID, bool loop)
{
	const int no = Find_data_no(_ID);

	return Play_in(no, all_bgm_data[no].volume, loop);
}

int BGM::Play(char *_ID, float volume, bool loop)
{
	const int no = Find_data_no(_ID);

	if (volume < 0) volume = 0;
	else if (volume > 1.0f) volume = 1.0f;

	return Play_in(no, volume, loop);
}

void BGM::Stop(char *_ID)
{
	int no = Find_data_no(_ID);
	if (no == NOT_FOUND) return;
	play_manager->Stop(no);
}

int BGM::Fade_in(char *_ID, int speed, bool loop)
{
	/*	�Ăяo����1�񂾂��ł��肢���܂�	*/
	int no = Find_data_no(_ID);
	if (no == NOT_FOUND) return NOT_FOUND;
	all_bgm_data[no].volume = .0f;
	all_bgm_data[no].fade_type = F_TYPE::FADE_IN;
	if (speed <= 0) speed = 1;
	all_bgm_data[no].fade_speed = (speed * .001f);
	play_manager->SetVolume(no, -10000);
	play_manager->Play(no, loop);


	return no;
}

void BGM::Fade_out(char *_ID, int speed)
{
	/*	�Ăяo����1�񂾂��ł��肢���܂�	*/

	int no = Find_data_no(_ID);
	if (no == NOT_FOUND) return;
	all_bgm_data[no].volume = all_bgm_data[no].max_vol;
	all_bgm_data[no].fade_type = F_TYPE::FADE_OUT;
	if (speed <= 0) speed = 1;
	all_bgm_data[no].fade_speed = (speed * .001f);
}

void BGM::Stop_all()
{
	for (int i = 0; i < data_no; i++) play_manager->Stop(i);
}

void BGM::Pause(char *_ID)
{
	int no = Find_data_no(_ID);
	if (no == NOT_FOUND) return;
	play_manager->Pause(no);
}

bool BGM::isPlay(char *_ID)
{
	int no = Find_data_no(_ID);
	if (no == NOT_FOUND) return false;
	return (play_manager->isPlay(no) != 0);
}

void BGM::Set_speed(char *_ID, float speed)
{
	int no = Find_data_no(_ID);
	if (no == NOT_FOUND) return;
	play_manager->SetSpeed(no, speed);
}


//
//=============================================================================================

//=============================================================================================
//		�h�c��v�f�[�^����
int	BGM::Find_data_no(char *_ID)
{
	int	result = NOT_FOUND;
	// �f�[�^�������[�v
	for (int i = 0; i < data_no; i++)
	{
		// ����
		if (strcmp(all_bgm_data[i].id, _ID) != 0) continue;

		// ����
		result = i;
		// ���[�v�𔲂���
		break;
	}

	return result;
}
//
//=============================================================================================




//********************************************************************************************************
//
//									B G M �� �� �� �� 
//
//********************************************************************************************************
#include	"../timer/Timer.h"

EventBGM::EventBGM()
{

}
void EventBGM::Initialize(char *mainBGMname)
{
	sprintf_s(mainBGM, "%s", mainBGMname);

	// �֐��|�C���^������
	Mode_funk[MODE::NONE] = &EventBGM::None;
	Mode_funk[MODE::START] = &EventBGM::Start;
	Mode_funk[MODE::KOUHAN] = &EventBGM::Kouhan;
	Mode_funk[MODE::END] = &EventBGM::End;

	isKouhan = false;
	step = frame = 0;
}
void EventBGM::Update()
{
	(this->*Mode_funk[mode])();
}
void EventBGM::None()
{
	if (!isKouhan)
	{
		if (timer->Get_limit_time() % 60 <= (LIMIT_TIME / 4)){
			this->Set_mode(MODE::KOUHAN);
			isKouhan = true;
		}
	}
}
void EventBGM::Start()
{
	bgm->Set_speed(mainBGM, 1.0f);
	bgm->Fade_in(mainBGM, 20);
	mode = MODE::NONE;
}
void EventBGM::Kouhan()
{
	switch (step)
	{
	case 0:
		bgm->Fade_out(mainBGM, 6);
		step++;
		break;

	case 1:
		if (frame++ > 180)
		{
			frame = 0;
			step++;
		}
		break;

	case 2:
		bgm->Set_speed(mainBGM, 1.1f);
		bgm->Fade_in(mainBGM, 20);
		step = 0;
		mode = MODE::NONE;
		break;
	}
}
void EventBGM::End()
{
	switch (step)
	{
	case 0:
		se->Play("�z�C�b�X��");
		step++;
		break;

	case 1:
		if (!se->isPlay("�z�C�b�X��"))
		{
			FadeControl::Setting(FadeControl::FADE_OUT_W, 2);
			step++;
			bgm->Fade_out(mainBGM, 5);
		}
		break;
	case 2:
		if (!bgm->isPlay(mainBGM)){
			step = 0;
			mode = MODE::NONE;
		}
		break;
	}
}


//=============================================================================================
//		��		��
SE *se;
BGM *bgm;
//
//=============================================================================================