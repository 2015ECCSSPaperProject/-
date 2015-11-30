//*****************************************************************************************************************************
#include	"iextreme.h"

#include	"IEX_AudioIIDX.h"

HRESULT result;

//**************************************************************************************************************
//
//		�T�E���h�o�b�t�@
//
//**************************************************************************************************************

//**************************************************************************************************************
//
//**************************************************************************************************************
SoundBufferIIDX::SoundBufferIIDX(LPDIRECTSOUND lpDS, char* filename, bool b3D)
{
	DSBUFFERDESC	dsbd;
	LPVOID			lpbuf1, lpbuf2;
	DWORD			dwbuf1, dwbuf2;

	lpBuf3D = NULL;
	lpBuf = NULL;

	/*	WAV�t�@�C���̃��[�h	*/
	lpWBuf = LoadWAV(filename, &size, &wfx);
	/*	���[�h���s	*/
	if (lpWBuf == NULL){
		return;
	}

	/* �t�@�C�����ۑ� */
	sprintf_s(wav_file_path, sizeof(wav_file_path), "%s", filename);

	/* �񎟃o�b�t�@�쐬	*/
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	if (b3D) dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRL3D;
	else dsbd.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
	dsbd.dwBufferBytes = size;
	dsbd.lpwfxFormat = &wfx;

	result = lpDS->CreateSoundBuffer(&dsbd, &lpBuf, NULL);

	if (b3D && result == E_INVALIDARG && dsbd.lpwfxFormat->nChannels == 2)
	{
		MyAssert(false, "3D�T�E���h������X�e���I�����͎g���Ȃ���I\n���m�����ɕϊ����Ă�");
	}

	if (result != DS_OK)
	{
		return;
	}

	lpBuf3D = NULL;
	if (b3D == true)
	{
		lpBuf->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID*)&lpBuf3D);
		this->Set_all3D(DS3D_DEFAULTMAXDISTANCE, DS3D_DEFAULTMINDISTANCE, Vector3(0, 0, 0), Vector3(0, 0, -1), DS3D_MAXCONEANGLE, DS3D_DEFAULTCONEOUTSIDEVOLUME, Vector3(0, 0, 0));
	}

	/* �񎟃o�b�t�@�̃��b�N	*/
	lpBuf->Lock(0, size, &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0);
	/* �����f�[�^�̐ݒ�	*/
	memcpy(lpbuf1, lpWBuf, dwbuf1);
	if (dwbuf2 != 0) memcpy(lpbuf2, lpWBuf + dwbuf1, dwbuf2);
	/* �����f�[�^�̉��	*/
	GlobalFree(lpWBuf);
	/* �񎟃o�b�t�@�̃��b�N����	*/
	lpBuf->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2);

	/* ���쏉���� */
	PlayCursor = 0;
	BufferSize = size;
	format = wfx;
}

void SoundBufferIIDX::Create_and_copy(LPDIRECTSOUND lpDS, char* filename, bool b3D, SoundBufferIIDX **buffers, int dst, int count)
{
	DSBUFFERDESC	dsbd;
	LPVOID			lpbuf1, lpbuf2;
	DWORD			dwbuf1, dwbuf2;


	//===========================================================
	//		�R�s�[���쐬
	//===========================================================
	buffers[dst]->lpBuf3D = NULL;
	buffers[dst]->lpBuf = NULL;

	/*	WAV�t�@�C���̃��[�h	*/
	buffers[dst]->lpWBuf = buffers[dst]->LoadWAV(filename, &buffers[dst]->size, &buffers[dst]->wfx);

	/* �t�@�C�����ۑ� */
	sprintf_s(buffers[dst]->wav_file_path, sizeof(buffers[dst]->wav_file_path), "%s", filename);

	/* �񎟃o�b�t�@�쐬	*/
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	if (b3D) dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRL3D;
	else dsbd.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
	dsbd.dwBufferBytes = buffers[dst]->size;
	dsbd.lpwfxFormat = &buffers[dst]->wfx;

	result = lpDS->CreateSoundBuffer(&dsbd, &buffers[dst]->lpBuf, NULL);

	if (b3D && result == E_INVALIDARG && dsbd.lpwfxFormat->nChannels == 2)
	{
		MyAssert(false, "3D�T�E���h������X�e���I�����͎g���Ȃ���I\n���m�����ɕϊ����Ă�");
	}

	if (result != DS_OK)
	{
		return;
	}

	buffers[dst]->lpBuf3D = NULL;
	if (b3D)
	{
		buffers[dst]->lpBuf->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID*)&buffers[dst]->lpBuf3D);
		buffers[dst]->Set_all3D(DS3D_DEFAULTMAXDISTANCE, DS3D_DEFAULTMINDISTANCE, Vector3(0, 0, 0), Vector3(0, 0, -1), DS3D_MAXCONEANGLE, DS3D_DEFAULTCONEOUTSIDEVOLUME, Vector3(0, 0, 0));
	}

	/* �񎟃o�b�t�@�̃��b�N	*/
	buffers[dst]->lpBuf->Lock(0, buffers[dst]->size, &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0);
	/* �����f�[�^�̐ݒ�	*/
	memcpy(lpbuf1, buffers[dst]->lpWBuf, dwbuf1);
	if (dwbuf2 != 0) memcpy(lpbuf2, buffers[dst]->lpWBuf + dwbuf1, dwbuf2);
	/* �����f�[�^�̉��	*/
	//GlobalFree(lpWBuf);	// ���̕����R�����g�A�E�g����̂ɑS�Ă��l�܂��Ă�
	/* �񎟃o�b�t�@�̃��b�N����	*/
	buffers[dst]->lpBuf->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2);

	/* ���쏉���� */
	buffers[dst]->PlayCursor = 0;
	buffers[dst]->BufferSize = buffers[dst]->size;
	buffers[dst]->format = buffers[dst]->wfx;


	//===========================================================
	//		�R�s�[��쐬
	//===========================================================
	for (int no = dst + 1; no < dst + count; no++)
	{

		DSBUFFERDESC	dsbd;
		LPVOID			lpbuf1 = NULL, lpbuf2 = NULL;
		DWORD			dwbuf1 = 0, dwbuf2 = 0;

		buffers[no]->lpBuf3D = NULL;
		buffers[no]->lpBuf = NULL;

		/* �񎟃o�b�t�@�쐬	*/
		ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
		dsbd.dwSize = sizeof(DSBUFFERDESC);
		if (b3D == true) dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRL3D;
		else dsbd.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME;
		dsbd.dwBufferBytes = buffers[dst]->size;
		dsbd.lpwfxFormat = &buffers[dst]->wfx;

		result = lpDS->CreateSoundBuffer(&dsbd, &buffers[no]->lpBuf, NULL);
		if (result == E_OUTOFMEMORY)
		{
			MessageBox(0, "�Đ����Ԃ�����wav�t�@�C�����ǂݍ��܂ꂷ���ă�����������Ȃ��Ȃ��Ă��B�Z�b�g�̐������炵�Ă�", null, MB_OK);
			assert(0);
			return;
		}

		buffers[no]->lpBuf3D = NULL;
		if (b3D)
		{
			result = buffers[no]->lpBuf->QueryInterface(IID_IDirectSound3DBuffer, (LPVOID*)&buffers[no]->lpBuf3D);
			buffers[no]->Set_all3D(DS3D_DEFAULTMAXDISTANCE, DS3D_DEFAULTMINDISTANCE, Vector3(0, 0, 0), Vector3(0, 0, -1), DS3D_MAXCONEANGLE, DS3D_DEFAULTCONEOUTSIDEVOLUME, Vector3(0, 0, 0));
		}

		/* �񎟃o�b�t�@�̃��b�N	*/
		buffers[no]->lpBuf->Lock(0, buffers[dst]->size, &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0);
		/* �����f�[�^�̐ݒ�	*/
		memcpy(lpbuf1, buffers[dst]->lpWBuf, dwbuf1);
		if (dwbuf2 != 0) memcpy(lpbuf2, buffers[dst]->lpWBuf + dwbuf1, dwbuf2);
		/* �����f�[�^�̉��	*/
		//GlobalFree(buffers[no].lpWBuf);
		/* �񎟃o�b�t�@�̃��b�N����	*/
		buffers[no]->lpBuf->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2);

		/* ���쏉���� */
		buffers[no]->PlayCursor = 0;
		buffers[no]->BufferSize = buffers[dst]->size;
		buffers[no]->format = buffers[dst]->wfx;
	}

	/* �R�s�[�������f�[�^�̉��	*/
	GlobalFree(buffers[dst]->lpWBuf);
}

SoundBufferIIDX::~SoundBufferIIDX()
{
	if (lpBuf != NULL)lpBuf->Release();
	if (lpBuf3D != NULL) lpBuf3D->Release();

}

//**************************************************************************************************************
//		�v�`�u�t�@�C���̓ǂݍ���
//**************************************************************************************************************
LPBYTE SoundBufferIIDX::LoadWAV(LPSTR fname, LPDWORD size, LPWAVEFORMATEX wfx)
{
	HMMIO			hMMIO = NULL;		/*	�t�@�C���n���h��	*/
	PCMWAVEFORMAT	pwf;				/*	WAV�f�[�^�`��		*/
	MMCKINFO		ckparent, ckinfo;	/*	RIFF�`�����N���	*/
	MMIOINFO		mminfo;				/*	�t�@�C�����		*/
	DWORD			i;
	LPBYTE			buf = NULL;			/*	�ǂݍ��݃o�b�t�@	*/

	/* �I�[�v��	*/
	if ((hMMIO = mmioOpen(fname, NULL, MMIO_ALLOCBUF | MMIO_READ)) == NULL)
	{
		MessageBox(0, "wav�t�@�C���������Ă��Ȃ����Awav�t�@�C�������Ԉ���Ă����", null, MB_OK);
		assert(0);
		return NULL;
	}
	if (mmioDescend(hMMIO, &ckparent, NULL, 0) != 0) goto WAVE_LoadError;
	/*	�v�`�u(RIFF)�t�@�C���`�F�b�N		*/
	if ((ckparent.ckid != FOURCC_RIFF) || (ckparent.fccType != mmioFOURCC('W', 'A', 'V', 'E'))) goto WAVE_LoadError;
	/*	�������`�����N�ɐN��		*/
	ckinfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(hMMIO, &ckinfo, &ckparent, MMIO_FINDCHUNK) != 0) goto WAVE_LoadError;
	if (ckinfo.cksize < sizeof(PCMWAVEFORMAT)) goto WAVE_LoadError;
	/*	�`�����N���烊�[�h	*/
	if (mmioRead(hMMIO, (HPSTR)&pwf, sizeof(pwf)) != sizeof(pwf)) goto WAVE_LoadError;
	if (pwf.wf.wFormatTag != WAVE_FORMAT_PCM) goto WAVE_LoadError;
	/*	WAV�t�H�[�}�b�g�̕ۑ�	*/
	memset(wfx, 0, sizeof(WAVEFORMATEX));
	memcpy(wfx, &pwf, sizeof(pwf));
	/*	�f�[�^�̓ǂݍ���	*/
	if (mmioSeek(hMMIO, ckparent.dwDataOffset + sizeof(FOURCC), SEEK_SET) == -1) goto WAVE_LoadError;
	/*	���������`�����N�ɐN��		*/
	ckinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(hMMIO, &ckinfo, &ckparent, MMIO_FINDCHUNK) != 0) goto WAVE_LoadError;
	if (mmioGetInfo(hMMIO, &mminfo, 0) != 0) goto WAVE_LoadError;
	/*	�o�b�t�@�T�C�Y�ۑ�	*/
	if (size != NULL) *size = ckinfo.cksize;
	/*	�v�`�u�p�o�b�t�@�̎擾	*/
	buf = (LPBYTE)GlobalAlloc(LPTR, ckinfo.cksize);
	if (buf == NULL) goto WAVE_LoadError;
	/*	�f�[�^�̓ǂ݂Ƃ�	*/
	for (i = 0; i < ckinfo.cksize; i++){
		/*	�G���[�`�F�b�N	*/
		if (mminfo.pchNext >= mminfo.pchEndRead){
			if (mmioAdvance(hMMIO, &mminfo, MMIO_READ) != 0) goto WAVE_LoadError;
			if (mminfo.pchNext >= mminfo.pchEndRead) goto WAVE_LoadError;
		}
		*(buf + i) = *((LPBYTE)mminfo.pchNext);
		mminfo.pchNext++;
	}
	/*	�t�@�C���A�N�Z�X�I��	*/
	mmioSetInfo(hMMIO, &mminfo, 0);
	mmioClose(hMMIO, 0);
	return buf;

WAVE_LoadError:	/*	�G���[�I��	*/
	mmioClose(hMMIO, 0);
	if (buf != NULL) GlobalFree(buf);
	return NULL;
}


//**************************************************************************************************************
//	���ݒ�
//**************************************************************************************************************
//-------------------------------------------------------------
//	�ʒu�ݒ�	
//-------------------------------------------------------------

//**************************************************************************************************************
//	�Đ��Ǘ�
//**************************************************************************************************************
//-------------------------------------------------------------
//	�Đ�	
//-------------------------------------------------------------
void SoundBufferIIDX::Play(BOOL loop)
{
	lpBuf->Stop();
	lpBuf->SetCurrentPosition(PlayCursor);
	//	���[�v�Đ�
	if (loop) lpBuf->Play(0, 0, DSBPLAY_LOOPING);
	//	�m�[�}���Đ�
	else	   lpBuf->Play(0, 0, 0);

	//	lpBuf->SetFrequency(8000);


	PlayCursor = 0;
}

//-------------------------------------------------------------
//	��~	
//-------------------------------------------------------------
void SoundBufferIIDX::Stop()
{
	lpBuf->Stop();
}

void SoundBufferIIDX::Pause()
{
	lpBuf->GetCurrentPosition(&PlayCursor, NULL);
	lpBuf->Stop();
}

//-------------------------------------------------------------
//	�{�����[���ύX
//-------------------------------------------------------------
void SoundBufferIIDX::SetVolume(int volume)
{
	lpBuf->SetVolume(volume);
}

//-------------------------------------------------------------
//	�{�����[���Q�b�^�[
//-------------------------------------------------------------
int	SoundBufferIIDX::GetVolume()
{
	LONG ret;
	lpBuf->GetVolume(&ret);
	return ret;
}

//-------------------------------------------------------------
//	�X�e���I(���E��)�֌W
//-------------------------------------------------------------
void SoundBufferIIDX::SetPan(int pan)
{
	lpBuf->SetPan(pan);
}

int SoundBufferIIDX::GetPan()
{
	LONG ret;
	lpBuf->GetPan(&ret);
	return ret;
}


//-------------------------------------------------------------
//	���g���֌W(�Đ����x�E�s�b�`)
//-------------------------------------------------------------
void SoundBufferIIDX::SetFrequency(int pitch)
{
	lpBuf->SetFrequency(pitch);
}

int SoundBufferIIDX::GetFrequency()
{
	DWORD ret;
	lpBuf->GetFrequency(&ret);
	return ret;
}

//-------------------------------------------------------------
//	�Đ����x(��̎�����Ă邾��)
//-------------------------------------------------------------
void SoundBufferIIDX::SetSpeed(float speed)
{
	DWORD frequency = (DWORD)(format.nSamplesPerSec*speed);
	lpBuf->SetFrequency(frequency);
}

float SoundBufferIIDX::GetSpeed()
{
	DWORD work;
	lpBuf->GetFrequency(&work);

	return ((float)work / format.nSamplesPerSec);
}


//-------------------------------------------------------------
//	�Đ��`�F�b�N	
//-------------------------------------------------------------
BOOL SoundBufferIIDX::isPlay()
{
	DWORD	dwAns;
	lpBuf->GetStatus(&dwAns);
	if ((dwAns&DSBSTATUS_PLAYING) != 0) return TRUE;
	else return FALSE;
}


//-------------------------------------------------------------
//	�Đ��ʒu�֌W
//-------------------------------------------------------------
DWORD SoundBufferIIDX::GetPlayCursor()
{
	DWORD ret;
	lpBuf->GetCurrentPosition(&ret, NULL);

	return ret;
}

DWORD SoundBufferIIDX::GetPlayFrame()
{
	return (GetPlayCursor() / (format.nAvgBytesPerSec / 60));
}

int SoundBufferIIDX::GetPlaySecond()
{
	return (GetPlayCursor() / format.nAvgBytesPerSec);
}

void SoundBufferIIDX::SetPlaySecond(int sec)
{
	DWORD set = sec * format.nAvgBytesPerSec;
	lpBuf->SetCurrentPosition(set);
}

DWORD SoundBufferIIDX::GetSize()
{
	return BufferSize;
}

int SoundBufferIIDX::GetLengthSecond()
{
	return (BufferSize / format.nAvgBytesPerSec);
}

//-------------------------------------------------------------
//	3D�T�E���h�֌W
//-------------------------------------------------------------
void SoundBufferIIDX::Set_dist(float max_dist, float min_dist)
{
	lpBuf3D->SetMaxDistance(max_dist, DS3D_DEFERRED), lpBuf3D->SetMinDistance(min_dist, DS3D_DEFERRED);
}
void SoundBufferIIDX::Set_pos(const Vector3 &pos)
{
	lpBuf3D->SetPosition(pos.x, pos.y, pos.z, DS3D_DEFERRED);
}
void SoundBufferIIDX::Set_front(const Vector3 &front)
{
	lpBuf3D->SetConeOrientation(front.x, front.y, front.z, DS3D_DEFERRED);
}
void SoundBufferIIDX::Set_range(int degreeIn)
{
	lpBuf3D->SetConeAngles(degreeIn, (DS3D_MAXCONEANGLE - degreeIn), DS3D_DEFERRED);
}
void SoundBufferIIDX::Set_outRange_volume(int out_vol)
{
	lpBuf3D->SetConeOutsideVolume(out_vol, DS3D_DEFERRED);
}
void SoundBufferIIDX::Set_move(const Vector3 &move)
{
	lpBuf3D->SetVelocity(move.x, move.y, move.z, DS3D_DEFERRED);
}
void SoundBufferIIDX::Set_all3D(float max_dist, float min_dist, const Vector3 &pos, const Vector3 &front, int degreeIn, int out_vol, const Vector3 &move)
{
	DS3DBUFFER set;
	ZeroMemory(&set, sizeof(DS3DBUFFER));
	set.dwSize = sizeof(DS3DBUFFER);

	/*
	DS3DMODE_HEADRELATIVE
	�T�E���h �p�����[�^(�ʒu�E���x�E����) �́A���X�i�[�̃p�����[�^�ɑ΂��đ��ΓI�Ȃ��̂ł���B
	���̃��[�h�ł́A���X�i�[�̃p�����[�^���ω�����ƃT�E���h�̐�΃p�����[�^�͎����I�ɍX�V�����̂ŁA
	���΃p�����[�^�͈��ɕۂ����B

	DS3DMODE_NORMAL 
	�W���̏����B����̓f�t�H���g�̃��[�h�ł���B
	*/
	set.dwMode = DS3DMODE_HEADRELATIVE;

	set.flMaxDistance = max_dist;
	set.flMinDistance = min_dist;
	set.vPosition = D3DXVECTOR3(pos.x, pos.y, pos.z);
	set.vConeOrientation = D3DXVECTOR3(front.x, front.y, front.z);
	set.dwInsideConeAngle = degreeIn;
	set.dwOutsideConeAngle = (DS3D_MAXCONEANGLE - degreeIn);
	set.lConeOutsideVolume = out_vol;
	set.vVelocity = D3DXVECTOR3(move.x, move.y, move.z);

	lpBuf3D->SetAllParameters(&set, DS3D_DEFERRED);
}

//**************************************************************************************************************
//
//		�X�g���[���T�E���h
//
//**************************************************************************************************************

static int	NumStream = 0;
static bool ForceEnd = false;

//**************************************************************************************************************
//		�Ǘ��X���b�h
//**************************************************************************************************************
DWORD WINAPI ThreadIIDX(LPDWORD lpdwParam)
{
	DWORD	param;
	iexStreamSoundIIDX*	lpStream;

	lpStream = (iexStreamSoundIIDX*)(lpdwParam);
	for (;;){
		if (lpStream == NULL) break;
		param = WaitForMultipleObjects(3, lpStream->hEvent, FALSE, 100);

		if (ForceEnd == true) param = -1;

		switch (param){
		case WAIT_OBJECT_0:		if (lpStream->type == TYPE_WAV) lpStream->SetBlockWAV(1);
			if (lpStream->type == TYPE_OGG) lpStream->SetBlockOGG(1);
			break;
		case WAIT_OBJECT_0 + 1:	if (lpStream->type == TYPE_WAV) lpStream->SetBlockWAV(0);
			if (lpStream->type == TYPE_OGG) lpStream->SetBlockOGG(0);
			break;

		case WAIT_TIMEOUT:		switch (lpStream->GetMode()){
		case STR_NORMAL:		break;
		case STR_FADEIN:	lpStream->SetVolume(lpStream->GetVolume() + lpStream->GetParam());
			if (lpStream->GetVolume() >= 255) lpStream->SetMode(STR_NORMAL, 0);
			break;
		case STR_FADEOUT:	lpStream->SetVolume(lpStream->GetVolume() - lpStream->GetParam());
			if (lpStream->GetVolume() <= 0){
				lpStream->Stop();
				delete lpStream;
				ExitThread(TRUE);
				return 0;
			}
			break;
		}
								break;

		default:
			lpStream->Stop();
			delete lpStream;
			ExitThread(TRUE);
			return 0;

		}
	}
	return 0;
}

//**************************************************************************************************************
//
//**************************************************************************************************************
iexStreamSoundIIDX::iexStreamSoundIIDX(LPDIRECTSOUND lpDS, LPSTR filename, BYTE mode, int param)
{
	NumStream++;

	LPSTR	work;
	BOOL	bInit;
	//	�X�g���[���t�@�C�����J��
	work = &filename[lstrlen(filename) - 4];
	if (lstrcmpi(work, ".wav") == 0) bInit = SetWAV(lpDS, filename);
	if (lstrcmpi(work, ".ogg") == 0) bInit = SetOGG(lpDS, filename);

	lpStream->SetCurrentPosition(0);
	if (mode != STR_FADEIN) SetVolume(255);
	else SetVolume(0);
	/*	�Ǘ��X���b�h�̍쐬	*/
	hStrThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadIIDX, this, 0, &dwThreadId);
	if (hStrThread == NULL) return;
	/*	�Đ��J�n	*/
	lpStream->Play(0, 0, DSBPLAY_LOOPING);

	this->mode = mode;
	this->param = param;
}



iexStreamSoundIIDX::~iexStreamSoundIIDX()
{
	if (lpStream != NULL){
		if (type == TYPE_OGG) ov_clear(&vf);
		else if (hStrFile != NULL) fclose(hStrFile);

		if (lpStrNotify != NULL) lpStrNotify->Release();
		lpStrNotify = NULL;
		/*	�o�b�t�@�J��	*/
		if (lpStream != NULL) lpStream->Release();
		lpStream = NULL;
	}

	NumStream--;
}

//**************************************************************************************************************
//
//**************************************************************************************************************
//-------------------------------------------------------------
//	�u���b�N�쐬
//-------------------------------------------------------------
BOOL	iexStreamSoundIIDX::OGGRead(LPBYTE dst, unsigned long size)
{
	DWORD	remain = size;
	char*	dstPtr = (char*)dst;
	while (remain > 0){
		long actualRead = ov_read(&vf, dstPtr, remain, 0, 2, 1, NULL);
		//�I�[�`�F�b�N
		if (actualRead <= 0){
			if (ov_pcm_seek(&vf, 0)) return FALSE;
		}
		remain -= actualRead;
		dstPtr += actualRead;
	}
	return TRUE;
}

BOOL	iexStreamSoundIIDX::SetBlockOGG(int block)
{
	LPBYTE	blk1, blk2;
	DWORD	bs1, bs2;

	DWORD	CurPos;

	CurPos = StrSize - StrPos;
	//	�o�b�t�@���b�N
	lpStream->Lock((rate * 4 * STRSECOND)*block, rate * 4 * STRSECOND, (LPVOID*)&blk1, &bs1, (LPVOID*)&blk2, &bs2, 0);
	//	�u���b�N�P�ւ̃f�[�^�Z�b�g
	OGGRead(blk1, bs1);
	StrPos += bs1;
	//	�u���b�N�Q�ւ̃f�[�^�Z�b�g
	if (blk2){
		OGGRead(blk2, bs2);
		StrPos += bs2;
	}

	lpStream->Unlock(blk1, bs1, blk2, bs2);
	return TRUE;
}


BOOL	iexStreamSoundIIDX::SetBlockWAV(int block)
{
	LPBYTE	blk1, blk2;
	DWORD	bs1, bs2, work;

	DWORD	CurPos;

	CurPos = StrSize - StrPos;
	lpStream->Lock((rate * 4 * STRSECOND)*block, rate * 4 * STRSECOND, (LPVOID*)&blk1, &bs1, (LPVOID*)&blk2, &bs2, 0);

	work = fread(blk1, bs1, 1, hStrFile);
	StrPos += work;
	if (work < bs1){
		/*	�����߂�	*/
		fseek(hStrFile, LoopPtr, SEEK_SET);
		work = fread(blk1 + work, bs1 - work, 1, hStrFile);
		StrPos = work;
	}

	if (blk2){
		work = fread(blk2, bs2, 1, hStrFile);
		StrPos += work;
		if (work < bs2){
			/*	�����߂�	*/
			fseek(hStrFile, LoopPtr, SEEK_SET);
			work = fread(blk2 + work, bs2 - work, 1, hStrFile);
			StrPos = work;
		}
	}

	lpStream->Unlock(blk1, bs1, blk2, bs2);
	return TRUE;
}


//**************************************************************************************************************
//	�f�[�^�Ǘ�
//**************************************************************************************************************
void iexStreamSoundIIDX::Initialize(LPDIRECTSOUND lpDS, int rate)
{
	DSBUFFERDESC	dsbd;
	WAVEFORMATEX	wfx;


	/*	�������`�F�b�N	*/
	if (lpDS == NULL) return;
	/*	�v�`�u�d�t�H�[�}�b�g������	*/
	ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 2;			/*	stereo	*/
	wfx.nSamplesPerSec = rate;
	wfx.wBitsPerSample = 16;			/*	16bit	*/
	wfx.nBlockAlign = 4;
	wfx.nAvgBytesPerSec = rate * 4;
	/* �񎟃o�b�t�@�̏�����	*/
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_LOCSOFTWARE;
	dsbd.dwBufferBytes = rate * 4 * STRSECOND * 2;
	dsbd.lpwfxFormat = &wfx;
	if (lpDS->CreateSoundBuffer(&dsbd, &lpStream, NULL) != DS_OK) return;
	lpStream->SetFormat(&wfx);

	if (lpStream->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&lpStrNotify) != DS_OK) return;
	/*	�ʒu�C�x���g�쐬	*/
	hEvent[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	hEvent[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
	hEvent[2] = CreateEvent(NULL, FALSE, FALSE, NULL);

	pn[0].dwOffset = 0;
	pn[0].hEventNotify = hEvent[0];
	pn[1].dwOffset = rate * 4 * STRSECOND;
	pn[1].hEventNotify = hEvent[1];

	pn[2].dwOffset = DSBPN_OFFSETSTOP;
	pn[2].hEventNotify = hEvent[2];

	if (lpStrNotify->SetNotificationPositions(3, pn) != DS_OK) return;

	this->rate = rate;
	return;
}


BOOL	iexStreamSoundIIDX::SetWAV(LPDIRECTSOUND lpDS, char* filename)
{
	HMMIO			hMMIO = NULL;		/*	�t�@�C���n���h��	*/
	MMCKINFO		ckinfo, ckparent;	/*	RIFF�`�����N���	*/
	LRESULT			ptr;

	/* �I�[�v��	*/
	hMMIO = mmioOpen(filename, NULL, MMIO_ALLOCBUF | MMIO_READ);
	mmioDescend(hMMIO, &ckparent, NULL, 0);
	/*	���������`�����N�ɐN��		*/
	ckinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmioDescend(hMMIO, &ckinfo, &ckparent, MMIO_FINDCHUNK);
	//	���݈ʒu�擾
	ptr = mmioSeek(hMMIO, 0, SEEK_CUR);
	/*	�t�@�C���A�N�Z�X�I��	*/
	mmioClose(hMMIO, 0);
	if (ptr == -1) return FALSE;

	/*	�t�@�C���I�[�v��	*/
	hStrFile = fopen(filename, "rb");
	if (hStrFile == NULL) return FALSE;
	//	�X�g���[�����ݒ�
	StrPos = 0;
	LoopPtr = ptr;

	fseek(hStrFile, 0L, SEEK_END);
	StrSize = ftell(hStrFile) - LoopPtr;

	StrSize = GetFileSize(hStrFile, NULL) - LoopPtr;
	//	�t�@�C���V�[�N
	fseek(hStrFile, LoopPtr, SEEK_SET);

	type = TYPE_WAV;
	Initialize(lpDS, 22050);

	/*	�Đ�����	*/
	SetBlockWAV(0);

	return TRUE;
}

//
//	OGG�p�X�g���[��������
//

BOOL	iexStreamSoundIIDX::SetOGG(LPDIRECTSOUND lpDS, char* filename)
{
	//	�t�@�C���I�[�v�� 
	hStrFile = fopen(filename, "rb");
	if (hStrFile == NULL) return FALSE;

	//Ogg���J��
	ov_open(hStrFile, &vf, NULL, 0);

	//�V�[�N�\?
	if (!ov_seekable(&vf)){
		ov_clear(&vf);
		return FALSE;
	}

	//���ݒ�
	const vorbis_info *info = ov_info(&vf, -1);
	Initialize(lpDS, info->rate);

	type = TYPE_OGG;
	StrPos = 0;
	StrSize = (DWORD)ov_pcm_total(&vf, -1);

	/*	�Đ�����	*/
	SetBlockOGG(0);

	return TRUE;
}


//**************************************************************************************************************
//	�Đ��Ǘ�
//**************************************************************************************************************

void iexStreamSoundIIDX::Stop()
{
	if (lpStream == NULL) return;
	if (hStrFile == NULL) return;

	lpStream->Stop();
}

void iexStreamSoundIIDX::SetVolume(int volume)
{
	int		vol;
	if (lpStream == NULL) return;
	/*	���ʃZ�b�g	*/
	if (volume > 255) volume = 255;
	if (volume < 0) volume = 0;
	this->volume = volume;
	volume -= 255;
	vol = (volume*volume * 100 / (255 * 255));
	lpStream->SetVolume(-vol*vol);
}

void iexStreamSoundIIDX::SetMode(BYTE mode, int param)
{
	this->mode = mode;
	this->param = param;
}




//**************************************************************************************************************
//
//		�T�E���h�}�l�[�W��
//
//**************************************************************************************************************

//**************************************************************************************************************
//
//**************************************************************************************************************
iexSoundIIDX::iexSoundIIDX()
{
	hWndWAV = iexSystem::Window;
	CoInitialize(NULL);
	//	DirectSound�̏�����
	if (DirectSoundCreate8(NULL, &lpDS, NULL) != DS_OK){
		lpDS = NULL;
		return;
	}

	lpDS->SetCooperativeLevel(hWndWAV, DSSCL_PRIORITY);
	for (int i = 0; i<WavNum; i++) buffer[i] = NULL;

	lpPrimary = NULL;
	DSBUFFERDESC	dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
	lpDS->CreateSoundBuffer(&dsbd, &lpPrimary, NULL);

	lp3DListener = NULL;
	//lpPrimary->QueryInterface(IID_IDirectSound3DListener, (LPVOID *)lp3DListener);

	WAVEFORMATEX   wfx;
	ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.nSamplesPerSec = 22050;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	lpPrimary->SetFormat(&wfx);

}

iexSoundIIDX::~iexSoundIIDX()
{
	int		i;

	//	�o�b�t�@�̉��
	for (i = 0; i<WavNum; i++){
		if (buffer[i] != NULL) delete buffer[i];
		buffer[i] = NULL;
	}

	//	Direct Sound���
	if (lpPrimary != NULL) lpPrimary->Release();
	if (lpDS != NULL) lpDS->Release();

	lpDS = NULL;
	lpPrimary = NULL;
}

//**************************************************************************************************************
//
//**************************************************************************************************************
void iexSoundIIDX::Set(int no, char* filename, bool b3D)
{
	//	�������`�F�b�N
	if (lpDS == NULL) return;
	//	�����̃o�b�t�@�̉��
	if (buffer[no] != NULL) delete buffer[no];
	buffer[no] = NULL;
	//	WAV�t�@�C���̃��[�h
	if (filename != NULL)
	{
		buffer[no] = new SoundBufferIIDX(lpDS, filename, b3D);
		if (buffer[no]->GetBuf() == NULL)
		{
			delete buffer[no];
			buffer[no] = NULL;
		}
	}
}

void iexSoundIIDX::Set_copy(char *filename, int dst, int count, bool b3D)
{
	if (count == 1)
	{
		Set(dst, filename, b3D);	// 1�����Ȃ��Ȃ�R�s�[����Ӗ��Ȃ�����Ȃ��I
		return;
	}

	//	�������`�F�b�N
	if (lpDS == NULL) return;

	for (int i = dst; i < dst + count; i++)
	{
		//	�����̃o�b�t�@�̉��
		if (buffer[i] != NULL) delete buffer[i];
		buffer[i] = NULL;

		buffer[i] = new SoundBufferIIDX();
	}


	//	WAV�t�@�C���̃��[�h
	if (filename != NULL)
	{
		SoundBufferIIDX::Create_and_copy(lpDS, filename, b3D, buffer, dst, count);
		if (buffer[dst]->GetBuf() == NULL)
		{

			assert(0);
		}
	}
}

//**************************************************************************************************************
//
//**************************************************************************************************************

void iexSoundIIDX::Play(int no, BOOL loop)
{
	//	�������`�F�b�N
	if (lpDS == NULL) return;
	//	�f�[�^�������I�I
	if (buffer[no] == NULL) return;
	buffer[no]->Play(loop);
}

void iexSoundIIDX::Stop(int no)
{
	if (lpDS == NULL) return;
	if (buffer[no] == NULL) return;
	buffer[no]->Stop();
}

void iexSoundIIDX::Pause(int no)
{
	if (lpDS == NULL) return;
	if (buffer[no] == NULL) return;
	buffer[no]->Pause();
}


/*	�{�����[���̐ݒ�	*/
void	iexSoundIIDX::SetVolume(int no, int volume)
{
	if (lpDS == NULL) return;
	if (buffer[no] == NULL) return;
	//	���ʃZ�b�g(0�`-10000�͈̔�)
	buffer[no]->SetVolume(volume);
}

int		iexSoundIIDX::GetVolume(int no)
{
	if (lpDS == NULL) return 0;
	if (buffer[no] == NULL) return 0;
	return buffer[no]->GetVolume();
}

/*	�p��(�X�e���I)�̐ݒ�	*/
void	iexSoundIIDX::SetPan(int no, int pan)
{
	if (lpDS == NULL) return;
	if (buffer[no] == NULL) return;
	//	�p���Z�b�g(-10000(��)�`0(�^��)�`10000(�E)�͈̔�)
	buffer[no]->SetPan(pan);
}
int		iexSoundIIDX::GetPan(int no)
{
	if (lpDS == NULL) return 0;
	if (buffer[no] == NULL) return 0;
	return buffer[no]->GetPan();
}

/*	���g���̐ݒ�	*/
void	iexSoundIIDX::SetFrequency(int no, int pitch)
{
	if (lpDS == NULL) return;
	if (buffer[no] == NULL) return;
	buffer[no]->SetFrequency(pitch);
}
int	iexSoundIIDX::GetFrequency(int no)
{
	if (lpDS == NULL) return 0;
	if (buffer[no] == NULL) return 0;
	return buffer[no]->GetFrequency();
}

/*	�Đ����x(���̂��Ƃ���Ă邾��)	*/
void iexSoundIIDX::SetSpeed(int no, float speed)
{
	if (lpDS == NULL) return;
	if (buffer[no] == NULL) return;
	buffer[no]->SetSpeed(speed);
}
float iexSoundIIDX::GetSpeed(int no)
{
	if (lpDS == NULL) return 0;
	if (buffer[no] == NULL) return 0;
	return buffer[no]->GetSpeed();
}


/*	�Đ��󋵂̃`�F�b�N	*/
BOOL	iexSoundIIDX::isPlay(int no)
{
	if (lpDS == NULL) return FALSE;
	if (buffer[no] == NULL) return FALSE;
	return buffer[no]->isPlay();
}

/*	pos(�Đ��ʒu)�̎擾	*/
DWORD	iexSoundIIDX::GetPlayCursor(int no)
{
	if (lpDS == NULL) return 0;
	if (buffer[no] == NULL) return 0;
	return buffer[no]->GetPlayCursor();
}
DWORD	iexSoundIIDX::GetPlayFrame(int no)
{
	if (lpDS == NULL) return 0;
	if (buffer[no] == NULL) return 0;
	return buffer[no]->GetPlayFrame();
}
int		iexSoundIIDX::GetPlaySecond(int no)
{
	if (lpDS == NULL) return 0;
	if (buffer[no] == NULL) return 0;
	return buffer[no]->GetPlaySecond();
}

void	iexSoundIIDX::SetPlaySecond(int no, int sec)
{
	if (lpDS == NULL) return;
	if (buffer[no] == NULL) return;
	buffer[no]->SetPlaySecond(sec);
}


DWORD iexSoundIIDX::GetSize(int no)
{
	if (lpDS == NULL) return 0;
	if (buffer[no] == NULL) return 0;
	return buffer[no]->GetSize();
}

int		iexSoundIIDX::GetLengthSecond(int no)
{
	if (lpDS == NULL) return 0;
	if (buffer[no] == NULL) return 0;
	return buffer[no]->GetLengthSecond();
}

//**************************************************************************************************************
//	�X�g���[���T�E���h�Ǘ�
//**************************************************************************************************************
iexStreamSoundIIDX* iexSoundIIDX::PlayStream(char* filename)
{
	return PlayStream(filename, STR_NORMAL, 0);
}

iexStreamSoundIIDX* iexSoundIIDX::PlayStream(char* filename, BYTE mode, int param)
{
	iexStreamSoundIIDX*	lpStream;

	//	�������`�F�b�N
	if (lpDS == NULL) return NULL;

	lpStream = new iexStreamSoundIIDX(lpDS, filename, mode, param);
	return lpStream;
}



//**************************************************************************************************************
//
//		3D�T�E���h
//
//**************************************************************************************************************

/*
DS3D_DEFERRED
�A�v���P�[�V������ IDirectSound3DListener8::CommitDeferredSettings ���\�b�h���Ăяo���܂ŁA
�ݒ�͓K�p����Ȃ��B�����̐ݒ��ύX������A1 ��̍Čv�Z�ł�����K�p�ł���B

DS3D_IMMEDIATE
�ݒ�𒼂��ɓK�p���A�V�X�e���͂��ׂĂ� 3D �T�E���h �o�b�t�@�� 3D ���W���Čv�Z����B
*/

iex3DSoundIIDX::iex3DSoundIIDX()
{
	// 3D���X�i�[�쐬
	lp3DListener = NULL;
	result = lpPrimary->QueryInterface(IID_IDirectSound3DListener, (LPVOID *)&lp3DListener);

	if (result == E_INVALIDARG)	// �v���V�[�W���̌Ăяo���A�܂��͈������s���炵��
	{
		assert(0);
	}

	this->Set_listener_all(Vector3(0, 0, 0), Vector3(0, 0, 1), Vector3(0, 1, 0), Vector3(0, 0, 0));
	this->Update();
}

iex3DSoundIIDX::~iex3DSoundIIDX()
{
	int		i;

	//	�o�b�t�@�̉��
	for (i = 0; i<WavNum; i++){
		if (buffer[i] != NULL) delete buffer[i];
		buffer[i] = NULL;
	}

	//	Direct Sound���
	if (lpPrimary != NULL) lpPrimary->Release();
	if (lpDS != NULL) lpDS->Release();
	//if (lp3DListener != NULL) lp3DListener->Release();

	lpDS = NULL;
	lpPrimary = NULL;
	lp3DListener = NULL;
}

void iex3DSoundIIDX::Set_dist(int no, float max_dist, float min_dist)
{
	buffer[no]->Set_dist(max_dist, min_dist);
}
void iex3DSoundIIDX::Set_pos(int no, const Vector3 &pos)
{
	buffer[no]->Set_pos(pos);
}
void iex3DSoundIIDX::Set_front(int no, const Vector3 &front)
{
	buffer[no]->Set_front(front);
}
void iex3DSoundIIDX::Set_range(int no, int degreeIn)
{
	buffer[no]->Set_range(degreeIn);
}
void iex3DSoundIIDX::Set_outRange_volume(int no, int out_vol)
{
	buffer[no]->Set_outRange_volume(out_vol);
}
void iex3DSoundIIDX::Set_move(int no, const Vector3 &move)
{
	buffer[no]->Set_move(move);
}
void iex3DSoundIIDX::Set_all3D(int no, float max_dist, float min_dist, const Vector3 &pos, const Vector3 &front, int degreeIn, int out_vol, const Vector3 &move)
{
	buffer[no]->Set_all3D(max_dist, min_dist, pos, front, degreeIn, out_vol, move);
}

//**************************************************************************************************************
//	������������l�̏��ݒ�
//**************************************************************************************************************
void iex3DSoundIIDX::Set_listener_pos(const Vector3 &pos)
{
	lp3DListener->SetPosition(pos.x, pos.y, pos.z, DS3D_DEFERRED);
}
void iex3DSoundIIDX::Set_listener_orientation(const Vector3 &front, const Vector3 &up)
{
	lp3DListener->SetOrientation(front.x, front.y, front.z, up.x, up.y, up.z, DS3D_DEFERRED);
}
void iex3DSoundIIDX::Set_listener_move(const Vector3 &move)
{
	lp3DListener->SetVelocity(move.x, move.y, move.z, DS3D_DEFERRED);
}
void iex3DSoundIIDX::Set_roll_off_factor(float pow)
{
	lp3DListener->SetRolloffFactor(pow, DS3D_DEFERRED);
}
void iex3DSoundIIDX::Set_listener_all(const Vector3 &pos, const Vector3 &front, const Vector3 &up, const Vector3 &velocity)
{
	DS3DLISTENER set;
	ZeroMemory(&set, sizeof(DS3DLISTENER));
	set.dwSize = sizeof(DS3DLISTENER);

	// ���X�i�[���ݒ�
	set.vPosition = D3DXVECTOR3(pos.x, pos.y, pos.z);
	set.vOrientFront = D3DXVECTOR3(front.x, front.y, front.z);
	set.vOrientTop = D3DXVECTOR3(up.x, up.y, up.z);
	set.vVelocity = D3DXVECTOR3(velocity.x, velocity.y, velocity.z);

	// �T�E���h�v�Z���ݒ�(�f�t�H���g�l)
	set.flDistanceFactor = DS3D_DEFAULTDISTANCEFACTOR;	// �x�N�g���P�ʂɂ����郁�[�g����
	set.flDopplerFactor = DS3D_DEFAULTDOPPLERFACTOR;	// �h�b�v���[���ʂɂ��Ă̒l
	set.flRolloffFactor = 0.01f;	// �����ɂ�錸���ɂ��Ă̒l

	lp3DListener->SetAllParameters(&set, DS3D_DEFERRED);
}
void iex3DSoundIIDX::Update()
{
	result = lp3DListener->CommitDeferredSettings();
	assert(result == DS_OK);
}



//*****************************************************************************************************************************
//
//	�T�E���h����֐��Q

void iex3DSoundIIDX::Play(int no, const Vector3 &pos, const Vector3 &front, BOOL loop)
{
	buffer[no]->Set_pos(pos);
	buffer[no]->Set_front(front);
	buffer[no]->Play(loop);
}