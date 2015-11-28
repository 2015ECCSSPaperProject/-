#include	<assert.h>
#include	<time.h>

#include	"../../IEX/iextreme.h"

#include	"../Manager/GameManager.h"

#include	"system/system.h"
#include	"../system/Framework.h"

#include	"../system/Thread.h"

/* **************************************************************************************** */

#define	CPU_NAME			"�b�o�t"
#define	NAME_BUFFER_MAX		9

float Wrap(float val)
{
	if (val < 0.0f)		return val + 1.0f;
	if (val > 1.0f)		return val - 1.0f;
	return val;
}
float Clamp(float val, float minval, float maxVal)
{
	if (val < minval)	return minval;
	else if (val > maxVal)	return maxVal;
	else					return val;
}

//**********************************************************************************************
//
//	�Q�[���}�X�^�[
//
//**********************************************************************************************


//**********************************************************************************************
//	�R���X�g���N�^�E�f�X�g���N�^
//**********************************************************************************************
GameMasterServerA::GameMasterServerA()
: playerList(nullptr)
{
	hMutex = CreateMutex(NULL, FALSE, NULL);
	MainFrame = new Framework();

	//	�X���b�g(���X�|�[��)�ʒu�ݒ�
	//�@�ǂݍ��݂Ə�����:�X���b�g
	char*	filename = "DATA/Slot/Slot/Slot4.IMO";
	WORD	state = 0x0000;

	const float SLOT_SCALE = 8.0f;
	struct
	{
		Vector3 pos;
		float	angle;
		float	scale;
	} slotConfig[PANZER_MAX]
	{
		{ Vector3(500.0f, 0.0f, -800.0f), 0.0f, SLOT_SCALE },
		{ Vector3(170.0f, 0.0f, -800.0f), 0.0f, SLOT_SCALE },
		{ Vector3(-155.0f, 0.0f, -800.0f), 0.0f, SLOT_SCALE },
		{ Vector3(-480.0f, 0.0f, -800.0f), 0.0f, SLOT_SCALE },
		{ Vector3(480.0f, 0.0f, 800.0f), PI, SLOT_SCALE },
		{ Vector3(155.0f, 0.0f, 800.0f), PI, SLOT_SCALE },
		{ Vector3(-170.0f, 0.0f, 800.0f), PI, SLOT_SCALE },
		{ Vector3(-500.0f, 0.0f, 800.0f), PI, SLOT_SCALE },
	};

	for (size_t i = 0; i < SLOT_NUM; i++)
	{
		Matrix scale, rot, trans;
		D3DXMatrixScaling(&scale, slotConfig[i].scale, slotConfig[i].scale, slotConfig[i].scale);
		D3DXMatrixRotationY(&rot, slotConfig[i].angle);
		D3DXMatrixTranslation(&trans, slotConfig[i].pos.x, slotConfig[i].pos.y, slotConfig[i].pos.z);
		slotInfo[i].transMatrix = scale * rot * trans;
	}
}
GameMasterServerA::~GameMasterServerA()
{
	CloseHandle(hMutex);
	SAFE_ARRAY_DELETE(playerList);
}

//**********************************************************************************************
// ���C�����[�v����
//**********************************************************************************************
void	GameMasterServerA::GameRun()
{
	ServerActivate();		//	�ʐM�J�n
	MSG	msg;

	//	���C�����[�v
	for (;;){
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
			if (!GetMessage(&msg, NULL, 0, 0)) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (IsGameStart() == true)
		if (MainFrame->Update()) MainFrame->Render();
	}

	Sleep(100);

	ServerDisActivate();	//	�ʐM�I��

	DestroyRoom();			//	�����폜
	SystemExit();
}

//**********************************************************************************************
// �ʐM����
//**********************************************************************************************
void GameMasterServerA::ServerActivate()
{
	isRun = true;
	//	�X���b�h����
	m_pThread = new Thread(ThreadFunc, this);
	m_pThread->Run();
}
void GameMasterServerA::ThreadFunc(void* pData)
{
	GameMasterServerA* pMain((GameMasterServerA*)pData);
	ServerManager::Init();

	while (pMain->isRun)
		ServerManager::Update();

	ServerManager::Release();
}

//*****************************************************************************
//	�����O
//*****************************************************************************
bool GameMasterServerA::GameInitialize()
{
	//	�Q�[���v���C���͏��������Ȃ�
	if (isGameStart == true)	return true;
	WaitForSingleObject(hMutex, INFINITE);

	//	�v���C���[���ݒ�	
	CreateRoom();

	//	�X���b�g��񏉊���
	for (size_t i = 0; i < SLOT_NUM; i++)
	{
		slotInfo[i].actionInfo = 0x0000;
		slotInfo[i].reelUV[0] = 0.0f;
		slotInfo[i].reelUV[1] = 0.0f;
		slotInfo[i].reelUV[2] = 0.0f;
		slotInfo[i].reelNo[0] = 0;
		slotInfo[i].reelNo[1] = 0;
		slotInfo[i].reelNo[2] = 0;
	}
	for (size_t i = 0; i < PANZER_MAX; i++)
	{
		for (size_t j = 0; j < PANZER_MAX / 2; j++)
		{
			assistData[i].timer[j] = 0;
		}
	}

	ReleaseMutex(hMutex);

	//	�Q�[�����ݒ�
	FILE*	fp = nullptr;
	fopen_s(&fp, "./DATA/GAMETIME.txt", "r");
	assert(fp);
	time_t gameWidth = 0;
	fscanf_s(fp, "%d", &gameWidth);
	fclose(fp);

	gameState = WAIT;
	startTime = time(NULL);
	endTime = startTime + gameWidth;
	curTime = time(NULL);

	//	scene����
	MainFrame->ChangeScene(new sceneMutch());

	// �Q�[���O�����͂��̑O�ɏI��点�Ă���( �X���b�h������ )
	isGameStart = true;

	return true;
}
void GameMasterServerA::CreateRoom()
{
	

}

//*****************************************************************************
//	������
//*****************************************************************************
void GameMasterServerA::Update()
{
	
}

//*****************************************************************************
// ������
//*****************************************************************************

void GameMasterServerA::MutchExit()
{
	gameState = GAME_STATE_TYPE::GAME_RESULT;
	isGameStart = false;
}
void GameMasterServerA::SystemExit()
{
	SAFE_DELETE(m_pThread);
	SAFE_DELETE(MainFrame);
}


// �V���O���g��
GameMasterServerA*	GetGameInfo()
{
	static	GameMasterServerA	 instanse;
	return &instanse;
}
