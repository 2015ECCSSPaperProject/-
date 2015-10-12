
#include	<time.h>
#include	<tchar.h>

#include	"iextreme.h"
#include	"system/system.h"
#include	"system\Framework.h"

#include	"..\Object\TankScrapper.h"

#include	"../Manager/GameManager.h"
#include	"../Manager/MeshRegister.h"

#include	"InputManager.h"
#include	"SystemAdd/Drawing.h"

#include	"../Object/Camera.h"
#include	"Socket.h"

#include	"Stage.h"

#include	"Item.h"

#include	"Tank.h"

#include	"Missile.h"
#include	"Grenade.h"
#include	"HomingMissile.h"
#include	"Laser.h"
#include	"Shishamo.h"
#include	"Shield.h"
#include	"CapsuleWeapon.h"

#include	"../Manager/GameManager.h"

#include	"TankController.h"

#include	"Player.h"
#include	"NetPlayer.h"

#include	"Sky.h"


#include	"Effect.h"
#include	"Interface.h"

#include	"ShadingManager.h"

#include	"InformationBuffer.h"

#include	"SoundManager.h"

#include	"sceneMain.h"
#include	"sceneMutch.h"
#include	"sceneResult.h"

#include	"../Slot/Slot.h"
#include	"../Slot/Coin.h"
#include	"../Object/TankFactory.h"

#include	"../system/Thread.h"
#include	"../Object/ViewMotion.h"
#include	"appMath.h"

#include	"Event.h"
#include	"imageController.h"
#include	"sceneEnd.h"
#include 	"Gimmick.h"


//*****************************************************************************************************************************
//		������
//*****************************************************************************************************************************
bool	sceneMutch::Initialize()
{
	//	�`��}�l�[�W���[�������t���O���N���A
	m_InitShadingManager	=	false;

	//	��̐F�Z�b�g
	SkyColor = 0xFF8888FF;
	{
		iexLight::SetAmbient( 0x404040 );
		iexLight::SetFog( 800, 5000, SkyColor );

		Vector3 dir( 1.0f, -1.0f, -0.5f );
		dir.Normalize();
		iexLight::DirLight( shader, 0, &dir, 0.8f, 0.8f, 0.8f );

		iexParticle::Initialize( "DATA\\particle.png", 2000 );
	}

	{	
		//�������@����Ǝ����ŕ�����
		for (s32 i = 0; i <PANZER_MAX; ++i)
		{
			if( i == SOCKET_MANAGER->GetID())
				tank[i] = new Player();
			else
				tank[i] = new NetPlayer();
		}
		m_pSky = new iexMesh("data\\stage\\sky.x");
	}

	//	�A�C�e��������
	{	
		ItemManager::Init( 8, "data\\item\\ItemCoinX.x" );

		//	���X�|�[�����W�̃Z�b�g
		ItemManager::SetBornPos( Vector3(    0,  8,  0   ));
		ItemManager::SetBornPos( Vector3(  580,  8, -174 ));
		ItemManager::SetBornPos( Vector3( -242,  8, -6   ));
		ItemManager::SetBornPos( Vector3( -452, 91, -15  ));
		ItemManager::SetBornPos( Vector3(  106,  8, 292  ));

		ItemManager::SetBornPos( Vector3(  -40,  8, -460 ));
		ItemManager::SetBornPos( Vector3(  420,  8, 15   ));
		ItemManager::SetBornPos( Vector3( -500,  8, -455 ));
		ItemManager::SetBornPos( Vector3( -120,  8,  500 ));
		ItemManager::SetBornPos( Vector3(  530,  8,  530 ));

		ItemManager::SetBornPos( Vector3(  260,  8,  275 ));
		ItemManager::SetBornPos( Vector3( -550,  8,  430 ));
		ItemManager::SetBornPos( Vector3(  145,  8, -400 ));
		ItemManager::SetBornPos( Vector3(  240,  8, -40  ));
		ItemManager::SetBornPos( Vector3(  -80,  8,  215 ));
	}
	
	{
		//	���͏��N���A
		InputManager::Clear();

		//	�G�t�F�N�g������
		ImpactEffect::InitializeForGame();
		ImpactEffect2::InitializeForGame();
		FireEffect::InitializeForGame();
		MuzzleFlashEffect::InitializeForGame();
		ExplosionEffect::InitializeForGame();
		FloorRuddingEffect::InitializeForGame();
		DotEffect::InitializeForGame();
		UnionEffect::InitializeForGame();
		SparkEffect::InitializeForGame();
		LandingEffect::InitializeForGame();
		SmokeManager::InitializeForGame();
		MissileSmokeEffect::InitializeForGame();
		MissileExplosionEffect::InitializeForGame();
		LaserEffect::InitializeForGame();
		LaserHitEffect::InitializeForGame();
	
		//	�C���^�[�t�F�C�X������
		Alignment::InitializeForGame();
		DamageFilter::InitializeForGame();
		DamageGuide::InitializeForGame();
		EnemyHPGage::InitializeForGame();	
		GameLog::InitializeForGame();
		NamePlate::InitializeForGame();
		ChatLog::InitializeForGame();
		ChatBox::InitializeForGame();
		PlayerHPGageType2::InitializeForGame();
		MiniMap::InitializeForGame();
		ReloadGauge::Initialize();
		FrontGuide::InitializeForGame();
		PrizeView::InitializeForGame();
		BallisticGuide::Disactivate();
		LockOnCursor::InitializeForGame();
	
		//	�}�E�X�ւ̃A�N�Z�X�����擾

		InfoBuffer::SetPlayerList((const Player**)tank);
		Camera::Initialize();
		curlocalState		= DEATH;

		selectSlotTimer = 0;
		GameMasterClientA::SetSlotState( SELECT );
		
		slotButton = new SlotButton();
		motionState = new ViewerMotion(ViewerMotion::USAGE::WRITE);
		motionState->Load("DATA/Slot/bone.ykd");
		motionDirector = new ysSkeltonDirector( motionState->GetSkeltonSize() );

		//	�I���C�x���g�p�N���X�̐���
		m_end = new SceneEnd(this);
	}

	//	�O���l�[�h�Ǘ��N���X������
	GrenadeManager::InitializeForGame();
	//	�U���~�T�C���Ǘ��N���X������
	HomingMissileManager::InitializeForGame();
	//	���[�U�[�Ǘ��N���X������
	LaserManager::InitializeForGame();
	//	��������Ǘ��N���X������
	ShishamoManager::InitializeForGame();
	//	�V�[���h�Ǘ��N���X������
	ShieldManager::InitializeForGame();

	//�E�F�|���J�v�Z���Ǘ��N���X������
	PunchManager::Initialize();
	BurningManager::Initialize();

	//	�X�N���b�v������
	TankScrapper::Initialize();
	TankScrapper::SettingCenterGravity();

	//	�`��p�X�e�[�W�Ǘ��N���X������
	RenderStageManager::InitializeForGame();
	
	//
	GimmickManager::Init();	

	//
	m_pThread		= new EL::Thread( ThreadFunc, this );

	InputManager::SetMouseExclusion( true );

	//	�������Ƃ�
	SOCKET_MANAGER->InitGame();//������

	//	�l�b�g���[�N���J�n����
	m_pThread->Run();

#ifdef	USE_SOUND_EFFECT
	//	�a�f�l�Đ��J�n
	{
		bool	loop	=	true;
		float	pitch	=	1.0f;
		float	gain	=	0.1f;
		m_BGMSourseID	=	SoundManager::Play( "�퓬�a�f�l", loop, pitch, gain );
	}
#endif

	return	true;
}
void	sceneMutch::InitializeShadingManager( void )
{
	//	�`��}�l�[�W���[������
	ShadingManager::InitializeForGame();

	//	�`��p�����[�^�̏�����
	D3DXCOLOR	fogColor		=	{	0.0f,	0.0f,	0.0f,	0.0f	};
	D3DXCOLOR	skyColor		=	{	1.0f,	0.7f,	0.5f,	0.05f	};
	D3DXCOLOR	groundColor		=	{	0.62f,	0.48f,	0.4f,	0.01f	};
	D3DXCOLOR	dirLightColor	=	{	1.0f,	0.7f,	0.5f,	1.0f	};

	ShadingManager::SetFogColor( fogColor );
	ShadingManager::SetDistanceFog( 112233.0f, 223344.0f );
	ShadingManager::SetHeightFog( -10000.0f, -20000.0f );
	ShadingManager::SetExponentFog( 100000.0f, 200000.0f );

	ShadingManager::SetSkyDirection( 0.0f, -1.0f, 0.0f );
	ShadingManager::SetSkyColor( skyColor );
	ShadingManager::SetGroundColor( groundColor );

	ShadingManager::SetDirectionalLightDir( 0.5f, -1.0f, 0.5f );
	ShadingManager::SetDirectionalLightColor( dirLightColor );

	ShadingManager::SetRimLightColor( dirLightColor );
	ShadingManager::SetRimLightPower( 2.0f );
	ShadingManager::SetRimLightRate( 0.75f );

	ShadingManager::SetScreenColor( 1.0f, 1.0f, 1.0f );
	ShadingManager::SetContrast( 1.0f );
	ShadingManager::SetSaturation( 1.2f );
	ShadingManager::SetBrightness( 0.06f );

	//	��L�T�[�N��
	ShadingManager::SetCircleColor	( 0xFFffffff );
	ShadingManager::SetCirclePos	( 0.0f, 0.0f );	
	ShadingManager::SetCircleWidth	( 10.0f );	
	ShadingManager::SetCircleRadius	( 200.0f );
}

//*****************************************************************************************************************************
//		���
//*****************************************************************************************************************************
sceneMutch::~sceneMutch()
{
#ifdef	USE_SOUND_EFFECT
	//	�a�f�l��~
	SoundManager::Stop( m_BGMSourseID );
#endif

	//	�X���b�h�I��
	delete m_pThread;
	delete m_end;
	delete m_pSky;

	//	��ԉ��
	for ( s32 i=0; i<PANZER_MAX ; ++i )
		delete tank[i];

	//	�J�������
	Camera::Release();
	
	//	�`��}�l�[�W���[���
	ShadingManager::ReleaseForGame();
	
	//	�G�t�F�N�g���
	ImpactEffect::ReleaseForGame();
	ImpactEffect2::ReleaseForGame();
	FireEffect::ReleaseForGame();
	MuzzleFlashEffect::ReleaseForGame();
	ExplosionEffect::ReleaseForGame();
	FloorRuddingEffect::ReleaseForGame();
	DotEffect::ReleaseForGame();
	UnionEffect::ReleaseForGame();
	SparkEffect::ReleaseForGame();
	LandingEffect::ReleaseForGame();
	SmokeManager::ReleaseForGame();
	MissileSmokeEffect::ReleaseForGame();
	MissileExplosionEffect::ReleaseForGame();
	LaserEffect::ReleaseForGame();
	LaserHitEffect::ReleaseForGame();

	//	�C���^�[�t�F�C�X���
	Alignment::ReleaseForGame();
	DamageGuide::ReleaseForGame();
	EnemyHPGage::ReleaseForGame();
	GameLog::ReleaseForGame();
	NamePlate::ReleaseForGame();
	ChatBox::ReleaseForGame();
	ChatLog::ReleaseForGame();
	PlayerHPGageType2::ReleaseForGame();
	MiniMap::ReleaseForGame();
	ReloadGauge::Release();
	FrontGuide::ReleaseForGame();
	PrizeView::ReleaseForGame();
	LockOnCursor::ReleaseForGame();

	//
	iexParticle::Release();
	
	ItemManager::Release();

	//	�O���l�[�h�Ǘ��N���X���
	GrenadeManager::ReleaseForGame();
	//	�U���~�T�C���Ǘ��N���X���
	HomingMissileManager::ReleaseForGame();
	//	���[�U�[�Ǘ��N���X���
	LaserManager::ReleaseForGame();
	//	��������Ǘ��N���X���
	ShishamoManager::ReleaseForGame();
	//	�V�[���h�Ǘ��N���X�J��
	ShieldManager::ReleaseForGame();

	//�E�F�|���J�v�Z���Ǘ��N���X�J��
	PunchManager::Release();
	BurningManager::Release();

	//	�X�N���b�v�J����
	TankScrapper::Release();

	//	�`��p�X�e�[�W�Ǘ��N���X���
	RenderStageManager::ReleaseForGame();
	
	//	�M�~�b�N���
	GimmickManager::Release();
	//	���X�|�[���p�J����
	SAFE_DELETE( slotButton		);
	SAFE_DELETE( motionState	);		
	SAFE_DELETE( motionDirector );

}

//*****************************************************************************************************************************
//		�X�V
//*****************************************************************************************************************************
void	sceneMutch::Update( void ) 
{
	//	�`��}�l�[�W���[�̏�����
	if( m_InitShadingManager == false ){
		InitializeShadingManager();
		m_InitShadingManager	=	true;
	}

	//	�`��}�l�[�W���[�X�V
	ShadingManager::Update();

	//	������
	if( !m_end->m_isEndMutch )
	{
		//	�l�b�g���[�N�̏��𔽉f������
		GameMasterClientA::UpdateServerInfo();
		GAME_STATE_TYPE curType = GameMasterClientA::GetGameState();
		if( curType == GAME_STATE_TYPE::WAIT ) return;

		slotButton->Update();

		// �C�x���g�X�V
		//EventManager::Update();

		//	�X���b�g�X�V
		GameMasterClientA::UpdateSlots();

		//	��ԍX�V
		UpdateTanks();

		//	�Q�[�����X�V
		switch(curlocalState)
		{
		case COMBAT:	UpdateCombatTank();		break;
		case DEATH:		UpdateDeathTank();		break;
		case RESPAWN:	UpdateRespawnTank();	break;
		default:								break;
		}

		//	�O���l�[�h�p�̃K�C�h
		if( GameMasterClientA::GetMyTankState() != PLAYER_STATE::DEATH ){
			BallisticGuide::Set( SOCKET_MANAGER->GetID() );
		}
			
		//	�`��p�X�e�[�W�Ǘ��N���X�X�V
		RenderStageManager::Update( SOCKET_MANAGER->GetID() );

		//if( InputManager::GetKeyState( DIK_X ) )	ShieldManager::Activate( SOCKET_MANAGER->GetID() );
		//else										ShieldManager::Disactivate( SOCKET_MANAGER->GetID() );

		//ShieldManager::Activate( SOCKET_MANAGER->GetID() );

		//	�I�u�W�F�N�g�X�V
		MissileManager::Update();
		GrenadeManager::Update();
		HomingMissileManager::Update();
		LaserManager::Update();
		ShishamoManager::Update();
		ShieldManager::Update();

		PunchManager::Update();
		BurningManager::Update();


		TankScrapper::Update();
		m_pSky->Update();

		//	�G�t�F�N�g�X�V	
		ImpactEffect::Update();
		ImpactEffect2::Update();
		ExplosionEffect::Update();
		FloorRuddingEffect::Update();
		FireEffect::Update();
		MuzzleFlashEffect::Update();
		UnionEffect::Update();
		SparkEffect::Update();
		LandingEffect::Update();
		SmokeManager::Update();
		MissileSmokeEffect::Update();
		MissileExplosionEffect::Update();
		LaserEffect::Update();
		LaserHitEffect::Update();
		
		DotEffect::Update();

		ItemManager::Update();
		iexParticle::Update();

		//	�C���^�[�t�F�C�X�X�V
		Alignment::Update();
		DamageGuide::Update();
		PlayerHPGageType2::Update();
		DamageFilter::Update( SOCKET_MANAGER->GetID() );
		GameLog::Update();
		NamePlate::Update();
		ChatBox::Update( SOCKET_MANAGER->GetID() );
		ChatLog::Update();
		MiniMap::Update();
		ReloadGauge::Update();
		FrontGuide::Update( SOCKET_MANAGER->GetID() );
		PrizeView::Update();
		LockOnCursor::Update();
		
		PlayerEffectCoin::Update();
		PlayerGameInfoCard::Update();

		//	�M�~�b�N�̍X�V
		GimmickManager::Update();

		//	�I���m�F
		if( m_end->CheckEnd())
			SoundManager::Stop( m_BGMSourseID );
	}
	else
	{
		m_end->Update();
	}
}


//*****************************************************************************************************************************
//		�`��
//*****************************************************************************************************************************
void	sceneMutch::Render( void )
{
	//	�`��}�l�[�W���[������������Ă��Ȃ���΃X�L�b�v
	if( m_InitShadingManager == false )	return;

#ifdef _DEBUG
#if 1
	if( KEY_Get( KEY_B ) && KEY_Get( KEY_C ) )
	{
		GameMasterClientA::dbgInfo();
		return;
	}
#endif
#endif // _DEBUG
	if( !m_end->m_isEndMutch )
	{
		//	�`��p�e�N�X�`�����N���A
		ShadingManager::ClearTexture( SkyColor );

		//	�X�e�[�g
		
		switch( curlocalState ){
		case	RESPAWN:	
		case	COMBAT:		SetUpCombatCamera();	break;
		case	DEATH:		SetUpDethTankCamera();	break;
		default:			break;
		}

#if		!defined( ANTI_ALIASING_DEFAULT )
		//	�����_�[�^�[�Q�b�g��ύX
		ShadingManager::SetRenderTarget();
#endif

		m_pSky->Render();


		//	�~�T�C���`��
		MissileManager::Render();
		GrenadeManager::Render();
		HomingMissileManager::Render();
		LaserManager::Render();
		ShishamoManager::Render();

		iexSystem::Device->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

		//	�C�x���g�I�u�W�F�N�g�`��
		//EventManager::Render3D();

		//	�X�e�[�W�`��i���ƕǁj
		RenderStageManager::RenderBase();

		//	�X�e�[�W�`��i���ƕǈȊO�j
		RenderStageManager::RenderObstacle();

		//	�X���b�g�`��
		GameMasterClientA::RenderSlots();

		//������	�^���N�̕`��i�����ȊO�j
		for( int i = 0; i < PANZER_MAX; i++ ){
			if( i == SOCKET_MANAGER->GetID() )	continue;

			//	�����Ă��Ԃ�����`�悷��
//			float	hp		=	tank[i]->GetHP();
//			float	damage	=	tank[i]->GetDamage();
//			if( damage < hp )	tank[i]->Render();
			if( tank[i]->IsAlive() == true )
				tank[i]->Render();
		}

		//������	�^���N�̕`��i���������j
		if( GameMasterClientA::GetMyTankState() != PLAYER_STATE::DEATH ){
			tank[ SOCKET_MANAGER->GetID() ]->Render();
		}

		// �A�C�e���R�C���`��
		ItemManager::Render3D();

	 	//�j�Е`��
		TankScrapper::Render();
		
		//	�M�~�b�N�`��
		GimmickManager::Render();

		//	�����ň�U�A�\�񂳂ꂽ�`�揈�������s
		ShadingManager::Render();

		//	�G�t�F�N�g�`��
		SmokeManager::Render();
		MissileSmokeEffect::Render();
		ExplosionEffect::Render();
		MissileExplosionEffect::Render();
		UnionEffect::Render();

		//	�V�[���h�`��
		ShieldManager::Render();

		//�o�[�j���O�`��
		BurningManager::Render();



		//	����̃p�����[�^��`��
		EnemyHPGage::Render();
		NamePlate::Render();
		PrizeView::Render();

		iexParticle::Render();

		//	���߂��Ă����Q����`��
		RenderStageManager::RenderAlphaObstacle();
		ShadingManager::Render();

		//	�O���K�C�h��`��
		if( GameMasterClientA::GetMyTankState() != PLAYER_STATE::DEATH ){
			FrontGuide::Render();
		}

		//	���Z�G�t�F�N�g
		ImpactEffect::Render();
		ImpactEffect2::Render();
		MuzzleFlashEffect::Render();
		SparkEffect::Render();
		LaserEffect::Render();
		LaserHitEffect::Render();
		DotEffect::Render();


#if		!defined( ANTI_ALIASING_DEFAULT )
		//	�����_�[�^�[�Q�b�g��`��
		ShadingManager::RenderRenderTarget();
#endif

		// �A�C�e���X���b�g�`��
		ItemManager::Render2D();

		// �C�x���g�`��(2D)
		//EventManager::Render2D();

	
		//	�X���b�g�{�^��
		slotButton->Render();

		Render2D();
		// ���E�ɂ��閼�O�A�R�C�����\��
		PlayerGameInfoCard::RenderPlayerCard();
		if( GameMasterClientA::GetMyTankState() == PLAYER_STATE::COMBAT )
			PlayerEffectCoin::Render();
		GameMasterClientA::RenderCenterCardImage();
		GameMasterClientA::RenderGameInfo();

		if( curlocalState != PLAYER_STATE::DEATH ){
			//	�_���[�W�K�C�h�`��
			DamageGuide::Render();
	
			//	���b�N�I���J�[�\���`��
			LockOnCursor::Render();

			//	�Ə��`��
			if( tank[ SOCKET_MANAGER->GetID() ]->GetBulletType() !=	 BULLET_TYPE_GRENADE ){
				Alignment::RenderProcForPlayer( SOCKET_MANAGER->GetID() );
			}

			//	�~�j�}�b�v�`��
			MiniMap::Render();

			//	�g�o�Q�[�W�`��
			PlayerHPGageType2::Render();
			ReloadGauge::Render( SOCKET_MANAGER->GetID() );
		}

		//	�Q�[�����O�`��
		GameLog::Render();

		//	�`���b�g�t�h�`��
		ChatBox::Render();
		ChatLog::Render();

		if( GameMasterClientA::GetMyTankState() != PLAYER_STATE::DEATH ){
			//	�_���[�W�t�B���^�[�`��
			DamageFilter::Render();
		}

		if( curlocalState != PLAYER_STATE::DEATH ){
			//�T�C�g�\��
			static iex2DObj* site = new iex2DObj("DATA\\Interface\\site.png");
			site->Render(2, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight);
		}
		
		//	���̉��o�`��
		m_union.Render();

		//	�I�������\���e�X�g
		m_end->RenderEndFrop();
	}
	else
	{	
		m_end->Render();
	}
	
	//IEX_DrawText( g_DebugStr, 100, 100, 500, 500, 0xFFff0000 );
}


//==============================================================================
//	thread
//==============================================================================
void	sceneMutch::ThreadFunc( void* pData,bool* isEnd )
{
	sceneMutch* pMain((sceneMutch*)pData);
	SOCKET_MANAGER->InitUser();

	/*�t���[�����[�N*/
	DWORD dwFrameTime( clock() );
	while(true)
	{
		//	�X���b�h�I��
		if( *isEnd )
			return;

		//	�t���[������	
		DWORD CurrentTime = clock()*10;
		if( CurrentTime < dwFrameTime+167 ) 
			continue;

		//	�o�ߎ���
		DWORD	dTime = CurrentTime - dwFrameTime;
		if( dTime > 2000 ) dwFrameTime = CurrentTime;
		dwFrameTime += 167;

		//	�l�b�g���[�N�X�V
		SOCKET_MANAGER->InitUser();
		SOCKET_MANAGER->UpdateUser();
		//SOCKET_MANAGER->UpdateEvent();

		//	�A�C�e���X�V
		SOCKET_MANAGER->ReceiveGetItemInfo();	//	�擾���̃C�x���g�p
		SOCKET_MANAGER->ReceiveItem();			//	���ʔ��f�p

		//	�`���b�g�X�V
		std::string str = ChatBox::GetLog();

		if( str.size() > 0 ){
			SOCKET_MANAGER->SendChat(str);
		}

		SOCKET_MANAGER->ReceiveChat();

		//	���O�X�V
		SOCKET_MANAGER->ReceiveLog();
	}
}


//==============================================================================
//	methods
//==============================================================================
void	sceneMutch::UpdateServerInfo()
{
//	GameMasterClientA::UpdateServerInfo();
//	��L�Ɋ֐��̒��g���ړ�
}
void	sceneMutch::UpdateTanks()
{
	//�@�����̕`��Ƒ���̕`��𕪂���
	if( curlocalState != DEATH )
		tank[ GameMasterClientA::GetMyID() ]->Update();//����

	for( int i = 0; i < PANZER_MAX; i++ )
	{	
		if( i == GameMasterClientA::GetMyID() ) continue;
		tank[ i ]->Update();//����
	}
}
 
//*****************************************************************************************************************************
//	�X�e�[�g�}�V�� : �Q�[�����C��
//*****************************************************************************************************************************
void	sceneMutch::UpdateCombatTank()
{
	//	�J�����X�V
	Camera::Update( InfoBuffer::GetPlayerList()[SOCKET_MANAGER->GetID()]->GetPos() );
	
	//	�m�����̉�ʐԉ�
	if( GameMasterClientA::GetMyTankState() == PLAYER_STATE::COMBAT ){
		static	const	float	c_WarningLine	=	0.4f;
		static	const	float	c_MaxInterval	=	60;
		static	const	float	c_MinInterval	=	30;

		int		myID		=	SOCKET_MANAGER->GetID();
		float	hp			=	InfoBuffer::GetPlayerList()[ myID ]->GetHP();
		float	damage		=	InfoBuffer::GetPlayerList()[ myID ]->GetDamage();
		float	percentage	=	( hp - damage ) / hp;

		if( percentage > 1.0f )	percentage	=	1.0f;
		if( percentage < 0.0f )	percentage	=	0.0f;

		if( percentage < c_WarningLine ){
			static	int		s_Counter	=	0;

			//	�����v�Z
			int		interval	=	( int )( c_MaxInterval + ( c_MinInterval - c_MaxInterval ) * ( 1.0f - percentage / c_WarningLine ) + 0.5f );

			//	�J�E���^�[�X�V
			++s_Counter;

#ifdef	USE_SOUND_EFFECT
			//	�x�����Đ�
			if( s_Counter >= interval ){
				//SoundManager::Play( "�x����", false, 1.0f, 0.2f );
			}
#endif

			//	�t�B���^�[�Z�b�g���J�E���^�[���Z�b�g
			if( s_Counter >= interval ){
				DamageFilter::Set( interval, 0.5f );
				DamageFilter::Set( interval * 2, 0.25f );
				s_Counter	=	0;
			}
		}
	}

	//	�X�e�[�g���X�V�i�L�[���͖��͎��Ԍo�߂Ń��X�|�[���ցj
	{
		static	const	int		c_WaitTime		=	120;
		static			int		s_WaitCounter	=	0;

		if( GameMasterClientA::GetMyTankState() != PLAYER_STATE::COMBAT ){
			//	�ҋ@����
			if( ++s_WaitCounter > c_WaitTime
			||	InputManager::GetKeyState( DIK_SPACE ) == KEY_STATE::KEY_STATE_PRESSED ){
				//	�V�[���̏�Ԃ��X�V
				curlocalState 	=	GameMasterClientA::GetMyTankState();

				//	�J�E���^�[�����Z�b�g
				s_WaitCounter	=	0;
			}

			//	�Ə�������
			Alignment::Disactivate();
			BallisticGuide::Disactivate();

			FrontGuide::Clear();
		}
	}

	//	�p�����[�^���N���A�i���S�����Ɣ��f�j
	if( curlocalState != COMBAT ){
		//	��Ԃ̃p�����[�^���N���A
		tank[ SOCKET_MANAGER->GetID() ]->Clear();

		//	�C���^�[�t�F�C�X�𖳌���
		ReloadGauge::Clear();
		MiniMap::Clear();
		PlayerHPGageType2::Clear();
	}
}
void	sceneMutch::SetUpCombatCamera()
{
	//	��ʂ��N���A
	Camera::Clear( SkyColor );
	//	�`��p�����[�^��ݒ�
	{
		ShadingManager::SetEyePosition( Camera::GetPos() );
		ShadingManager::SetEyeGaze( Camera::GetGaze() );
	}

#ifdef	USE_SOUND_EFFECT
	//	���X�i�[�̏����X�V
	{
		Vector3	pos			=	Camera::GetPos();
		Vector3	velocity	=	Vector3( 0.0f, 0.0f, 0.0f );
		Vector3	vFront		=	Camera::GetGaze() - Camera::GetPos();
				vFront.Normalize();
		Vector3	vUp			=	Vector3( 0.0f, 1.0f, 0.0f );
		SoundManager::UpdateListener( pos, velocity, vFront, vUp );
	}
#endif
}

//*****************************************************************************************************************************
//	�X�e�[�g�}�V�� : �X���b�g����
//*****************************************************************************************************************************
BYTE	sceneMutch::SelectSlot()
{	
	bool	isClick					=	InputManager::GetMouseButton(0) == KEY_STATE_PRESSED
									||	GetPadKeyDecision();
	if( isClick == false )	return 0xFF;
		
	POINT			  cursorPos		= InputManager::GetMouseCursorCoord();
	Global::TEAM_NAME myTeam		= GameMasterClientA::GetMyTeam();

	//�@�}�E�X�����R�c���W�ɕϊ�����s��쐬
	Matrix	invViewProjTrans;
	Matrix	viewport;
	D3DXMatrixIdentity(&viewport);
	float halfWidth	 = (float)iexSystem::ScreenWidth  * 0.5f;
	float halfHeight = (float)iexSystem::ScreenHeight * 0.5f;
	viewport._11 =  halfWidth; 
	viewport._22 = -halfHeight;
	viewport._41 =  halfWidth;
	viewport._42 =  halfHeight;

	Matrix	viewProjViewport = makeCamera.MatView() * makeCamera.MatProj() * viewport;
	D3DXMatrixInverse( &invViewProjTrans, NULL, &viewProjViewport );

	// �}�E�X�̂R�c���W����ray��������
	Vector3 lookAt;
	D3DXVECTOR3	mousePosNear, mousePosFar;
	Vector3		camPos = makeCamera.GetPos();
	D3DXVec3TransformCoord( &mousePosNear, &D3DXVECTOR3((float)cursorPos.x,(float)cursorPos.y, 0.0f), &invViewProjTrans );
	D3DXVec3TransformCoord( &mousePosFar,  &D3DXVECTOR3((float)cursorPos.x,(float)cursorPos.y, 1.0f), &invViewProjTrans );

	lookAt.x = mousePosFar.x - mousePosNear.x;
	lookAt.y = mousePosFar.y - mousePosNear.y;
	lookAt.z = mousePosFar.z - mousePosNear.z;
	lookAt.Normalize();

	// �X���b�g�Ƃ̌�������
	size_t loopStart = (myTeam == Global::TEAM_A ) ? 0 : 4;
	size_t loopEnd   = (myTeam == Global::TEAM_A ) ? 4 : 8;
	for(size_t i = loopStart; i < loopEnd; i++)
	{		
		SlotClient*	slot = GameMasterClientA::GetSlot(i);
		WORD curAction	 = slot->GetActionInfo();
		if(( curAction & 0xF000 ) > 0 )
			continue;
		float Dist = FLT_MAX;
		int res = GameMasterClientA::GetSlot( i )->RayPickAtWorld( &Vector3(), &camPos, &lookAt, &Dist );
		if( res != -1 ){

#ifdef	USE_SOUND_EFFECT
			//	���ʉ��Đ�
			SoundManager::Play( "���艹", false, 1.0f, 0.075f );
#endif

			return i;
		}
	}

	return  0xFF;
}
void	sceneMutch::UpdateDeathTank()
{
	//	�p�b�h�ɂ��}�E�X�J�[�\���̑���
	UpdateCursorCoordAtPadAxis( 0.2f, 10.0f );

	// �����܂ł̃X�e�b�v����
	switch( GameMasterClientA::GetMySlotState() )
	{
	case INIT:
	{
		GameMasterClientA::SetSendSelectSlot( 0xFF );
		GameMasterClientA::SendReqestClear();
		GameMasterClientA::SetSlotState( SLOT_STATE::SELECT );	
	}
	break;
	case SELECT:
	{
		BYTE	select	=	SelectSlot();		
		int		timerMax = 60 * 3;
		if( select == 0xFF && ++selectSlotTimer > timerMax )
		{
			u32 loopStart = ( GameMasterClientA::GetMyTeam() == Global::TEAM_A ) ? 0 : 4;
			u32 loopEnd = ( GameMasterClientA::GetMyTeam() == Global::TEAM_A ) ? 4 : 8;
			for(size_t i = loopStart; i < loopEnd; i++)
			{
				SlotClient*	slot = GameMasterClientA::GetSlot(i);
				WORD curAction	 = slot->GetActionInfo();
				if(( curAction & 0xF000 ) > 0 )	continue;

//				select = i;
				break;
			}

			selectSlotTimer = 0;
		}

		// �v���Ƃ��ăT�[�o�[�ɑ���
		GameMasterClientA::SetSendSelectSlot( select );
		if( select != 0xFF )
		{
			GameMasterClientA::SetSlotState( SLOT_STATE::SERVER_ENQUIRY );	
			selectSlotTimer = 0;			
		}
		else
		{
			// �I�����Ă��Ȃ����Ƃ��T�[�o�[�ɑ���
			GameMasterClientA::SetSendSelectSlot( 0xFF );
		}
	}
	break;
 	case SERVER_ENQUIRY:// �T�[�o�[�ɖ⍇����...
	{		
	}
	break;
	case PLAY:
	{
		// �{�^�����͊J�n			
		BYTE playSlot		= GameMasterClientA::GetSelectSlot();				// �g�p���X���b�g�擾
		BYTE curReqwst		= GameMasterClientA::GetReqest();					// ���݂̏�Ԏ擾
		PLAYER_STATE state	= GameMasterClientA::GetMyTankState();
		slotButton->Activate(playSlot);
		GameMasterClientA::GetSlot(playSlot)->Activate();
		if( state == RESPAWN )
		{
			slotButton->DeActivate();
			GameMasterClientA::GetSlot(playSlot)->DeActivate();			
			GameMasterClientA::SendReqestClear();								// �v�����N���A
			GameMasterClientA::SetSlotState( UNION );							// ���̃X�e�b�v��
		}
	}
	break;
	case UNION:
	{
		// �{�^�����͏I��
		slotButton->DeActivate();

		BYTE	select	= GameMasterClientA::GetSelectSlot();
		Vector3	slotPos = GameMasterClientA::GetSlot(select)->GetPos();
		if( UnionTank(slotPos) == true )
		{
			//	��ԕ�������
			curlocalState		= GameMasterClientA::GetMyTankState();
			GameMasterClientA::SendReqestClear();
			GameMasterClientA::SetSlotState( SELECT );
			float angle = 
				GameMasterClientA::GetSlot( select )->GetAngle().y;
			Global::TEAM_NAME	myTeam = GameMasterClientA::GetMyTeam();
			Camera::SetAngleHorizontal(	( myTeam == Global::TEAM_A ) ? PI : 0.0f );
			Camera::SetAngleVertical( 0.0f );
		}
	}
	break;
	default:
		break;
	}

}
void	sceneMutch::SetUpDethTankCamera()
{

	switch(GameMasterClientA::GetMySlotState())
	{
	case SELECT:
	{
		Vector3	selectCamPos[2];
		selectCamPos[0]		= Vector3(0.0f, 180.0f, 500.0f - 600.0f);
		selectCamPos[1]		= Vector3(0.0f, 180.0f,   0.0f + 00.0f);
		
		Vector3	selectCamLookAt[2];
		selectCamLookAt[0]	= Vector3(0.0f, 120.0f,    0.0f - 600.0f);
		selectCamLookAt[1]	= Vector3(0.0f, 120.0f, 500.0f + 00.0f);
		float selectCamFov = PI / 4.0f + 0.25f;
		Global::TEAM_NAME team = GameMasterClientA::GetMyTeam();
		makeCamera.Set( selectCamPos[team], selectCamLookAt[team]);
		makeCamera.SetFov( selectCamFov );
	}
	break;
	case PLAY:
	{
		size_t	skeltonSize = motionState->GetSkeltonSize();
		for(size_t i = 0; i < skeltonSize; i++)
			motionState->UpdateMotion( 0.0f, i );
		//	�J�����Z�b�g
		ysJointNode*	rootNode = motionState->GetSkeltons()->GetRootNode();
	
		motionDirector->Run( rootNode );
	
		Vector3	boneLocate0 = motionDirector->GetWorldSkelton( 0 ).locate;
		Vector3	boneLocate1 = motionDirector->GetWorldSkelton( 1 ).locate;


		BYTE	selectSlot	= GameMasterClientA::GetSelectSlot();

		if( selectSlot != 0xFF )
		{
			Matrix	slotMat		= GameMasterClientA::GetSlot( selectSlot )->TransMatrix;
			Transform( boneLocate0, boneLocate0, slotMat );
			Transform( boneLocate1, boneLocate1, slotMat );

			float selectCamFov = PI / 4.0f;
			float	fov		= makeCamera.GetFov();
			Vector3 pos		= makeCamera.GetPos();
			Vector3 target	= makeCamera.GetTarget();

			fov		= fov		+ ( selectCamFov - makeCamera.GetFov() ) * 0.02f;
			pos		= pos		+ ( boneLocate0 - pos	 ) * 0.05f;
			target	= target	+ ( boneLocate1 - target ) * 0.05f;

			makeCamera.SetFov( fov );
			makeCamera.Set( pos, target );
		}
	}


	break;
	default:
		break;
	}




	makeCamera.Activate();
	makeCamera.Clear( SkyColor );

	//	�`��p�����[�^��ݒ�
	{
		ShadingManager::SetEyePosition( makeCamera.GetPos() );
		ShadingManager::SetEyeGaze( makeCamera.GetTarget() );
	}

#ifdef	USE_SOUND_EFFECT
	//	���X�i�[�̏����X�V
	{
		Vector3	pos			=	makeCamera.GetPos();
		Vector3	velocity	=	Vector3( 0.0f, 0.0f, 0.0f );
		Vector3	vFront		=	Vector3( makeCamera.GetTarget() ) - makeCamera.GetPos();
				vFront.Normalize();
		Vector3	vUp			=	Vector3( 0.0f, 1.0f, 0.0f );
		SoundManager::UpdateListener( pos, velocity, vFront, vUp );
	}
#endif

}

//*****************************************************************************************************************************
//	�X�e�[�g�}�V�� : ����( ���G���Ԓ� 
//*****************************************************************************************************************************
void	sceneMutch::UpdateRespawnTank()
{
	//	�J�����X�V
	Camera::Update( InfoBuffer::GetPlayerList()[SOCKET_MANAGER->GetID()]->GetPos() );
	//	�X�e�[�g���X�V
	curlocalState = GameMasterClientA::GetMyTankState();
}
//	
void	sceneMutch::Render2D()
{
	//	����ɂg�o�o�[��\���i�`��\��̂݁j
	{
		//	�`��\��
		for( int i = 0; i < PANZER_MAX; i++ ){
			if( SOCKET_MANAGER->GetID() == i )	continue;
			if( tank[i]->IsAlive() == false )	continue;

			Global::TEAM_NAME	myTeam		=	GameMasterClientA::GetMyTeam();
			Global::TEAM_NAME	thisTeam	=	GameMasterClientA::GetPlayerTeam( i );

			Vector3	drawPos		=	tank[i]->GetPos();
			int		hp			=	(int)( tank[i]->GetHP()	    + 0.5f );
			int		damage		=	(int)( tank[i]->GetDamage() + 0.5f );
			D3DXCOLOR	gageColor = ( thisTeam == myTeam ) ? 0xFF20dd20 : 0xFFff2020;

			EnemyHPGage::ReserveRender( gageColor, drawPos, hp, damage );
		}
	}


	//	����ɖ��O��\���i�`��\��̂݁j
	{
		//	�����̃`�[��ID���擾
		Global::TEAM_NAME	myTeam		=	GameMasterClientA::GetPlayerTeam( SOCKET_MANAGER->GetID() );

		//	�`�[���h�c�擾
		Global::TEAM_NAME	teamID[]	=	{
			GameMasterClientA::GetPlayerTeam( 0 ),
			GameMasterClientA::GetPlayerTeam( 1 ),
			GameMasterClientA::GetPlayerTeam( 2 ),
			GameMasterClientA::GetPlayerTeam( 3 ),
			GameMasterClientA::GetPlayerTeam( 4 ),
			GameMasterClientA::GetPlayerTeam( 5 ),
			GameMasterClientA::GetPlayerTeam( 6 ),
			GameMasterClientA::GetPlayerTeam( 7 ),
		};
		//	�`��p�����[�^
		struct{
			LPCSTR	name;
			DWORD	fillColor;
			DWORD	edgeColor;
		}	c_Param[]	=	{
			{	GameMasterClientA::GetPlayerName( 0 ),	( teamID[0] != myTeam )?	0xFFff5050	:	0xFF5050ff,	0xAF000000	},
			{	GameMasterClientA::GetPlayerName( 1 ),	( teamID[1] != myTeam )?	0xFFff5050	:	0xFF5050ff,	0xAF000000	},
			{	GameMasterClientA::GetPlayerName( 2 ),	( teamID[2] != myTeam )?	0xFFff5050	:	0xFF5050ff,	0xAF000000	},
			{	GameMasterClientA::GetPlayerName( 3 ),	( teamID[3] != myTeam )?	0xFFff5050	:	0xFF5050ff,	0xAF000000	},
			{	GameMasterClientA::GetPlayerName( 4 ),	( teamID[4] != myTeam )?	0xFFff5050	:	0xFF5050ff,	0xAF000000	},
			{	GameMasterClientA::GetPlayerName( 5 ),	( teamID[5] != myTeam )?	0xFFff5050	:	0xFF5050ff,	0xAF000000	},
			{	GameMasterClientA::GetPlayerName( 6 ),	( teamID[6] != myTeam )?	0xFFff5050	:	0xFF5050ff,	0xAF000000	},
			{	GameMasterClientA::GetPlayerName( 7 ),	( teamID[7] != myTeam )?	0xFFff5050	:	0xFF5050ff,	0xAF000000	},
		};

		//	�`��
		for( int i = 0; i < PANZER_MAX; i++ ){
			if( i == SOCKET_MANAGER->GetID() )	continue;
			if( tank[i]->IsAlive() == false )	continue;
			LPCSTR	drawStr		=	c_Param[i].name;
			Vector3	drawPos		=	tank[i]->GetPos();
					drawPos.y	+=	30.0f;

			NamePlate::RenderReserve( drawPos, drawStr, c_Param[i].fillColor, c_Param[i].edgeColor );
		}
	}

	//	����ɏ܋��z�\���i�`��\��̂݁j
	{
		Global::TEAM_NAME	myTeam	=	GameMasterClientA::GetMyTeam();
		int					myID	=	SOCKET_MANAGER->GetID();
		for( int i = 0; i < PANZER_MAX; i++ ){
			if( GameMasterClientA::GetPlayerTeam( i ) == myTeam )	continue;
			if( tank[i]->IsAlive() == false )	continue;

			Vector3	pos		=	tank[i]->GetPos() + Vector3( 0.0f, 30.0f, 0.0f );
			int		prize	=	2 + SOCKET_MANAGER->GetPanzer( i ).prize;
					prize	+=	SOCKET_MANAGER->GetPanzer( i ).numBet + SOCKET_MANAGER->GetPanzer( myID ).numBet;

			if( tank[ myID ]->GetItemEffect().effect == ITEM_EFFECT::ITEM_EFFECT_GET_COIN_UP ){
				prize		+=	tank[ myID ]->GetItemEffect().level;
			}

			prize	+=	( int )( GameMasterClientA::GetPlayerCoin( i ) / 4.0f + 0.5f );

			PrizeView::RenderReserve( pos, prize );
		}
	}

	
	if( curlocalState == DEATH )
	{
		POINT	cursopos = InputManager::GetMouseCursorCoord();
			 if( cursopos.x < 0		)	cursopos.x = 0;
		else if( cursopos.x > 1280	)	cursopos.x = 1280;
			 if( cursopos.y < 0		)	cursopos.y = 0;
		else if( cursopos.y > 720	)	cursopos.y = 720;
		
		Alignment::Render( (float)cursopos.x, (float)cursopos.y );
	}
}


//---------------------------------------------------------------------
//  �^���N�쐬���o�֐�
//---------------------------------------------------------------------
bool	sceneMutch::UnionTank( Vector3 slotPos )
{
	const float moveValue	= 50;
	const float height		= 20;
	const float dist		= 60;
	const int   id			= GameMasterClientA::GetMyID();

	int team = GameMasterClientA::GetMyTeam() == Global::TEAM_NAME::TEAM_A? 1:0;
	int ref = 0;
	if( team )	ref = 1;
	else		ref = -1;

	struct 
	{
		float start;
		float end;
	}effect[]=
	{
		{ PI*1.5f,	PI*2.5f },
		{ PI/2,		PI*1.5f },
	};

	struct 
	{
		D3DXQUATERNION	orien;
		float			partsAngle;;
	}angles[]=
	{
		{ D3DXQUATERNION(0,1,0,-4.3711e-008f),	PI },
		{ D3DXQUATERNION(0,0,0,1			),	0  },
	};


	switch ( m_union.m_state )
	{
	case Union::INIT_CAMERA:
		m_union.cpos			=	makeCamera.GetPos();
		m_union.ctarget			=	makeCamera.GetTarget();
		m_union.t				=	0;
		m_union.value			=	0;
		m_union.nowParts		=	0;
		m_union.m_state			=	Union::UPDATE_CAMERA;
		break;


	case Union::UPDATE_CAMERA:
	{
		m_union.t += 0.01f;
		if( m_union.t >= 1.0f )
			m_union.m_state	= Union::INIT_TANK;


		Vector3 lerpPos		( slotPos.x, slotPos.y+height, slotPos.z + ( dist * ref ) );
		Vector3 lerpPTarget	( slotPos.x, slotPos.y+height, slotPos.z + ( dist * ref ) - ( 100.0f * ref ));

		makeCamera.SetPos	( appMathAPI::Vec3Lerp( m_union.cpos,		lerpPos,		m_union.t ));
		makeCamera.SetTarget( appMathAPI::Vec3Lerp( m_union.ctarget,	lerpPTarget,	m_union.t ));
	} break;
	

	case Union::INIT_TANK:
	{
		slotPos.y += moveValue;
		tank[ id ]->SetOrien		( angles[team].orien );
		tank[ id ]->SetPos			( slotPos );
		tank[ id ]->SetHeadAngle	( 0 );
		tank[ id ]->SetCannonAngle	( 0 );
		tank[ id ]->Tank::Update	();
		m_union.m_state = Union::UPDATE_TANK;
	} break;

	case Union::UPDATE_TANK:
	{
		const float downSpeed	(1.0f);
		const float psnzerHeight(8.0f);
		m_union.value += downSpeed;

		//	������
		if( m_union.value < moveValue - psnzerHeight )
		{		
			tank[ id ]->DownfallParts( m_union.nowParts, downSpeed );
		}
		else
		{
			//	�G�t�F�N�g���Z�b�g
			{
				int		bodyNum		=	tank[ id ]->GetBody().partsNum;
				bool	isFixed		=	PartsStorage::GetBodyData( bodyNum ).isFixed;
				if( m_union.nowParts == 0 ){
					LandingEffect::Set( Vector3(
						tank[ id ]->GetLeg().obj->TransMatrix._41,
						tank[ id ]->GetLeg().obj->TransMatrix._42,
						tank[ id ]->GetLeg().obj->TransMatrix._43 ), 1.0f );
				}
				if( m_union.nowParts == 1 ){
					if( isFixed )			UnionEffect::Set(	tank[ id ]->GetLeg().obj,	tank[ id ]->GetBody().head,		0	);
					else					UnionEffect::Set(	tank[ id ]->GetLeg().obj,	tank[ id ]->GetBody().obj,		0	);
				}
				if( m_union.nowParts == 2 )	UnionEffect::Set(	tank[ id ]->GetBody().head,	tank[ id ]->GetCannon().obj,	1	);
			}

#ifdef	USE_SOUND_EFFECT
			//	���̉��Đ�
			{
				bool	loop		=	false;
				float	pitch		=	0.9f + 0.2f * ( rand() / ( float )RAND_MAX );
				float	gain		=	0.3f;

				if( m_union.nowParts == 0 )	SoundManager::Play( "���̉��Q", loop, pitch, gain );
				else						SoundManager::Play( "���̉�", loop, pitch, gain );
			}
#endif

			m_union.nowParts++;
			m_union.value = 0;
		}


		//	���̏I��
		if( m_union.nowParts >= 3 )
		{		
			m_union.m_state			= Union::ZYAZYA__N;
			m_union.t				= effect[team].start;
			m_union.timer			= 0;
		}
	} 
	break;

	case Union::ZYAZYA__N:
		if( m_union.timer == 20 )
		{
#ifdef	USE_SOUND_EFFECT
			SoundManager::Play( "���̉��R" );
#endif
		}
		if( ++m_union.timer > 120 )
			m_union.m_state	= Union::ROT_CAMERA;

		break;
	case Union::ROT_CAMERA:
	{
		float rotSpeed(1.5f);
		m_union.t += D3DXToRadian(rotSpeed);

		Vector3 rotPos	= appMathAPI::CircularMovement( slotPos, m_union.t, dist );
		rotPos.y		= makeCamera.GetPos().y;
		Vector3 target	( slotPos.x, slotPos.y+height, slotPos.z );



		makeCamera.SetPos	( rotPos );
		makeCamera.SetTarget( target );


		//	��ԕ���
		if( m_union.t >= effect[team].end )
		{		
			m_union.m_state		= Union::INIT_CAMERA;

			ReloadGauge::Clear();
			ReloadGauge::Activate();
	
			MiniMap::Clear();
			MiniMap::Activate();
	
			PlayerHPGageType2::Clear();
			PlayerHPGageType2::Activate( GameMasterClientA::GetMyID() );

			FrontGuide::Clear();
			FrontGuide::Activate();

			Alignment::Activate();
			BallisticGuide::Activate();

			return true;
		}
	}break;

	default:	break;
	}

	return false;
}
		
sceneMutch::Union::Union()
{ 
	m_state = INIT_CAMERA;
	pEffect = new Image("data\\effect\\speed-line.png");
}
sceneMutch::Union::~Union()
{
	delete pEffect;
}
void sceneMutch::Union::Render()
{
	if( m_state == ZYAZYA__N && timer > 30 )
	{
		float drawX	= 0;
		float drawY	= 0;
		float drawW	= (float)iexSystem::ScreenWidth;
		float drawH	= (float)iexSystem::ScreenHeight;
		float loadX	= 0;
		float loadY	= 0;
		float loadW	= 1280.0f;
		float loadH	= 720.0f;
		float depth = .0f;
		float angle = timer%10 > 5? .0f : PI;
		DWORD color = 0xFFFFFFFF;
		pEffect->CutDraw2D( drawX, drawY, drawW, drawH, loadX, loadY, loadW, loadH, depth, color, angle, RS_ADD );
	}
}
