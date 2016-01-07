//*****************************************************************************************************************************
#include	"iextreme.h"

#include	"IEX_AudioIIDX.h"

HRESULT result_sound;

//**************************************************************************************************************
//
//		サウンドバッファ
//
//**************************************************************************************************************

//**************************************************************************************************************
//
//**************************************************************************************************************
fstSoundBuffer::fstSoundBuffer(LPDIRECTSOUND8 lpDS, char* filename, bool b3D)
{
	DSBUFFERDESC	dsbd;
	LPVOID			lpbuf1, lpbuf2;
	DWORD			dwbuf1, dwbuf2;

	lpBuf3D = nullptr;
	lpBuf = nullptr;

	/*	WAVファイルのロード	*/
	lpWBuf = LoadWAV(filename, &size, &wfx);
	/*	ロード失敗	*/
	if (lpWBuf == nullptr){
		return;
	}

	/* ファイル名保存 */
	sprintf_s(wav_file_path, sizeof(wav_file_path), "%s", filename);

	/* 二次バッファ作成	*/
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	if (b3D) dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRL3D | DSBCAPS_CTRLFX;	// CTRL_FX等、サウンド制御に必要なフラグをONにする(CTRL3DをONにするとCTRLPANを使えない)
	else dsbd.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFX;
	dsbd.dwBufferBytes = size;
	dsbd.lpwfxFormat = &wfx;

	/* 本来はDIRECTSOUNDBUFFERを使っていたが、BUFFER8に切り替えるために、一旦BUFFERを作る→そのBUFFERのQueriInterfaceの流れでBUFFER8を作成 */
	LPDIRECTSOUNDBUFFER lpWork;
	lpDS->CreateSoundBuffer(&dsbd, &lpWork, nullptr);
	lpWork->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&lpBuf);
	lpWork->Release();	// 用済み

	MyAssert(!b3D || result_sound != E_INVALIDARG || dsbd.lpwfxFormat->nChannels != 2, "3Dサウンドだからステレオ音源は使えないよ！\nモノラルに変換してね");
	//if (b3D && result == E_INVALIDARG && dsbd.lpwfxFormat->nChannels == 2)

	if (result_sound != DS_OK)
	{
		return;
	}

	lpBuf3D = nullptr;
	if (b3D == true)
	{
		/* サウンドバッファから3Dバッファに変換&作成 */
		lpBuf->QueryInterface(IID_IDirectSound3DBuffer8, (LPVOID*)&lpBuf3D);
		/* 3Dパラメータ初期化 */
		this->SetAll3D(DS3D_DEFAULTMAXDISTANCE, DS3D_DEFAULTMINDISTANCE, Vector3(0, 0, 0), Vector3(0, 0, -1), DS3D_MAXCONEANGLE, DS3D_DEFAULTCONEOUTSIDEVOLUME, Vector3(0, 0, 0));
	}

	/* 二次バッファのロック	*/
	lpBuf->Lock(0, size, &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0);
	/* 音源データの設定	*/
	memcpy(lpbuf1, lpWBuf, dwbuf1);
	if (dwbuf2 != 0) memcpy(lpbuf2, lpWBuf + dwbuf1, dwbuf2);
	/* 音源データの解放	*/
	GlobalFree(lpWBuf);
	/* 二次バッファのロック解除	*/
	lpBuf->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2);

	/* 自作初期化 */
	PlayCursor = 0;
	BufferSize = size;
	format = wfx;
}

void fstSoundBuffer::Create_and_copy(LPDIRECTSOUND8 lpDS, char* filename, bool b3D, fstSoundBuffer **buffers, int dst, int count)
{
	DSBUFFERDESC	dsbd;
	LPVOID			lpbuf1, lpbuf2;
	DWORD			dwbuf1, dwbuf2;


	//===========================================================
	//		コピー元作成
	//===========================================================
	buffers[dst]->lpBuf3D = nullptr;
	buffers[dst]->lpBuf = nullptr;

	/*	WAVファイルのロード	*/
	buffers[dst]->lpWBuf = buffers[dst]->LoadWAV(filename, &buffers[dst]->size, &buffers[dst]->wfx);

	/* ファイル名保存 */
	sprintf_s(buffers[dst]->wav_file_path, sizeof(buffers[dst]->wav_file_path), "%s", filename);

	/* 二次バッファ作成	*/
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	if (b3D) dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRL3D | DSBCAPS_CTRLFX;
	else dsbd.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFX;
	dsbd.dwBufferBytes = buffers[dst]->size;
	dsbd.lpwfxFormat = &buffers[dst]->wfx;

	LPDIRECTSOUNDBUFFER lpWork;
	result_sound = lpDS->CreateSoundBuffer(&dsbd, &lpWork, nullptr);

	result_sound = lpWork->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&buffers[dst]->lpBuf);	// LPDIRECTSOUNDBUFFER→LPDIRECTSOUNDBUFFER8
	lpWork->Release();	// 用済み

	MyAssert(!b3D || result_sound != E_INVALIDARG || dsbd.lpwfxFormat->nChannels != 2, "3Dサウンドだからステレオ音源は使えないよ！\nモノラルに変換してね");

	if (result_sound != DS_OK)
	{
		return;
	}

	buffers[dst]->lpBuf3D = nullptr;
	if (b3D)
	{
		buffers[dst]->lpBuf->QueryInterface(IID_IDirectSound3DBuffer8, (LPVOID*)&buffers[dst]->lpBuf3D);
		buffers[dst]->SetAll3D(DS3D_DEFAULTMAXDISTANCE, DS3D_DEFAULTMINDISTANCE, Vector3(0, 0, 0), Vector3(0, 0, -1), DS3D_MAXCONEANGLE, DS3D_DEFAULTCONEOUTSIDEVOLUME, Vector3(0, 0, 0));
	}

	/* 二次バッファのロック	*/
	buffers[dst]->lpBuf->Lock(0, buffers[dst]->size, &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0);
	/* 音源データの設定	*/
	memcpy(lpbuf1, buffers[dst]->lpWBuf, dwbuf1);
	if (dwbuf2 != 0) memcpy(lpbuf2, buffers[dst]->lpWBuf + dwbuf1, dwbuf2);
	/* 音源データの解放	*/
	//GlobalFree(lpWBuf);	// この文をコメントアウトするのに全てが詰まってる
	/* 二次バッファのロック解除	*/
	buffers[dst]->lpBuf->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2);

	/* 自作初期化 */
	buffers[dst]->PlayCursor = 0;
	buffers[dst]->BufferSize = buffers[dst]->size;
	buffers[dst]->format = buffers[dst]->wfx;


	//===========================================================
	//		コピー先作成
	//===========================================================
	for (int no = dst + 1; no < dst + count; no++)
	{

		DSBUFFERDESC	dsbd;
		LPVOID			lpbuf1 = nullptr, lpbuf2 = nullptr;
		DWORD			dwbuf1 = 0, dwbuf2 = 0;

		buffers[no]->lpBuf3D = nullptr;
		buffers[no]->lpBuf = nullptr;

		/* 二次バッファ作成	*/
		ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
		dsbd.dwSize = sizeof(DSBUFFERDESC);
		if (b3D == true) dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRL3D | DSBCAPS_CTRLFX;
		else dsbd.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFX;
		dsbd.dwBufferBytes = buffers[dst]->size;
		dsbd.lpwfxFormat = &buffers[dst]->wfx;

		LPDIRECTSOUNDBUFFER lpWork2;
		result_sound = lpDS->CreateSoundBuffer(&dsbd, &lpWork2, nullptr);

		if (result_sound == E_OUTOFMEMORY)
		{
			MessageBox(0, "wavファイルが読み込まれすぎてメモリが足りなくなってるよ。セットの数を減らすか、wavファイルの長さを短くしてね", nullptr, MB_OK);
			assert(0);
			return;
		}

		result_sound = lpWork2->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&buffers[no]->lpBuf);	// LPDIRECTSOUNDBUFFER→LPDIRECTSOUNDBUFFER8
		lpWork2->Release();	// 用済み

		buffers[no]->lpBuf3D = nullptr;
		if (b3D)
		{
			result_sound = buffers[no]->lpBuf->QueryInterface(IID_IDirectSound3DBuffer8, (LPVOID*)&buffers[no]->lpBuf3D);
			buffers[no]->SetAll3D(DS3D_DEFAULTMAXDISTANCE, DS3D_DEFAULTMINDISTANCE, Vector3(0, 0, 0), Vector3(0, 0, -1), DS3D_MAXCONEANGLE, DS3D_DEFAULTCONEOUTSIDEVOLUME, Vector3(0, 0, 0));
		}

		/* 二次バッファのロック	*/
		buffers[no]->lpBuf->Lock(0, buffers[dst]->size, &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0);
		/* 音源データの設定	*/
		memcpy(lpbuf1, buffers[dst]->lpWBuf, dwbuf1);
		if (dwbuf2 != 0) memcpy(lpbuf2, buffers[dst]->lpWBuf + dwbuf1, dwbuf2);
		/* 音源データの解放	*/
		//GlobalFree(buffers[no].lpWBuf);
		/* 二次バッファのロック解除	*/
		buffers[no]->lpBuf->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2);

		/* 自作初期化 */
		buffers[no]->PlayCursor = 0;
		buffers[no]->BufferSize = buffers[dst]->size;
		buffers[no]->format = buffers[dst]->wfx;
	}

	/* コピー元音源データの解放	*/
	GlobalFree(buffers[dst]->lpWBuf);
}

fstSoundBuffer::~fstSoundBuffer()
{
	if (lpBuf != nullptr)lpBuf->Release();
	if (lpBuf3D != nullptr) lpBuf3D->Release();
}

//**************************************************************************************************************
//		ＷＡＶファイルの読み込み
//**************************************************************************************************************
LPBYTE fstSoundBuffer::LoadWAV(LPSTR fname, LPDWORD size, LPWAVEFORMATEX wfx)
{
	HMMIO			hMMIO = nullptr;		/*	ファイルハンドル	*/
	PCMWAVEFORMAT	pwf;				/*	WAVデータ形式		*/
	MMCKINFO		ckparent, ckinfo;	/*	RIFFチャンク情報	*/
	MMIOINFO		mminfo;				/*	ファイル情報		*/
	DWORD			i;
	LPBYTE			buf = nullptr;			/*	読み込みバッファ	*/

	/* オープン	*/
	if ((hMMIO = mmioOpen(fname, nullptr, MMIO_ALLOCBUF | MMIO_READ)) == nullptr)
	{
		MessageBox(0, "wavファイルが入っていないか、wavファイル名が間違っているよ", nullptr, MB_OK);
		assert(0);
		return nullptr;
	}
	if (mmioDescend(hMMIO, &ckparent, nullptr, 0) != 0) goto WAVE_LoadError;
	/*	ＷＡＶ(RIFF)ファイルチェック		*/
	if ((ckparent.ckid != FOURCC_RIFF) || (ckparent.fccType != mmioFOURCC('W', 'A', 'V', 'E'))) goto WAVE_LoadError;
	/*	ｆｍｔチャンクに侵入		*/
	ckinfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(hMMIO, &ckinfo, &ckparent, MMIO_FINDCHUNK) != 0) goto WAVE_LoadError;
	if (ckinfo.cksize < sizeof(PCMWAVEFORMAT)) goto WAVE_LoadError;
	/*	チャンクからリード	*/
	if (mmioRead(hMMIO, (HPSTR)&pwf, sizeof(pwf)) != sizeof(pwf)) goto WAVE_LoadError;
	if (pwf.wf.wFormatTag != WAVE_FORMAT_PCM) goto WAVE_LoadError;
	/*	WAVフォーマットの保存	*/
	memset(wfx, 0, sizeof(WAVEFORMATEX));
	memcpy(wfx, &pwf, sizeof(pwf));
	/*	データの読み込み	*/
	if (mmioSeek(hMMIO, ckparent.dwDataOffset + sizeof(FOURCC), SEEK_SET) == -1) goto WAVE_LoadError;
	/*	ｄａｔａチャンクに侵入		*/
	ckinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(hMMIO, &ckinfo, &ckparent, MMIO_FINDCHUNK) != 0) goto WAVE_LoadError;
	if (mmioGetInfo(hMMIO, &mminfo, 0) != 0) goto WAVE_LoadError;
	/*	バッファサイズ保存	*/
	if (size != nullptr) *size = ckinfo.cksize;
	/*	ＷＡＶ用バッファの取得	*/
	buf = (LPBYTE)GlobalAlloc(LPTR, ckinfo.cksize);
	if (buf == nullptr) goto WAVE_LoadError;
	/*	データの読みとり	*/
	for (i = 0; i < ckinfo.cksize; i++){
		/*	エラーチェック	*/
		if (mminfo.pchNext >= mminfo.pchEndRead){
			if (mmioAdvance(hMMIO, &mminfo, MMIO_READ) != 0) goto WAVE_LoadError;
			if (mminfo.pchNext >= mminfo.pchEndRead) goto WAVE_LoadError;
		}
		*(buf + i) = *((LPBYTE)mminfo.pchNext);
		mminfo.pchNext++;
	}
	/*	ファイルアクセス終了	*/
	mmioSetInfo(hMMIO, &mminfo, 0);
	mmioClose(hMMIO, 0);
	return buf;

WAVE_LoadError:	/*	エラー終了	*/
	mmioClose(hMMIO, 0);
	if (buf != nullptr) GlobalFree(buf);
	return nullptr;
}


//**************************************************************************************************************
//	再生管理
//**************************************************************************************************************
//-------------------------------------------------------------
//	再生	
//-------------------------------------------------------------
void fstSoundBuffer::Play(bool loop, DWORD cursor)
{
	lpBuf->Stop();
	lpBuf->SetCurrentPosition(cursor);
	//	ループ再生
	if (loop) lpBuf->Play(0, 0, DSBPLAY_LOOPING);
	//	ノーマル再生
	else	   lpBuf->Play(0, 0, 0);

	//	lpBuf->SetFrequency(8000);
	PlayCursor = 0;
	loop_flag = loop;
}

//-------------------------------------------------------------
//	停止	
//-------------------------------------------------------------
void fstSoundBuffer::Stop()
{
	lpBuf->Stop();
}

void fstSoundBuffer::Pause()
{
	lpBuf->GetCurrentPosition(&PlayCursor, nullptr);
	lpBuf->Stop();
}
void fstSoundBuffer::PauseOff()
{
	lpBuf->SetCurrentPosition(PlayCursor);
	//	ループ再生
	if (loop_flag) lpBuf->Play(0, 0, DSBPLAY_LOOPING);
	//	ノーマル再生
	else	   lpBuf->Play(0, 0, 0);

	PlayCursor = 0;
}

//-------------------------------------------------------------
//	ボリューム変更
//-------------------------------------------------------------
void fstSoundBuffer::SetVolume(int volume)
{
	lpBuf->SetVolume(volume);
}

//-------------------------------------------------------------
//	ボリュームゲッター
//-------------------------------------------------------------
int	fstSoundBuffer::GetVolume()
{
	LONG ret;
	lpBuf->GetVolume(&ret);
	return ret;
}

//-------------------------------------------------------------
//	ステレオ(左右音)関係
//-------------------------------------------------------------
void fstSoundBuffer::SetPan(int pan)
{
	lpBuf->SetPan(pan);
}

int fstSoundBuffer::GetPan()
{
	LONG ret;
	lpBuf->GetPan(&ret);
	return ret;
}


//-------------------------------------------------------------
//	周波数関係(再生速度・ピッチ)
//-------------------------------------------------------------
void fstSoundBuffer::SetFrequency(int pitch)
{
	lpBuf->SetFrequency(pitch);
}

int fstSoundBuffer::GetFrequency()
{
	DWORD ret;
	lpBuf->GetFrequency(&ret);
	return ret;
}

//-------------------------------------------------------------
//	再生速度(上の事やってるだけ)
//-------------------------------------------------------------
void fstSoundBuffer::SetSpeed(float speed)
{
	DWORD frequency = (DWORD)(format.nSamplesPerSec*speed);
	lpBuf->SetFrequency(frequency);
}

float fstSoundBuffer::GetSpeed()
{
	DWORD work;
	lpBuf->GetFrequency(&work);

	return ((float)work / format.nSamplesPerSec);
}


//-------------------------------------------------------------
//	再生チェック	
//-------------------------------------------------------------
bool fstSoundBuffer::isPlay()
{
	DWORD	dwAns;
	lpBuf->GetStatus(&dwAns);
	return ((dwAns&DSBSTATUS_PLAYING) != 0) ? true : false;
}


//-------------------------------------------------------------
//	再生位置関係
//-------------------------------------------------------------
DWORD fstSoundBuffer::GetPlayCursor()
{
	DWORD ret;
	lpBuf->GetCurrentPosition(&ret, nullptr);

	return ret;
}

void fstSoundBuffer::SetPlayCursor(DWORD new_position)
{
	lpBuf->SetCurrentPosition(new_position);
}

DWORD fstSoundBuffer::GetPlayFrame()
{
	return (GetPlayCursor() / (format.nAvgBytesPerSec / 60));
}

int fstSoundBuffer::GetPlaySecond()
{
	return (GetPlayCursor() / format.nAvgBytesPerSec);
}

void fstSoundBuffer::SetPlaySecond(int sec)
{
	DWORD set = sec * format.nAvgBytesPerSec;
	lpBuf->SetCurrentPosition(set);
}

DWORD fstSoundBuffer::GetSize()
{
	return BufferSize;
}

int fstSoundBuffer::GetLengthSecond()
{
	return (BufferSize / format.nAvgBytesPerSec);
}

//-------------------------------------------------------------
//	3Dサウンド関係
//-------------------------------------------------------------
void fstSoundBuffer::SetDist(float max_dist, float min_dist)
{
	lpBuf3D->SetMaxDistance(max_dist, DS3D_DEFERRED), lpBuf3D->SetMinDistance(min_dist, DS3D_DEFERRED);
}
void fstSoundBuffer::SetPos(const Vector3 &pos)
{
	lpBuf3D->SetPosition(pos.x, pos.y, pos.z, DS3D_DEFERRED);
}
void fstSoundBuffer::SetFront(const Vector3 &front)
{
	lpBuf3D->SetConeOrientation(front.x, front.y, front.z, DS3D_DEFERRED);
}
void fstSoundBuffer::SetRange(int degreeIn)
{
	lpBuf3D->SetConeAngles(degreeIn, (DS3D_MAXCONEANGLE - degreeIn), DS3D_DEFERRED);
}
void fstSoundBuffer::SetOutRange_volume(int out_vol)
{
	lpBuf3D->SetConeOutsideVolume(out_vol, DS3D_DEFERRED);
}
void fstSoundBuffer::SetMove(const Vector3 &move)
{
	lpBuf3D->SetVelocity(move.x, move.y, move.z, DS3D_DEFERRED);
}
void fstSoundBuffer::SetAll3D(float max_dist, float min_dist, const Vector3 &pos, const Vector3 &front, int degreeIn, int out_vol, const Vector3 &move)
{
	DS3DBUFFER set;
	ZeroMemory(&set, sizeof(DS3DBUFFER));
	set.dwSize = sizeof(DS3DBUFFER);

	/*
	DS3DMODE_HEADRELATIVE
	サウンド パラメータ(位置・速度・向き) は、リスナーのパラメータに対して相対的なものである。
	このモードでは、リスナーのパラメータが変化するとサウンドの絶対パラメータは自動的に更新されるので、
	相対パラメータは一定に保たれる。

	DS3DMODE_NORMAL 
	標準の処理。これはデフォルトのモードである。
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

//-------------------------------------------------------------
//	エフェクト
//-------------------------------------------------------------
void fstSoundBuffer::SetFX(DXA_FX flag)
{
	bool isPlay(this->isPlay());
	// 演奏を停止し、エフェクトを全削除する(演奏中は設定不可らしい)
	if (isPlay)this->Pause();
	lpBuf->SetFX(0, nullptr, nullptr);
	if (flag == DXA_FX::DXAFX_OFF)
	{
		if (isPlay)this->PauseOff();
		return;
	}

	// エフェクト構造体設定
	DSEFFECTDESC ed;
	ZeroMemory(&ed, sizeof(ed));
	ed.dwSize = sizeof(DSEFFECTDESC);

	switch (flag)
	{
	case DXA_FX::DXAFX_CHORUS:ed.guidDSFXClass = GUID_DSFX_STANDARD_CHORUS;
		break;
	case DXA_FX::DXAFX_COMPRESSOR:ed.guidDSFXClass = GUID_DSFX_STANDARD_COMPRESSOR;
		break;
	case DXA_FX::DXAFX_DISTORTION:ed.guidDSFXClass = GUID_DSFX_STANDARD_DISTORTION;
		break;
	case DXA_FX::DXAFX_ECHO:ed.guidDSFXClass = GUID_DSFX_STANDARD_ECHO;
		break;
	case DXA_FX::DXAFX_FLANGER:ed.guidDSFXClass = GUID_DSFX_STANDARD_FLANGER;
		break;
	case DXA_FX::DXAFX_GARGLE:ed.guidDSFXClass = GUID_DSFX_STANDARD_GARGLE;
		break;
	case DXA_FX::DXAFX_ENVREVERB:ed.guidDSFXClass = GUID_DSFX_STANDARD_I3DL2REVERB;
		break;
	case DXA_FX::DXAFX_PARAMEQ:ed.guidDSFXClass = GUID_DSFX_STANDARD_PARAMEQ;
		break;
	case DXA_FX::DXAFX_WAVESREVERB:ed.guidDSFXClass = GUID_DSFX_WAVES_REVERB;
		break;
	}

	// DirectSoundに渡す
	result_sound = lpBuf->SetFX(1, &ed, nullptr);

	MyAssert(result_sound == S_OK || format.wBitsPerSample == 16 || flag != DXA_FX::DXAFX_WAVESREVERB, "ミュージックリバーブエフェクトの設定は16bitのオーディオフォーマットのみです");

	// 再生
	if (isPlay)this->PauseOff();

	//lpBuf->GetObjectInPath(GUID_DSFX_STANDARD_DISTORTION, 1, IDirectSoundFXDistortion8,)
}

//**************************************************************************************************************
//
//		ストリームサウンド
//
//**************************************************************************************************************

static int	NumStream = 0;
static bool ForceEnd = false;

//**************************************************************************************************************
//		管理スレッド
//**************************************************************************************************************
DWORD WINAPI ThreadIIDX(LPDWORD lpdwParam)
{
	DWORD	param;
	fstStreamSound*	lpStream;

	lpStream = (fstStreamSound*)(lpdwParam);
	for (;;){
		if (lpStream == nullptr) break;
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
fstStreamSound::fstStreamSound(LPDIRECTSOUND lpDS, LPSTR filename, BYTE mode, int param)
{
	NumStream++;

	LPSTR	work;
	BOOL	bInit;
	//	ストリームファイルを開く
	work = &filename[lstrlen(filename) - 4];
	if (lstrcmpi(work, ".wav") == 0) bInit = SetWAV(lpDS, filename);
	if (lstrcmpi(work, ".ogg") == 0) bInit = SetOGG(lpDS, filename);

	lpStream->SetCurrentPosition(0);
	if (mode != STR_FADEIN) SetVolume(255);
	else SetVolume(0);
	/*	管理スレッドの作成	*/
	hStrThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ThreadIIDX, this, 0, &dwThreadId);
	if (hStrThread == nullptr) return;
	/*	再生開始	*/
	lpStream->Play(0, 0, DSBPLAY_LOOPING);

	this->mode = mode;
	this->param = param;
}



fstStreamSound::~fstStreamSound()
{
	if (lpStream != nullptr){
		if (type == TYPE_OGG) ov_clear(&vf);
		else if (hStrFile != nullptr) fclose(hStrFile);

		if (lpStrNotify != nullptr) lpStrNotify->Release();
		lpStrNotify = nullptr;
		/*	バッファ開放	*/
		if (lpStream != nullptr) lpStream->Release();
		lpStream = nullptr;
	}

	NumStream--;
}

//**************************************************************************************************************
//
//**************************************************************************************************************
//-------------------------------------------------------------
//	ブロック作成
//-------------------------------------------------------------
BOOL	fstStreamSound::OGGRead(LPBYTE dst, unsigned long size)
{
	DWORD	remain = size;
	char*	dstPtr = (char*)dst;
	while (remain > 0){
		long actualRead = ov_read(&vf, dstPtr, remain, 0, 2, 1, nullptr);
		//終端チェック
		if (actualRead <= 0){
			if (ov_pcm_seek(&vf, 0)) return FALSE;
		}
		remain -= actualRead;
		dstPtr += actualRead;
	}
	return TRUE;
}

BOOL	fstStreamSound::SetBlockOGG(int block)
{
	LPBYTE	blk1, blk2;
	DWORD	bs1, bs2;

	DWORD	CurPos;

	CurPos = StrSize - StrPos;
	//	バッファロック
	lpStream->Lock((rate * 4 * STRSECOND)*block, rate * 4 * STRSECOND, (LPVOID*)&blk1, &bs1, (LPVOID*)&blk2, &bs2, 0);
	//	ブロック１へのデータセット
	OGGRead(blk1, bs1);
	StrPos += bs1;
	//	ブロック２へのデータセット
	if (blk2){
		OGGRead(blk2, bs2);
		StrPos += bs2;
	}

	lpStream->Unlock(blk1, bs1, blk2, bs2);
	return TRUE;
}


BOOL	fstStreamSound::SetBlockWAV(int block)
{
	LPBYTE	blk1, blk2;
	DWORD	bs1, bs2, work;

	DWORD	CurPos;

	CurPos = StrSize - StrPos;
	lpStream->Lock((rate * 4 * STRSECOND)*block, rate * 4 * STRSECOND, (LPVOID*)&blk1, &bs1, (LPVOID*)&blk2, &bs2, 0);

	work = fread(blk1, bs1, 1, hStrFile);
	StrPos += work;
	if (work < bs1){
		/*	巻き戻し	*/
		fseek(hStrFile, LoopPtr, SEEK_SET);
		work = fread(blk1 + work, bs1 - work, 1, hStrFile);
		StrPos = work;
	}

	if (blk2){
		work = fread(blk2, bs2, 1, hStrFile);
		StrPos += work;
		if (work < bs2){
			/*	巻き戻し	*/
			fseek(hStrFile, LoopPtr, SEEK_SET);
			work = fread(blk2 + work, bs2 - work, 1, hStrFile);
			StrPos = work;
		}
	}

	lpStream->Unlock(blk1, bs1, blk2, bs2);
	return TRUE;
}


//**************************************************************************************************************
//	データ管理
//**************************************************************************************************************
void fstStreamSound::Initialize(LPDIRECTSOUND lpDS, int rate)
{
	DSBUFFERDESC	dsbd;
	WAVEFORMATEX	wfx;


	/*	初期化チェック	*/
	if (lpDS == nullptr) return;
	/*	ＷＡＶＥフォーマット初期化	*/
	ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 2;			/*	stereo	*/
	wfx.nSamplesPerSec = rate;
	wfx.wBitsPerSample = 16;			/*	16bit	*/
	wfx.nBlockAlign = 4;
	wfx.nAvgBytesPerSec = rate * 4;
	/* 二次バッファの初期化	*/
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_LOCSOFTWARE;
	dsbd.dwBufferBytes = rate * 4 * STRSECOND * 2;
	dsbd.lpwfxFormat = &wfx;
	if (lpDS->CreateSoundBuffer(&dsbd, &lpStream, nullptr) != DS_OK) return;
	lpStream->SetFormat(&wfx);

	if (lpStream->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&lpStrNotify) != DS_OK) return;
	/*	位置イベント作成	*/
	hEvent[0] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	hEvent[1] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	hEvent[2] = CreateEvent(nullptr, FALSE, FALSE, nullptr);

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


BOOL	fstStreamSound::SetWAV(LPDIRECTSOUND lpDS, char* filename)
{
	HMMIO			hMMIO = nullptr;		/*	ファイルハンドル	*/
	MMCKINFO		ckinfo, ckparent;	/*	RIFFチャンク情報	*/
	LRESULT			ptr;

	/* オープン	*/
	hMMIO = mmioOpen(filename, nullptr, MMIO_ALLOCBUF | MMIO_READ);
	mmioDescend(hMMIO, &ckparent, nullptr, 0);
	/*	ｄａｔａチャンクに侵入		*/
	ckinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmioDescend(hMMIO, &ckinfo, &ckparent, MMIO_FINDCHUNK);
	//	現在位置取得
	ptr = mmioSeek(hMMIO, 0, SEEK_CUR);
	/*	ファイルアクセス終了	*/
	mmioClose(hMMIO, 0);
	if (ptr == -1) return FALSE;

	/*	ファイルオープン	*/
	hStrFile = fopen(filename, "rb");
	if (hStrFile == nullptr) return FALSE;
	//	ストリーム情報設定
	StrPos = 0;
	LoopPtr = ptr;

	fseek(hStrFile, 0L, SEEK_END);
	StrSize = ftell(hStrFile) - LoopPtr;

	StrSize = GetFileSize(hStrFile, nullptr) - LoopPtr;
	//	ファイルシーク
	fseek(hStrFile, LoopPtr, SEEK_SET);

	type = TYPE_WAV;
	Initialize(lpDS, 22050);

	/*	再生準備	*/
	SetBlockWAV(0);

	return TRUE;
}

//
//	OGG用ストリーム初期化
//

BOOL	fstStreamSound::SetOGG(LPDIRECTSOUND lpDS, char* filename)
{
	//	ファイルオープン 
	hStrFile = fopen(filename, "rb");
	if (hStrFile == nullptr) return FALSE;

	//Oggを開く
	ov_open(hStrFile, &vf, nullptr, 0);

	//シーク可能?
	if (!ov_seekable(&vf)){
		ov_clear(&vf);
		return FALSE;
	}

	//情報設定
	const vorbis_info *info = ov_info(&vf, -1);
	Initialize(lpDS, info->rate);

	type = TYPE_OGG;
	StrPos = 0;
	StrSize = (DWORD)ov_pcm_total(&vf, -1);

	/*	再生準備	*/
	SetBlockOGG(0);

	return TRUE;
}


//**************************************************************************************************************
//	再生管理
//**************************************************************************************************************

void fstStreamSound::Stop()
{
	if (lpStream == nullptr) return;
	if (hStrFile == nullptr) return;

	lpStream->Stop();
}

void fstStreamSound::SetVolume(int volume)
{
	int		vol;
	if (lpStream == nullptr) return;
	/*	音量セット	*/
	if (volume > 255) volume = 255;
	if (volume < 0) volume = 0;
	this->volume = volume;
	volume -= 255;
	vol = (volume*volume * 100 / (255 * 255));
	lpStream->SetVolume(-vol*vol);
}

void fstStreamSound::SetMode(BYTE mode, int param)
{
	this->mode = mode;
	this->param = param;
}




//**************************************************************************************************************
//
//		サウンドマネージャ基底
//
//**************************************************************************************************************

//=============================================================================================
//		初	期	化
fstSoundBase::fstSoundBase()
{
	hWndWAV = iexSystem::Window;
	CoInitialize(nullptr);
	//	DirectSoundの初期化
	if (DirectSoundCreate8(nullptr, &lpDS, nullptr) != DS_OK){
		lpDS = nullptr;
		return;
	}

	lpDS->SetCooperativeLevel(hWndWAV, DSSCL_PRIORITY);

	lpPrimary = nullptr;
	DSBUFFERDESC	dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
	lpDS->CreateSoundBuffer(&dsbd, &lpPrimary, nullptr);

	// 3Dリスナー作成
	lp3DListener = nullptr;
	result_sound = lpPrimary->QueryInterface(IID_IDirectSound3DListener, (LPVOID *)&lp3DListener);

	if (result_sound == E_INVALIDARG)	// プロシージャの呼び出し、または引数が不正らしい
	{
		assert(0);
	}

	this->SetListenerAll(Vector3(0, 0, 0), Vector3(0, 0, 1), Vector3(0, 1, 0), Vector3(0, 0, 0));
	this->UpdateListener();

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
//
//=============================================================================================


//=============================================================================================
//		解		放
fstSoundBase::~fstSoundBase()
{
	//	バッファの解放
	//for (int i = 0; i < WavNum; i++) for (UINT j = 0; j < buffer[i].size(); j++)
	//{
	//	SAFE_DELETE(buffer[i][j]);
	//}

	//	Direct Sound解放
	if (lpPrimary != nullptr) lpPrimary->Release();
	if (lpDS != nullptr) lpDS->Release();

	lpDS = nullptr;
	lpPrimary = nullptr;
}
//
//=============================================================================================

//**************************************************************************************************************
//
//		サウンドマネージャ(SE用)
//
//**************************************************************************************************************

//=============================================================================================
//		初	期	化
fstSoundSE::fstSoundSE()
{
	for (int i = 0; i < WavNum; i++)data[i].clear();
}
//
//=============================================================================================


//=============================================================================================
//		解		放
fstSoundSE::~fstSoundSE()
{
	// バッファ解放
	for (int i = 0; i < WavNum; i++)
	{
		for (UINT j = 0; j < data[i].size(); j++)
		{
			delete data[i][j]->buffer;
			delete data[i][j];
		}
	}
}
//
//=============================================================================================

//=============================================================================================
//		waveファイルセット
void fstSoundSE::Set(int ID, int num_of_play_simultaneously, char* filename, bool b3D)
{
	//	初期化チェック
	assert(lpDS);
	//	既存のバッファの解放
	if (ID < (int)data[ID].size())
	{
		for (UINT i = 0; i < data[i].size(); i++) SAFE_DELETE(data[ID][i]);
		data[ID].clear();
	}
	assert(filename);

	if (num_of_play_simultaneously == 1)
	{	// 1個しかないならコピーする意味ないじゃない！
		SEData *set = new SEData;
		set->buffer = new fstSoundBuffer(lpDS, filename, b3D);
		set->b3D = b3D;
		data[ID].push_back(set);
	}
	else
	{
		fstSoundBuffer **work = new fstSoundBuffer*[num_of_play_simultaneously];
		// 同時再生可能分のWAVファイルロード
		for (int i = 0; i < num_of_play_simultaneously; i++)
		{
			SEData *set = new SEData;
			set->buffer = new fstSoundBuffer();
			set->b3D = b3D;
			data[ID].push_back(set);
			work[i] = data[ID][i]->buffer;	// アドレスを渡す
		}
		fstSoundBuffer::Create_and_copy(lpDS, filename, b3D, work, 0, num_of_play_simultaneously);	// 上でworkにアドレスを渡したのでここでもうbuffer[ID]には作られている
		delete[] work;
		assert(data[ID][0]->buffer->GetBuf());
	}
}
//
//=============================================================================================

//=============================================================================================
//		再		生
int fstSoundSE::Play(int ID, bool loop)
{
	//	初期化チェック
	assert(lpDS);
	//	データが無い！！
	assert(data[ID].size() != 0);

	for (UINT play_no = 0; play_no < data[ID].size(); play_no++)
	{
		// 再生してないからいつでも514状態の人を検索
		if (!data[ID][play_no]->buffer->isPlay())
		{	// 見つかった！
			data[ID][play_no]->buffer->Play(loop);
			return play_no;
		}
	}
	
	// 全員再生状態だったので、再生失敗
	return -1;
}

int fstSoundSE::Play(int ID, const Vector3 &pos, const Vector3 &front, const Vector3 &move, bool loop)
{
	MyAssert(data[ID][0]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");

	//	初期化チェック
	assert(lpDS);
	//	データが無い！！
	assert(data[ID].size() != 0);

	for (UINT play_no = 0; play_no < data[ID].size(); play_no++)
	{
		// 再生してないからいつでも514状態の人を検索
		if (!data[ID][play_no]->buffer->isPlay())
		{	// 見つかった！
			data[ID][play_no]->buffer->SetAll3D(DS3D_DEFAULTMAXDISTANCE, DS3D_DEFAULTMINDISTANCE, pos, front, DS3D_DEFAULTCONEANGLE, DS3D_DEFAULTCONEOUTSIDEVOLUME, move);
			data[ID][play_no]->buffer->Play(loop);
			return play_no;
		}
	}

	// 全員再生状態だったので、再生失敗
	return -1;
}
//
//=============================================================================================

//=============================================================================================
//		停		止
void fstSoundSE::Stop(int ID, int no)
{
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->Stop();
}
void fstSoundSE::AllStop()
{
	assert(lpDS);
	for (int i = 0; i < WavNum; i++) for (UINT j = 0; j < data[i].size(); j++)if (data[i][j]->buffer->isPlay())data[i][j]->buffer->Stop();
}
//
//=============================================================================================

//=============================================================================================
//		ボリューム(-10000～0)
void fstSoundSE::SetVolume(int ID, int volume)
{
	if (data[ID][0]->b3D)return;	// 3Dサウンドに処理を任せているのでこちら側で音はいじれない
	assert(lpDS);
	assert(data[ID].size() != 0);
	for (UINT i = 0; i < data[ID].size(); i++) data[ID][i]->buffer->SetVolume(volume);	// ID分全部設定してるが、各自設定したい場合は、また作ります。
}
void fstSoundSE::SetVolume(int ID, float volume)
{
	SetVolume(ID, (int)(-5000 * volume));
}
int	fstSoundSE::GetVolume(int ID)
{
	assert(lpDS);
	assert(data[ID].size() != 0);
	return data[ID][0]->buffer->GetVolume();
}
//
//=============================================================================================

//=============================================================================================
//		ステレオ(-10000～0～10000)
void	fstSoundSE::SetPan(int ID, int pan)
{
	if (data[ID][0]->b3D)return;	// 3Dサウンドに処理を任せているのでこちら側で音はいじれない
	assert(lpDS);
	assert(data[ID].size() != 0);
	for (UINT i = 0; i < data[ID].size(); i++) data[ID][i]->buffer->SetPan(pan);
}
int		fstSoundSE::GetPan(int ID)
{
	assert(lpDS);
	assert(data[ID].size() != 0);
	return data[ID][0]->buffer->GetPan();
}
//
//=============================================================================================

//=============================================================================================
//		周波数
void fstSoundSE::SetFrequency(int ID, int frequency)
{
	assert(lpDS);
	assert(data[ID].size() != 0);
	for (UINT i = 0; i < data[ID].size(); i++) data[ID][i]->buffer->SetFrequency(frequency);
}
void fstSoundSE::SetFrequency(int ID, int no, int frequency)
{
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->SetFrequency(frequency);
}
int	fstSoundSE::GetFrequency(int ID, int no)
{
	assert(lpDS);
	assert(data[ID].size() != 0);
	return data[ID][no]->buffer->GetFrequency();
}
//
//=============================================================================================

//=============================================================================================
//		再生速度(周波数いじってるだけ)
void fstSoundSE::SetSpeed(int ID, float speed)
{
	assert(lpDS);
	assert(data[ID].size() != 0);
	for (UINT i = 0; i < data[ID].size(); i++) data[ID][i]->buffer->SetSpeed(speed);
}

void fstSoundSE::SetSpeed(int ID, int no, float speed)
{
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->SetSpeed(speed);
}
//
//=============================================================================================

//=============================================================================================
//		再生状態(再生中or停止中)
bool fstSoundSE::isPlay(int ID, int no)
{
	assert(lpDS);
	assert(data[ID].size() != 0);
	return data[ID][no]->buffer->isPlay();
}
//
//=============================================================================================

//=============================================================================================
//		3Dサウンドでの聞こえる人情報設定
/*
DS3D_DEFERRED
アプリケーションが IDirectSound3DListener8::CommitDeferredSettings メソッドを呼び出すまで、
設定は適用されない。複数の設定を変更した後、1 回の再計算でそれらを適用できる。

DS3D_IMMEDIATE
設定を直ちに適用し、システムはすべての 3D サウンド バッファの 3D 座標を再計算する。
*/
void fstSoundBase::SetListenerPos(const Vector3 &pos)
{
	lp3DListener->SetPosition(pos.x, pos.y, pos.z, DS3D_DEFERRED);
}
void fstSoundBase::SetListenerOrientation(const Vector3 &front, const Vector3 &up)
{
	lp3DListener->SetOrientation(front.x, front.y, front.z, up.x, up.y, up.z, DS3D_DEFERRED);
}
void fstSoundBase::SetListenerMove(const Vector3 &move)
{
	lp3DListener->SetVelocity(move.x, move.y, move.z, DS3D_DEFERRED);
}
void fstSoundBase::SetListenerAll(const Vector3 &pos, const Vector3 &front, const Vector3 &up, const Vector3 &velocity)
{
	DS3DLISTENER set;
	ZeroMemory(&set, sizeof(DS3DLISTENER));
	set.dwSize = sizeof(DS3DLISTENER);

	// リスナー情報設定
	set.vPosition = D3DXVECTOR3(pos.x, pos.y, pos.z);
	set.vOrientFront = D3DXVECTOR3(front.x, front.y, front.z);
	set.vOrientTop = D3DXVECTOR3(up.x, up.y, up.z);
	set.vVelocity = D3DXVECTOR3(velocity.x, velocity.y, velocity.z);

	// サウンド計算情報設定(デフォルト値)
	set.flDistanceFactor = DS3D_DEFAULTDISTANCEFACTOR;	// ベクトル単位におけるメートル数
	set.flDopplerFactor = DS3D_DEFAULTDOPPLERFACTOR;	// ドップラー効果についての値
	set.flRolloffFactor = 0.001f;	// 距離による減衰についての値

	lp3DListener->SetAllParameters(&set, DS3D_DEFERRED);
}
void fstSoundBase::UpdateListener()
{
	result_sound = lp3DListener->CommitDeferredSettings();
	assert(result_sound == DS_OK);
}
//
//=============================================================================================

//=============================================================================================
//		3Dサウンドでの音源情報設定
void fstSoundSE::SetDist(int ID, int no, float max_dist, float min_dist)
{
	MyAssert(data[ID][no]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->SetDist(max_dist, min_dist);
}
void fstSoundSE::SetPos(int ID, int no, const Vector3 &pos)
{
	MyAssert(data[ID][no]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->SetPos(pos);
}
void fstSoundSE::SetFront(int ID, int no, const Vector3 &front)
{
	MyAssert(data[ID][no]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->SetFront(front);
}
void fstSoundSE::SetRange(int ID, int no, int degreeIn)
{
	MyAssert(data[ID][no]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->SetRange(degreeIn);
}
void fstSoundSE::SetOutRange_volume(int ID, int no, int out_vol)
{
	MyAssert(data[ID][no]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->SetOutRange_volume(out_vol);
}
void fstSoundSE::SetMove(int ID, int no, const Vector3 &move)
{
	MyAssert(data[ID][no]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->SetMove(move);
}
void fstSoundSE::SetAll3D(int ID, int no, float max_dist, float min_dist, const Vector3 &pos, const Vector3 &front, int degreeIn, int out_vol, const Vector3 &move)
{
	MyAssert(data[ID][no]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->SetAll3D(max_dist, min_dist, pos, front, degreeIn, out_vol, move);
}
//
//=============================================================================================

//=============================================================================================
//		エフェクトセット
void fstSoundSE::SetFX(DXA_FX flag)
{
	for (int i = 0; i < WavNum; i++)for (UINT j = 0; j < data[i].size(); j++)data[i][j]->buffer->SetFX(flag);
}
void fstSoundSE::SetFX(int ID, DXA_FX flag)
{
	for (UINT i = 0; i < data[ID].size(); i++)data[ID][i]->buffer->SetFX(flag);
}
//
//=============================================================================================


//**************************************************************************************************************
//
//		サウンドマネージャ(BGM用)
//
//**************************************************************************************************************

//=============================================================================================
//		初	期	化
fstSoundBGM::fstSoundBGM()
{
	data.clear();
	Fade_funk[(int)MODE::NONE] = &fstSoundBGM::None;
	Fade_funk[(int)MODE::FADE_IN] = &fstSoundBGM::In;
	Fade_funk[(int)MODE::FADE_OUT] = &fstSoundBGM::Out;
}
//
//=============================================================================================


//=============================================================================================
//		解		放
fstSoundBGM::~fstSoundBGM()
{
	// バッファ解放
	for (UINT i = 0; i < data.size(); i++)
	{
		delete data[i]->buffer;
		delete data[i];
	}
	data.clear();
}
//
//=============================================================================================

//=============================================================================================
//		更		新
void fstSoundBGM::Update()
{
	for (UINT i = 0; i < data.size(); i++)
	{
		(this->*Fade_funk[(int)data[i]->fade_mode])(i);	// モード分岐
	}
}

void fstSoundBGM::None(int no){}
void fstSoundBGM::In(int no)
{
	// ボリューム上げていく
	if ((data[no]->volume += data[no]->fade_speed) >= 1.0f)
	{
		// フェードしきった！
		data[no]->volume = 1.0f;
		data[no]->fade_mode = MODE::NONE;
	}
	const int vol = (int)(MinVolume * (1.0f - data[no]->volume));
	data[no]->buffer->SetVolume(vol);
}
void fstSoundBGM::Out(int no)
{
	// ボリューム下げていく
	if ((data[no]->volume -= data[no]->fade_speed) <= 0)
	{
		// フェードしきった！
		data[no]->volume = 0;
		data[no]->fade_mode = MODE::NONE;
		data[no]->buffer->Stop();
	}
	const int vol = (int)(MinVolume * (1.0f - data[no]->volume));
	data[no]->buffer->SetVolume(vol);
}
//
//=============================================================================================

//=============================================================================================
//		waveファイルセット
void fstSoundBGM::Set(int ID, char* filename, bool b3D)
{
	//	初期化チェック
	assert(lpDS);
	//	既存のバッファの解放
	if (ID < (int)data.size())SAFE_DELETE(data[ID]->buffer);

	assert(filename);

	// 情報設定
	BGMData *set = new BGMData;
	set->b3D = b3D;
	set->buffer = new fstSoundBuffer(lpDS, filename, b3D);
	set->fade_mode = MODE::NONE;
	set->fade_speed = 0;
	set->volume = 1;
	data.push_back(set);
	assert(data[ID]->buffer->GetBuf());
}
//
//=============================================================================================

//=============================================================================================
//		再		生
void fstSoundBGM::Play(int ID, bool loop, DWORD cursor)
{
	//	初期化チェック
	assert(lpDS);
	//	データが無い！！
	assert(data[ID]->buffer);
	data[ID]->buffer->Play(loop, cursor);
}
void fstSoundBGM::Play(int ID, const Vector3 &pos, const Vector3 &front, const Vector3 &move, bool loop)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	data[ID]->buffer->SetAll3D(DS3D_DEFAULTMAXDISTANCE, DS3D_DEFAULTMINDISTANCE, pos, front, DS3D_DEFAULTCONEANGLE, DS3D_DEFAULTCONEOUTSIDEVOLUME, move);
	data[ID]->buffer->Play(loop);
}
//
//=============================================================================================


//=============================================================================================
//		停		止
void fstSoundBGM::Stop(int ID)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	data[ID]->buffer->Stop();
}
void fstSoundBGM::AllStop()
{
	assert(lpDS);
	for (UINT i = 0; i < data.size(); i++)if (data[i]->buffer->isPlay())data[i]->buffer->Stop();
}
void fstSoundBGM::Pause(int ID)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	data[ID]->buffer->Pause();
}
//
//=============================================================================================

//=============================================================================================
//		フェード関係
void fstSoundBGM::FadeIn(int ID, float fade_speed, bool loop, DWORD cursor)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	data[ID]->volume = 0;
	data[ID]->fade_mode = MODE::FADE_IN;
	data[ID]->fade_speed = fade_speed;
	data[ID]->buffer->SetVolume(DSBVOLUME_MIN);
	data[ID]->buffer->Play(loop, cursor);
}
void fstSoundBGM::FadeOut(int ID, float fade_speed)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	data[ID]->volume = 1.0f;
	data[ID]->fade_mode = MODE::FADE_OUT;
	data[ID]->fade_speed = fade_speed;
	data[ID]->buffer->SetVolume(DSBVOLUME_MAX);
}
void fstSoundBGM::CrossFade(int inID, int outID, float fade_speed, CROSS_FADE_TYPE type, bool loop)
{
	CrossFade(inID, outID, fade_speed, fade_speed, type, loop);
}
void fstSoundBGM::CrossFade(int inID, int outID, float in_speed, float out_speed, CROSS_FADE_TYPE type, bool loop)
{
	assert(lpDS);
	assert(data[inID]->buffer && data[outID]->buffer);

	// フェードアウト設定
	this->FadeOut(outID, out_speed);

	// フェードイン設定
	DWORD cursor;
	switch (type)
	{
	case CROSS_FADE_TYPE::NORMAL: cursor = 0;
		break;
	case CROSS_FADE_TYPE::END_OF_ETERNITY: cursor = data[outID]->buffer->GetPlayCursor();
		break;
	}
	this->FadeIn(inID, in_speed, loop, cursor);
}
//
//=============================================================================================

//=============================================================================================
//		ボリューム(-10000～0)
void fstSoundBGM::SetVolume(int ID, int volume)
{
	if (data[ID]->b3D)return;	// 3Dサウンドに処理を任せているのでこちら側で音はいじれない
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetVolume(volume);
}
void fstSoundBGM::SetVolume(int ID, float volume)
{
	SetVolume(ID, (int)(-5000 * volume));
}
int	fstSoundBGM::GetVolume(int ID)
{
	assert(lpDS);
	assert(data.size() != 0);
	return data[ID]->buffer->GetVolume();
}
//
//=============================================================================================

//=============================================================================================
//		ステレオ(-10000～0～10000)
void fstSoundBGM::SetPan(int ID, int pan)
{
	if (data[ID]->b3D)return;	// 3Dサウンドに処理を任せているのでこちら側で音はいじれない
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetPan(pan);
}
int	fstSoundBGM::GetPan(int ID)
{
	assert(lpDS);
	assert(data.size() != 0);
	return data[ID]->buffer->GetPan();
}
//
//=============================================================================================

//=============================================================================================
//		周波数
void fstSoundBGM::SetFrequency(int ID, int frequency)
{
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetFrequency(frequency);
}
int	fstSoundBGM::GetFrequency(int ID, int no)
{
	assert(lpDS);
	assert(data.size() != 0);
	return data[ID]->buffer->GetFrequency();
}
//
//=============================================================================================

//=============================================================================================
//		再生速度(周波数いじってるだけ)
void fstSoundBGM::SetSpeed(int ID, float speed)
{
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetSpeed(speed);
}

void fstSoundBGM::SetSpeed(int ID, int no, float speed)
{
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetSpeed(speed);
}
//
//=============================================================================================

//=============================================================================================
//		再生状態(再生中or停止中)
bool fstSoundBGM::isPlay(int ID)
{
	assert(lpDS);
	assert(data.size() != 0);
	return data[ID]->buffer->isPlay();
}
//
//=============================================================================================

//=============================================================================================
//		再生カーソル
DWORD fstSoundBGM::GetPlayCursor(int ID)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	return data[ID]->buffer->GetPlayCursor();
}
DWORD fstSoundBGM::GetPlayFrame(int ID)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	return data[ID]->buffer->GetPlayFrame();
}
int	fstSoundBGM::GetPlaySecond(int ID)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	return data[ID]->buffer->GetPlaySecond();
}

void fstSoundBGM::SetPlaySecond(int ID, int sec)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	data[ID]->buffer->SetPlaySecond(sec);
}


DWORD fstSoundBGM::GetSize(int ID)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	return data[ID]->buffer->GetSize();
}

int	fstSoundBGM::GetLengthSecond(int ID)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	return data[ID]->buffer->GetLengthSecond();
}
//
//=============================================================================================

//=============================================================================================
//		3Dサウンドでの音源情報設定
void fstSoundBGM::SetDist(int ID, int no, float max_dist, float min_dist)
{
	MyAssert(data[ID]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetDist(max_dist, min_dist);
}
void fstSoundBGM::SetPos(int ID, int no, const Vector3 &pos)
{
	MyAssert(data[ID]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetPos(pos);
}
void fstSoundBGM::SetFront(int ID, int no, const Vector3 &front)
{
	MyAssert(data[ID]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetFront(front);
}
void fstSoundBGM::SetRange(int ID, int no, int degreeIn)
{
	MyAssert(data[ID]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetRange(degreeIn);
}
void fstSoundBGM::SetOutRange_volume(int ID, int no, int out_vol)
{
	MyAssert(data[ID]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetOutRange_volume(out_vol);
}
void fstSoundBGM::SetMove(int ID, int no, const Vector3 &move)
{
	MyAssert(data[ID]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetMove(move);
}
void fstSoundBGM::SetAll3D(int ID, int no, float max_dist, float min_dist, const Vector3 &pos, const Vector3 &front, int degreeIn, int out_vol, const Vector3 &move)
{
	MyAssert(data[ID]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetAll3D(max_dist, min_dist, pos, front, degreeIn, out_vol, move);
}
//
//=============================================================================================

//=============================================================================================
//		エフェクトセット
void fstSoundBGM::SetFX(DXA_FX flag)
{
	for (UINT i = 0; i < data.size(); i++)data[i]->buffer->SetFX(flag);
}
void fstSoundBGM::SetFX(int ID, DXA_FX flag)
{
	data[ID]->buffer->SetFX(flag);
}
//
//=============================================================================================

//**************************************************************************************************************
//	ストリームサウンド管理
//**************************************************************************************************************
fstStreamSound* fstSoundBGM::PlayStream(char* filename)
{
	return PlayStream(filename, STR_NORMAL, 0);
}

fstStreamSound* fstSoundBGM::PlayStream(char* filename, BYTE mode, int param)
{
	fstStreamSound*	lpStream;

	//	初期化チェック
	if (lpDS == nullptr) return nullptr;

	lpStream = new fstStreamSound(lpDS, filename, mode, param);
	return lpStream;
}