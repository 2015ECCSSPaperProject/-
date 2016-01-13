#include	"iextreme.h"
#include	"SoundManager.h"

//**************************************************************************************************************
//
//		�T�E���h�Ǘ��N���X(winmain��framework�Ŏg������)
//
//**************************************************************************************************************
void SoundManager::Initialize()
{
	se = new SE_Manager;
	se->Initialize();
	bgm = new BGM_Manager;
	bgm->Initialize();
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

void SoundManager::SetFX(DXA_FX flag)
{
	se->SetFX(flag);
	bgm->SetFX(flag);
}

//**************************************************************************************************************
//
//		SE�Ǘ��N���X
//
//**************************************************************************************************************
//*********************************************************************************************
//		�p�����[�^�̐ݒ�
//*********************************************************************************************
//	�T�E���h�f�[�^(text�œǂݍ��ނ̂��ǂ���������Ȃ�)
SE_Manager::DATA all_dataS[] =
{
	{ "���s1", "DATA/Sound/SE/page03.wav", 12, true },
	{ "���s2", "DATA/Sound/SE/page04.wav", 12, true },
	{ "�\��", "DATA/Sound/SE/paper-haru.wav", 6, true },
	{ "�j��", "DATA/Sound/SE/����j�鉹.wav", 6, true },
	{ "���S�C", "DATA/Sound/SE/paper_gun.wav", 6, true },
	{ "�藠��", "DATA/Sound/SE/syuriken.wav", 6, true },
	{ "������", "DATA/Sound/SE/fall.wav", 1, false },
	{ "�̂ڂ�", "DATA/Sound/SE/rise.wav", 1, false },
	{ "�j��\��", "DATA/Sound/SE/cursor3.wav", 1, false },
	{ "�W�����v", "DATA/Sound/SE/jump.wav", 6, true },
	{ "�Z���j��", "DATA/Sound/SE/rend/rend_short.wav", 1, false },
	{ "�J�����_�[�j��", "DATA/Sound/SE/rend/rend_calendar.wav", 1, false },
	{ "�V���j��", "DATA/Sound/SE/rend/rend_shinbun.wav", 1, false },
	{ "�V���j��2", "DATA/Sound/SE/rend/rend_shinbun2.wav", 1, false },
	{ "�T�C���j��", "DATA/Sound/SE/rend/�R��.wav", 1, false },
	{ "�T�C���j��2", "DATA/Sound/SE/rend/�T�C��������.wav", 1, false },
	{ "�}�K�W���j��", "DATA/Sound/SE/rend/rend_magazine.wav", 1, false },
	{ "�}�K�W���j��2", "DATA/Sound/SE/rend/rend_magazine2.wav", 1, false },
	{ "�����j��", "DATA/Sound/SE/rend/rend_seisho.wav", 1, false },
	{ "�����j��2", "DATA/Sound/SE/rend/rend_seisho2.wav", 1, false },
	{ "�����j��3", "DATA/Sound/SE/rend/rend_seisho3.wav", 1, false },
	{ "�g�C���j��", "DATA/Sound/SE/rend/rend_wc_paper.wav", 1, false },
	{ "�g�C���j��2", "DATA/Sound/SE/rend/rend_wc_paper2.wav", 1, false },
	{ "�����j��", "DATA/Sound/SE/rend/rend_money.wav", 1, false },
	{ "�����j��2", "DATA/Sound/SE/rend/rend_money2.wav", 1, false },
	{ "END", nullptr }
};


//=============================================================================================
//		��	��	��
void SE_Manager::Initialize()
{
	play_manager = new fstSoundSE;

	for (int i = 0;;i++)
	{
		if (strcmp(all_dataS[i].id, "END") == 0) break;	// �I�[

		ID[all_dataS[i].id] = i;
		play_manager->Set(i, all_dataS[i].play_simultaneously, all_dataS[i].file_name, all_dataS[i].b3D);
	}
}
//
//=============================================================================================



//=============================================================================================
//		��		��
SE_Manager::~SE_Manager()
{
	delete play_manager;
}
//
//=============================================================================================



//=============================================================================================
//		�X		�V
void SE_Manager::Update()
{
	play_manager->UpdateListener();
}
//
//=============================================================================================



//=============================================================================================
//		��		��
int SE_Manager::Play_in(int data_num, bool loop)
{
	if (data_num != -1)
	{
		return play_manager->Play(data_num, loop);
	}
	return -1;
}

int SE_Manager::Play_in(int data_num, float volume, bool loop)
{
	if (data_num != -1)
	{
		play_manager->SetVolume(data_num, volume);
		return play_manager->Play(data_num, loop);
	}
	return -1;
}

int SE_Manager::Play_in(int data_num, const Vector3 &pos, const Vector3 &front, const Vector3 &move, bool loop)
{
	if (data_num != -1)
	{
		return play_manager->Play(data_num, pos, front, move, loop);
	}
	return -1;
}

int SE_Manager::Play(LPSTR _ID, bool loop)
{
	return Play_in(ID[_ID], loop);
}

int SE_Manager::Play(LPSTR _ID, float volume, bool loop)
{
	if (volume < 0) volume = 0;
	else if (volume > 1.0f) volume = 1.0f;

	return Play_in(ID[_ID], volume, loop);
}

int SE_Manager::Play(LPSTR _ID, const Vector3 &pos, const Vector3 &front, const Vector3 &move, bool loop)
{
	return Play_in(ID[_ID], pos, front, move, loop);
}

void SE_Manager::Stop(LPSTR _ID, int no)
{
	play_manager->Stop(ID[_ID], no);
}

void SE_Manager::Stop_all()
{
	play_manager->AllStop();
}

bool SE_Manager::isPlay(char *_ID,int no)
{
	return play_manager->isPlay(ID[_ID], no);
}

void SE_Manager::Set_listener(const Vector3 &pos, const Vector3 &front, const Vector3 &up, const Vector3 &move)
{
	play_manager->SetListenerAll(pos, front, up, move);
}
//
//=============================================================================================



//**************************************************************************************************************
//
//		BGM�Ǘ��N���X
//
//**************************************************************************************************************
//*********************************************************************************************
//		�p�����[�^�̐ݒ�
//*********************************************************************************************
//	�T�E���h�f�[�^(text�œǂݍ��ނ̂��ǂ���������Ȃ�)
BGM_Manager::DATA all_dataB[] =
{
	{ "�t���C�n�C", "DATA/Sound/BGM/�t���C�E�n�C.wav", false },
	{ "�t���C�n�C2", "DATA/Sound/BGM/�t���C�E�n�C.wav", false },
	{ "�z�C�b�X��", "DATA/Sound/SE/whistle.wav", false },
	{ "END", nullptr }
};


//=============================================================================================
//		��	��	��
void BGM_Manager::Initialize()
{
	play_manager = new fstSoundBGM;

	for (int i = 0;;i++)
	{
		if (strcmp(all_dataB[i].id, "END") == 0) break;	// �I�[

		ID[all_dataB[i].id] = i;
		play_manager->Set(i, all_dataB[i].file_name, all_dataB[i].b3D);
	}
}
//
//=============================================================================================



//=============================================================================================
//		��		��
BGM_Manager::~BGM_Manager()
{
	delete play_manager;
}
//
//=============================================================================================

//=============================================================================================
//		�X		�V
void BGM_Manager::Update()
{
	play_manager->Update();
	play_manager->UpdateListener();
}
//
//=============================================================================================



//=============================================================================================
//		��		��
void BGM_Manager::Play_in(int data_num, bool loop)
{
	if (data_num != -1)
	{
		play_manager->Play(data_num, loop);
	}
}

void BGM_Manager::Play_in(int data_num, float volume, bool loop)
{
	if (data_num != -1)
	{
		play_manager->SetVolume(data_num, volume);
		play_manager->Play(data_num, loop);
	}
}

void BGM_Manager::Play_in(int data_num, const Vector3 &pos, const Vector3 &front, const Vector3 &move, bool loop)
{
	if (data_num != -1)
	{
		play_manager->Play(data_num, pos, front, move, loop);
	}
}

void BGM_Manager::Play(LPSTR _ID, bool loop)
{
	Play_in(ID[_ID], loop);
}

void BGM_Manager::Play(LPSTR _ID, float volume, bool loop)
{
	if (volume < 0) volume = 0;
	else if (volume > 1.0f) volume = 1.0f;

	Play_in(ID[_ID], volume, loop);
}

void BGM_Manager::Play(LPSTR _ID, const Vector3 &pos, const Vector3 &front, const Vector3 &move, bool loop)
{
	return Play_in(ID[_ID], pos, front, move, loop);
}

void BGM_Manager::Stop(LPSTR _ID)
{
	play_manager->Stop(ID[_ID]);
}

void BGM_Manager::Stop_all()
{
	play_manager->AllStop();
}

bool BGM_Manager::isPlay(LPSTR _ID)
{
	return play_manager->isPlay(ID[_ID]);
}

void BGM_Manager::Set_speed(LPSTR _ID, float speed)
{
	return play_manager->SetSpeed(ID[_ID], speed);
}

void BGM_Manager::Set_volume(LPSTR _ID, int vol)
{
	return play_manager->SetVolume(ID[_ID], vol);
}


void BGM_Manager::Fade_in(LPSTR _ID, float fade_speed, bool loop)
{
	play_manager->FadeIn(ID[_ID], fade_speed, loop);
}

void BGM_Manager::Fade_out(LPSTR _ID, float fade_speed)
{
	play_manager->FadeOut(ID[_ID], fade_speed);
}

void BGM_Manager::Cross_fade(LPSTR inID, LPSTR outID, float fade_speed, bool loop)
{
	play_manager->CrossFade(ID[inID], ID[outID], fade_speed, fstSoundBGM::CROSS_FADE_TYPE::END_OF_ETERNITY, loop);
}

void BGM_Manager::Cross_fade(LPSTR inID, LPSTR outID, float in_speed, float out_speed, bool loop)
{
	play_manager->CrossFade(ID[inID], ID[outID], in_speed, out_speed, fstSoundBGM::CROSS_FADE_TYPE::END_OF_ETERNITY, loop);
}

void BGM_Manager::Set_listener(const Vector3 &pos, const Vector3 &front, const Vector3 &up, const Vector3 &move)
{
	play_manager->SetListenerAll(pos, front, up, move);
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
	step = frame = mainVolume = 0;

	bgm->Set_volume(mainBGM, mainVolume);
	bgm->SetFX(DXA_FX::DXAFX_OFF);
	se->SetFX(DXA_FX::DXAFX_OFF);
}
void EventBGM::Update()
{
	(this->*Mode_funk[mode])();
}
void EventBGM::None()
{
	if (!isKouhan)
	{
		if (timer->Get_limit_time() <= (LIMIT_TIME / 4)){
			this->Set_mode(MODE::KOUHAN);
			isKouhan = true;
		}
	}

	if (bgm->Get_volume(mainBGM) > mainVolume)
	{
		bgm->Fade_stop(mainBGM);
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
	//switch (step)
	//{
	//case 0:
	//	bgm->Fade_out(mainBGM, 6);
	//	step++;
	//	break;

	//case 1:
	//	if (frame++ > 180)
	//	{
	//		frame = 0;
	//		step++;
	//	}
	//	break;

	//case 2:
	//	bgm->Set_speed(mainBGM, 1.1f);
	//	bgm->Fade_in(mainBGM, 20);
	//	step = 0;
	//	mode = MODE::NONE;
	//	break;
	//}

	char str[64];
	strcpy_s(str, 64, mainBGM);
	strcat(mainBGM, "2");

	bgm->Cross_fade(mainBGM, str, .01f);

	bgm->Set_speed(mainBGM, 1.1f);
	step = 0;
	mode = MODE::NONE;
}
void EventBGM::End()
{
	switch (step)
	{
	case 0:
		bgm->Play("�z�C�b�X��", false);
		step++;
		break;

	case 1:
		if (!bgm->isPlay("�z�C�b�X��"))
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

void EventBGM::Set_manhole(bool in)
{
	// �}���z�[�����G�t�F�N�g
	if (in)
	{
		mainVolume = -1000;
		bgm->Set_volume(mainBGM, mainVolume);
		bgm->SetFX(DXA_FX::DXAFX_DISTORTION);
		se->SetFX(DXA_FX::DXAFX_ECHO);
	}

	// �}���z�[���O(�ʏ�)
	else
	{
		mainVolume = DSBVOLUME_MAX;
		bgm->Set_volume(mainBGM, mainVolume);
		bgm->SetFX(DXA_FX::DXAFX_OFF);
		se->SetFX(DXA_FX::DXAFX_OFF);
	}
}


//=============================================================================================
//		��		��
SE_Manager *se;
BGM_Manager *bgm;
//
//=============================================================================================