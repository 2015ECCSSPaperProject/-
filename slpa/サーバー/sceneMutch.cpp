
#include	<time.h>

#include	"iextreme.h"
#include	"system/system.h"
#include	"system\Framework.h"


#include	"../Manager/GameManager.h"
#include	"../Manager/MeshRegister.h"

#include	"InputManager.h"
#include	"SystemAdd/Drawing.h"

#include	"../Object/Camera.h"

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


#include	"CollisionManager.h"
#include	"CollisionMesh.h"
#include	"../Manager/GameManager.h"

#include	"TankController.h"

#include	"Player.h"
#include	"AI.h"
#include	"NetPlayer.h"

#include	"Sky.h"
#include	"Effect.h"
#include	"Interface.h"

#include	"ShadingManager.h"
#include	"InformationBuffer.h"

#include	"SoundManager.h"
#include	"scenemain.h"
#include	"sceneMutch.h"
#include	"../Slot/Slot.h"
#include	"../Slot/Coin.h"
#include	"../Object/TankFactory.h"
#include	"Server.h"

#include	"Event.h"
#include 	"Gimmick.h"
//*****************************************************************************************************************************
//
//		�}���`�ΐ�V�[��
//
//*****************************************************************************************************************************
	//		������
	//*************************************************************************************************************************
	bool	sceneMutch::Initialize( void )
	{
		//	���E�̏�����
		{
			iexLight::SetAmbient( 0x404040 );
			iexLight::SetFog( 800, 1000, 0 );

			Vector3 dir( 1.0f, -1.0f, -0.5f );
			dir.Normalize();
			iexLight::DirLight( shader, 0, &dir, 0.8f, 0.8f, 0.8f );
		}

		//	�`��}�l�[�W���[�̏�����
		{
			//	�`��}�l�[�W���[������
			ShadingManager::InitializeForGame();

			//	�`��p�����[�^�̏�����
			D3DXCOLOR	fogColor		=	{	0.0f,	0.0f,	0.0f,	0.0f	};
			D3DXCOLOR	skyColor		=	{	0.3f,	0.3f,	1.0f,	0.0f	};
			D3DXCOLOR	groundColor		=	{	0.62f,	0.48f,	0.4f,	0.0f	};
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

			ShadingManager::SetScreenColor( 1.0f, 1.0f, 1.0f );
			ShadingManager::SetContrast( 1.0f );
			ShadingManager::SetSaturation( 1.0f );
			ShadingManager::SetBrightness( 0.0f );
		}
	
		//	�A�C�e���̏�����
		{
			ItemManager::Init( 8, "data\\item\\bluecoin.x");

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
			InputManager::Clear();

			//
			CollisionPair::Initialize();
			MissileManager::Initialize();
			StageManager::Initialize();
			//EventManager::Initialize();

#ifdef	USE_EFFECT
			//	�G�t�F�N�g������
			ImpactEffect::InitializeForGame();
			ImpactEffect2::InitializeForGame();
			FireEffect::InitializeForGame();
			MuzzleFlashEffect::InitializeForGame();
			ExplosionEffect::InitializeForGame();
			FloorRuddingEffect::InitializeForGame();
			SparkEffect::InitializeForGame();
			SmokeManager::InitializeForGame();
			MissileSmokeEffect::InitializeForGame();
			MissileExplosionEffect::InitializeForGame();
			LaserEffect::InitializeForGame();
			LaserHitEffect::InitializeForGame();
#endif
		
			//	�C���^�[�t�F�C�X������
			GameLog::InitializeForGame();
			NamePlate::InitializeForGame();
			EnemyHPGage::InitializeForGame();
			ChatLog::InitializeForGame();
		}

		//	��Ԃ̏�����
		{
			for( s32 i = 0; i <PANZER_MAX ; ++i )
			{
				if( ServerManager::GetUser( i ).row == 0xFF )
				{
					tank[i]	=	new AI();
				}
				else
				{
					tank[ i ]	=	new NetPlayer( i );
				}
				tank[i]->Death();
				tank[i]->SetPos( Vector3( 0.0f, 0.0f, 0.0f ) );
			}

			//	���o�b�t�@�ɓo�^
			InfoBuffer::SetPlayerList( ( const Player** )tank );
		}

		//	�J����������
		{
			//	�T�[�o�[�J�����𐶐�
			m_pCamera		=	new	ServerCamera;

			//	���o�b�t�@�ɓo�^
			InfoBuffer::SetCamera( ( const ServerCamera* )m_pCamera );
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

		//	�`��p�X�e�[�W�Ǘ��N���X������
		RenderStageManager::InitializeForGame();

		//	�`��X�L�b�v�t���O������
		m_SkipRender	=	true;

		//
		GimmickManager::Init();
		return	true;
	}
	//*************************************************************************************************************************
	//		���
	//*************************************************************************************************************************
	sceneMutch::~sceneMutch( void )
	{
		//	�J�������
		delete	m_pCamera;

		//	��ԉ��
		for( s32 i = 0; i < PANZER_MAX; i++ ){
			delete	tank[i];
		}
		
		//
		StageManager::Release();

		//
		MissileManager::Release();

#ifdef	USE_EFFECT
		//	�G�t�F�N�g���
		ImpactEffect::ReleaseForGame();
		ImpactEffect2::ReleaseForGame();
		FireEffect::ReleaseForGame();
		MuzzleFlashEffect::ReleaseForGame();
		ExplosionEffect::ReleaseForGame();
		FloorRuddingEffect::ReleaseForGame();
		SparkEffect::ReleaseForGame();
		SmokeManager::ReleaseForGame();
		MissileSmokeEffect::ReleaseForGame();
		MissileExplosionEffect::ReleaseForGame();
		LaserEffect::ReleaseForGame();
		LaserHitEffect::ReleaseForGame();
#endif

		//	�C���^�[�t�F�C�X���
		GameLog::ReleaseForGame();
		NamePlate::ReleaseForGame();
		EnemyHPGage::ReleaseForGame();
		ChatLog::ReleaseForGame();

		//	�O���l�[�h�Ǘ��N���X�J��
		GrenadeManager::ReleaseForGame();
		//	�U���~�T�C���Ǘ��N���X���
		HomingMissileManager::ReleaseForGame();
		//	���[�U�[�Ǘ��N���X���
		LaserManager::ReleaseForGame();
		//	��������Ǘ��N���X���
		ShishamoManager::ReleaseForGame();
		//	�V�[���h�Ǘ��N���X���
		ShieldManager::ReleaseForGame();
	
		//�E�F�|���J�v�Z���Ǘ��N���X�J��
		PunchManager::Release();
		BurningManager::Release();

		//	�`��}�l�[�W���[���
		ShadingManager::ReleaseForGame();

		//	�A�C�e�����
		ItemManager::Release();

		//	�M�~�b�N���
		GimmickManager::Release();

		//	�`��p�X�e�[�W�Ǘ��N���X���
		RenderStageManager::ReleaseForGame();
	
	}
	//*************************************************************************************************************************
	//		�X�V
	//*************************************************************************************************************************
	void	sceneMutch::Update( void ) 
	{		
		DWORD start = timeGetTime();

		GAME_STATE_TYPE curType = GAME_MASTER->GetGameState();
		if( curType == GAME_STATE_TYPE::MUTCH )
		{	
			//
			CollisionManager::Update();


			// ��L���X�V
			StageManager::UpdateOccupyRate();
	
			// �C�x���g�X�V
			//EventManager::Update();
	
			//	��ԍX�V
			for( s32 i = 0; i < PANZER_MAX; i++ ){
				switch( GAME_MASTER->GetPlayerState( i ) ){
				case	RESPAWN:	
				case	COMBAT:		CombatTank( i );	break;
				case	DEATH:		DeathTank( i );		break;
				default:								break;
				}
			}

			//	�e�̍X�V
			MissileManager::Update();
			GrenadeManager::Update();
			HomingMissileManager::Update();
			LaserManager::Update();
			ShishamoManager::Update();

			//	�M�~�b�N�̍X�V
			GimmickManager::Update();

			PunchManager::Update();
			BurningManager::Update();

			//	�V�[���h�X�V
			ShieldManager::Update();


				
#ifdef	USE_EFFECT
			//	�G�t�F�N�g�X�V
			SmokeManager::Update();
			ExplosionEffect::Update();
			ImpactEffect::Update();
			ImpactEffect2::Update();
			FireEffect::Update();
			MuzzleFlashEffect::Update();
			SparkEffect::Update();
			MissileSmokeEffect::Update();
			MissileExplosionEffect::Update();
			LaserEffect::Update();
			LaserHitEffect::Update();
#endif
	
			ItemManager::Update();
	
			//	�C���^�[�t�F�C�X�X�V
			GameLog::Update();
			NamePlate::Update();
			ChatLog::Update();
			EnemyHPGage::Update();
		}

		//	�T�[�o���M�ׂ̈̏���
		GAME_MASTER->Update();

		//	�J�����X�V
		m_pCamera->Update();

		//	�`��p�X�e�[�W�Ǘ��N���X�X�V
		{
			if( m_pCamera->GetMode() == ServerCamera::Mode::MODE_SCOUT ){
				int		focusPlayerNum	=	m_pCamera->GetScoutPlayerNum();

				Vector3	targetPos		=	tank[ focusPlayerNum ]->GetPos();
				Vector3	cameraPos		=	m_pCamera->GetPos();
				Vector3	direction		=	targetPos - cameraPos;
						direction.Normalize();

				RenderStageManager::Update( cameraPos, direction );
			}
			else{
				RenderStageManager::Clear();
			}
		}

#ifdef	USE_SOUND_EFFECT
		//	���X�i�[�̍X�V
		{
			const	Vector3	pos			=	m_pCamera->GetPos();
			const	Vector3	velocity	=	Vector3( 0.0f, 0.0f, 0.0f );
			const	Vector3	vFront		=	Vector3( matView._13, matView._23, matView._33 );
			const	Vector3	vUp			=	Vector3( matView._12, matView._22, matView._32 );

			SoundManager::UpdateListener( pos, velocity, vFront, vUp );
		}
#endif

		//	�`��X�L�b�v�t���O�X�V�i���E�B���h�E�Ƀt�H�[�J�X������ꍇ�͖��������j
		if( InputManager::GetKeyState( DIK_V ) == KEY_STATE::KEY_STATE_PRESSED ){
			m_SkipRender	=	!m_SkipRender;
		}
		DWORD end = timeGetTime();
		//printf("ColTIme %d\n", end - start);
	}
	//*************************************************************************************************************************
	//		�`��
	//*************************************************************************************************************************
	void	sceneMutch::Render( void )
	{	
		m_pCamera->GetView()->Clear();
		
		if(KEY_Get(KEY_SPACE))
			GAME_MASTER->dbgInfo();
		//	�`��X�L�b�v0
		if( m_SkipRender )
		{
			return;
		}
		//3D
		ShadingManager::ClearTexture();
		ShadingManager::SetEyePosition( m_pCamera->GetPos() );
		ShadingManager::SetEyeGaze( m_pCamera->GetTarget() );
		{
			//	�X�e�[�W�`��i���ƕǁj
			RenderStageManager::RenderBase();

			//	�e�̕`��
			MissileManager::Render();
			GrenadeManager::Render();
			HomingMissileManager::Render();
			LaserManager::Render();
			ShishamoManager::Render();
			PunchManager::Render();

			// �A�C�e���R�C���`��
			ItemManager::Render3D();

			//	��ԕ`��
			for( s32 i = 0; i < PANZER_MAX; i++ ){
				tank[i]->Render();
			}

			//	�X�e�[�W�`��i���ƕǈȊO�j
			RenderStageManager::RenderObstacle();

			//	�M�~�b�N�`��
			GimmickManager::Render();
		}
		
		//	�\�񂳂ꂽ���b�V����`��
		ShadingManager::Render();

#ifdef	USE_EFFECT
		//	�G�t�F�N�g�`��
		SmokeManager::Render();
		MissileSmokeEffect::Render();
		ExplosionEffect::Render();
		MissileExplosionEffect::Render();
		ImpactEffect::Render();
		ImpactEffect2::Render();
		MuzzleFlashEffect::Render();
		SparkEffect::Render();
		LaserEffect::Render();
		LaserHitEffect::Render();
#endif

		//	�V�[���h�`��
		ShieldManager::Render();

		//�o�[�j���O�`��
		BurningManager::Render();

		//	���߂��Ă���I�u�W�F�N�g��`��
		RenderStageManager::RenderAlphaObstacle();
		ShadingManager::Render();

		//2D
		// �A�C�e���X���b�g�`��
		ItemManager::Render2D();

		//	����ɂg�o�o�[��`��
		{
			//	�`��\��
			for( int i = 0; i < PANZER_MAX; i++ ){
				Vector3		drawPos		=	tank[i]->GetPos();
				int			hp			=	(int)( tank[i]->GetHP()	    + 0.5f );
				int			damage		=	(int)( tank[i]->GetDamage() + 0.5f );
				D3DXCOLOR	gageColor	=	0xFF20dd20;

				//	�`��\��
				EnemyHPGage::ReserveRender( gageColor, drawPos, hp, damage );
			}

			//	�`��
			EnemyHPGage::Render();
		}

		//	����Ƀv���C���[����`��
		{
			//	�`�[���h�c�擾
			Global::TEAM_NAME	teamID[]	=	{
				GAME_MASTER->GetPlayerTeam( 0 ),
				GAME_MASTER->GetPlayerTeam( 1 ),
				GAME_MASTER->GetPlayerTeam( 2 ),
				GAME_MASTER->GetPlayerTeam( 3 ),
				GAME_MASTER->GetPlayerTeam( 4 ),
				GAME_MASTER->GetPlayerTeam( 5 ),
				GAME_MASTER->GetPlayerTeam( 6 ),
				GAME_MASTER->GetPlayerTeam( 7 ),
			};
			//	�`��p�����[�^
			struct{
				LPCSTR	name;
				DWORD	fillColor;
				DWORD	edgeColor;
			}	c_Param[]	=	{
				{	GAME_MASTER->GetPlayerName( 0 ),	( teamID[0] == Global::TEAM_A )?	0xFFff5050	:	0xFF5050ff,	0xAF000000	},
				{	GAME_MASTER->GetPlayerName( 1 ),	( teamID[1] == Global::TEAM_A )?	0xFFff5050	:	0xFF5050ff,	0xAF000000	},
				{	GAME_MASTER->GetPlayerName( 2 ),	( teamID[2] == Global::TEAM_A )?	0xFFff5050	:	0xFF5050ff,	0xAF000000	},
				{	GAME_MASTER->GetPlayerName( 3 ),	( teamID[3] == Global::TEAM_A )?	0xFFff5050	:	0xFF5050ff,	0xAF000000	},
				{	GAME_MASTER->GetPlayerName( 4 ),	( teamID[4] == Global::TEAM_A )?	0xFFff5050	:	0xFF5050ff,	0xAF000000	},
				{	GAME_MASTER->GetPlayerName( 5 ),	( teamID[5] == Global::TEAM_A )?	0xFFff5050	:	0xFF5050ff,	0xAF000000	},
				{	GAME_MASTER->GetPlayerName( 6 ),	( teamID[6] == Global::TEAM_A )?	0xFFff5050	:	0xFF5050ff,	0xAF000000	},
				{	GAME_MASTER->GetPlayerName( 7 ),	( teamID[7] == Global::TEAM_A )?	0xFFff5050	:	0xFF5050ff,	0xAF000000	},
			};

			//	�`��\��
			for( int i = 0; i < PANZER_MAX; i++ ){
				//	�p�����[�^���v�Z
				LPCSTR	drawStr		=	c_Param[i].name;
				Vector3	drawPos		=	tank[i]->GetPos();
						drawPos.y	+=	30.0f;

				//	�`��\��
				NamePlate::RenderReserve( drawPos, drawStr, c_Param[i].fillColor, c_Param[i].edgeColor );
			}

			//	�\�񂳂ꂽ�f�[�^��[�x���Ƀ\�[�g���ĕ`��
			NamePlate::Render();
		}

		//	�Q�[�����O�`��
		GameLog::Render();

		//	�`���b�g���O�`��
		ChatLog::Render();

		//	�R�C���̏��������\��
		DisplayPlayerData();

		//	�J�����̑���K�C�h�\��
		DisplayCameraGuide();

		char	str[256];
		sprintf_s( str, "cameraX = %f\ncameraZ = %f", m_pCamera->GetPos().x, m_pCamera->GetPos().z );

		IEX_DrawText( str, 300, 300, 500, 500, 0xFFff0000 );
	}
	//*************************************************************************************************************************
	//		���
	//*************************************************************************************************************************
	void	sceneMutch::CombatTank( int id )
	{
		//if (KEY_Get(KEY_A) == 3)
		//{
		//	Punch* p = new Punch(id);
		//	if (!PunchManager::Set(p))
		//	{
		//		delete p;
		//	}

		//}

		tank[ id ]->Update();
	}
	void	sceneMutch::DeathTank( int id )
	{
		if( GAME_MASTER->IsAITank( id ) == true )
		{
			static int playStep[PANZER_MAX]		= { 0 };	// ���ݎg�p���X���b�g(AI�p)
			static int waitTimer[PANZER_MAX]	= { 0 };	// �^�C�}�[
			static int stopCnt[PANZER_MAX]		= { 0 };	// �~�܂郊�[��( ��->��->�� )
			const int  probailityTime			= 10;		// ���I�Ԋu(  �����قǎ~�܂�ɂ��� )
			const int  probaility				= 10;		// �~�܂�m��( �����قǎ~�܂�ɂ��� )
			enum { SEARCH, BET, PLAY };
			switch( playStep[id] )
			{
			case SEARCH:
			{
				size_t st = ( GAME_MASTER->GetPlayerTeam(id) == Global::TEAM_A ) ? 0 : 4;
				int selectSlot = st + GAME_MASTER->GetPlayerTeamNo( id );
				int usingPlayer = GAME_MASTER->SearchSlot( selectSlot );
				if( usingPlayer == -1 )
				{
					GAME_MASTER->PlaySlot( id, selectSlot, 0x00 );
					playStep[id] = BET;
				}			
			}
				break;
			case BET:
			{
				int playSlot = GAME_MASTER->GetCurPlaySlot( id );
				GAME_MASTER->PlaySlot( id, playSlot, 0x10 );
				playStep[id]  = PLAY;
				break;
			}
			case PLAY:
			{
				int playSlot = GAME_MASTER->GetCurPlaySlot( id );
				if( (++waitTimer[id]%probailityTime) == 0 && rand()%probaility == 0 )
				{
					GAME_MASTER->PlaySlot( id, playSlot, 0x01 << stopCnt[id] );
					++stopCnt[id];
					waitTimer[id] = 0;
				}
				if( stopCnt[id] == 3 )
				{
					playStep[id] = SEARCH;
					stopCnt[id]	 = 0;
				}
			}
				break;
			default:break;
			}

//			const  int waitMax					= 100;
//			if( ++waitTimer[id] > waitMax )
//			{
//				waitTimer[id] = 0;		
//			
//				size_t st;
//				st	= ( GAME_MASTER->GetPlayerTeam(id) == Global::TEAM_A ) ? 0 : 4;
//				int selectSlot = st + GAME_MASTER->GetPlayerTeamNo( id );
//				int usingPlayer = GAME_MASTER->SearchSlot( selectSlot );
//				if( usingPlayer == -1 )
//				{
//					GAME_MASTER->RespawnTank(id,tank[id], 1, selectSlot );
//				}			
//			}
		}
		
	}
	//*************************************************************************************************************************
	//		�T�|�[�g�֐�
	//*************************************************************************************************************************
	//	�v���C���[�̏���\��
	void	sceneMutch::DisplayPlayerData( void )
	{
		//	�e�`�[���̃v���C���[�����擾
		LPCSTR	redPlayerName[4];
		int		redPlayerCoin[4];
		LPCSTR	bluePlayerName[4];
		int		bluePlayerCoin[4];
		{
			int		redCursor	=	0;
			int		blueCursor	=	0;
			for( int i = 0; i < PANZER_MAX; i++ ){
				if( GAME_MASTER->GetPlayerTeam( i ) == Global::TEAM_A ){
					redPlayerName[ redCursor ]	=	GAME_MASTER->GetPlayerName( i );
					redPlayerCoin[ redCursor ]	=	GAME_MASTER->GetPlayerCoinNum( i );
					++redCursor;
				}
				else{
					bluePlayerName[ blueCursor ]	=	GAME_MASTER->GetPlayerName( i );
					bluePlayerCoin[ blueCursor ]	=	GAME_MASTER->GetPlayerCoinNum( i );
					++blueCursor;
				}
			}
		}

		//	���O�̕\��
		{
			char	redPlayerNameStr[256];
			char	bluePlayerNameStr[256];
			sprintf_s( redPlayerNameStr, "%s\n%s\n%s\n%s", redPlayerName[0], redPlayerName[1], redPlayerName[2], redPlayerName[3] );
			sprintf_s( bluePlayerNameStr, "%s\n%s\n%s\n%s", bluePlayerName[0], bluePlayerName[1], bluePlayerName[2], bluePlayerName[3] );

			IEX_DrawText( redPlayerNameStr,  400, 50, 1280, 720, 0xFFff0000 );
			IEX_DrawText( bluePlayerNameStr, 600, 50, 1280, 720, 0xFF0000ff );
		}

		//	�R�C���̕\��
		{
			char	redPlayerCoinStr[256];
			char	bluePlayerCoinStr[256];
			sprintf_s( redPlayerCoinStr, "%3d\n%3d\n%3d\n%3d", redPlayerCoin[0], redPlayerCoin[1], redPlayerCoin[2], redPlayerCoin[3] );
			sprintf_s( bluePlayerCoinStr, "%3d\n%3d\n%3d\n%3d", bluePlayerCoin[0], bluePlayerCoin[1], bluePlayerCoin[2], bluePlayerCoin[3] );

			IEX_DrawText( redPlayerCoinStr, 525, 50, 1280, 720, 0xFFffff00 );
			IEX_DrawText( bluePlayerCoinStr, 725, 50, 1280, 720, 0xFFffff00 );
		}
	}
	//	�J�����̑�����@��\��
	void	sceneMutch::DisplayCameraGuide( void )
	{
		//	���[�h�̐؂�ւ����@
		{
			IEX_DrawText( "[ 1 ] �L�[      �����낵�J����\n[ 2 ] �L�[      �t���[�J����\n[ 3 ] �L�[      �ǐՃJ����",
				20, 200, 500, 500, 0xFFff0000 );
		}

		//	�g�p���̃J����
		{
			static	LPCSTR	c_CameraName[]	=	{
				"�����낵�J����",
				"�t���[�J����",
				"�ǐՃJ����",
			};

			char	str[256];
			sprintf_s( str, "�g�p���̃J����   =   %s", c_CameraName[ m_pCamera->GetMode() ] );
			IEX_DrawText( str, 20, 270, 500, 500, 0xFFffaf00 );
		}

		//	������@
		{
			static	LPCSTR	c_Str[]		=	{
				"[ NumPad2 ] [ NumPad4 ] [ NumPad6 ] [ NumPad8 ] �L�[  =   �ړ�\n[ NumPad7 ] [ NumPad9 ] �L�[                          =   �Y�[��\n[ NumPad5 ] �L�[                                      =   ���Z�b�g",
				"[ NumPad4 ] [ NumPad6 ] �L�[                          =   ����]\n[ NumPad2 ] [ NumPad8 ] �L�[                          =   �c��]\n[ �� ] [ �� ] [ �� ] [ �� ] �L�[                      =   �����ړ�\n[ NumPad3 ] [ NumPad9 ] �L�[                          =   �㉺�ړ�\n[ NumPad5 ] �L�[                                      =   ���Z�b�g",
				"[ �� ] [ �� ] �L�[                                    =   �ǐՂ���v���C���[��ύX\n[ NumPad4 ] [ NumPad6 ] �L�[                          =   ����]\n[ NumPad2 ] [ NumPad8 ] �L�[                          =   �c��]",
			};

			IEX_DrawText( ( LPSTR )c_Str[ m_pCamera->GetMode() ], 20, 600, 1000, 1000, 0xFFffff00 );
		}
	}
