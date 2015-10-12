
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
//		マルチ対戦シーン
//
//*****************************************************************************************************************************
	//		初期化
	//*************************************************************************************************************************
	bool	sceneMutch::Initialize( void )
	{
		//	視界の初期化
		{
			iexLight::SetAmbient( 0x404040 );
			iexLight::SetFog( 800, 1000, 0 );

			Vector3 dir( 1.0f, -1.0f, -0.5f );
			dir.Normalize();
			iexLight::DirLight( shader, 0, &dir, 0.8f, 0.8f, 0.8f );
		}

		//	描画マネージャーの初期化
		{
			//	描画マネージャー初期化
			ShadingManager::InitializeForGame();

			//	描画パラメータの初期化
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
	
		//	アイテムの初期化
		{
			ItemManager::Init( 8, "data\\item\\bluecoin.x");

			//	リスポーン座標のセット			
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
			//	エフェクト初期化
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
		
			//	インターフェイス初期化
			GameLog::InitializeForGame();
			NamePlate::InitializeForGame();
			EnemyHPGage::InitializeForGame();
			ChatLog::InitializeForGame();
		}

		//	戦車の初期化
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

			//	情報バッファに登録
			InfoBuffer::SetPlayerList( ( const Player** )tank );
		}

		//	カメラ初期化
		{
			//	サーバーカメラを生成
			m_pCamera		=	new	ServerCamera;

			//	情報バッファに登録
			InfoBuffer::SetCamera( ( const ServerCamera* )m_pCamera );
		}

		//	グレネード管理クラス初期化
		GrenadeManager::InitializeForGame();
		//	誘導ミサイル管理クラス初期化
		HomingMissileManager::InitializeForGame();
		//	レーザー管理クラス初期化
		LaserManager::InitializeForGame();
		//	ししゃも管理クラス初期化
		ShishamoManager::InitializeForGame();
		//	シールド管理クラス初期化
		ShieldManager::InitializeForGame();

		//ウェポンカプセル管理クラス初期化
		PunchManager::Initialize();
		BurningManager::Initialize();

		//	描画用ステージ管理クラス初期化
		RenderStageManager::InitializeForGame();

		//	描画スキップフラグ初期化
		m_SkipRender	=	true;

		//
		GimmickManager::Init();
		return	true;
	}
	//*************************************************************************************************************************
	//		解放
	//*************************************************************************************************************************
	sceneMutch::~sceneMutch( void )
	{
		//	カメラ解放
		delete	m_pCamera;

		//	戦車解放
		for( s32 i = 0; i < PANZER_MAX; i++ ){
			delete	tank[i];
		}
		
		//
		StageManager::Release();

		//
		MissileManager::Release();

#ifdef	USE_EFFECT
		//	エフェクト解放
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

		//	インターフェイス解放
		GameLog::ReleaseForGame();
		NamePlate::ReleaseForGame();
		EnemyHPGage::ReleaseForGame();
		ChatLog::ReleaseForGame();

		//	グレネード管理クラス開放
		GrenadeManager::ReleaseForGame();
		//	誘導ミサイル管理クラス解放
		HomingMissileManager::ReleaseForGame();
		//	レーザー管理クラス解放
		LaserManager::ReleaseForGame();
		//	ししゃも管理クラス解放
		ShishamoManager::ReleaseForGame();
		//	シールド管理クラス解放
		ShieldManager::ReleaseForGame();
	
		//ウェポンカプセル管理クラス開放
		PunchManager::Release();
		BurningManager::Release();

		//	描画マネージャー解放
		ShadingManager::ReleaseForGame();

		//	アイテム解放
		ItemManager::Release();

		//	ギミック解放
		GimmickManager::Release();

		//	描画用ステージ管理クラス解放
		RenderStageManager::ReleaseForGame();
	
	}
	//*************************************************************************************************************************
	//		更新
	//*************************************************************************************************************************
	void	sceneMutch::Update( void ) 
	{		
		DWORD start = timeGetTime();

		GAME_STATE_TYPE curType = GAME_MASTER->GetGameState();
		if( curType == GAME_STATE_TYPE::MUTCH )
		{	
			//
			CollisionManager::Update();


			// 占有率更新
			StageManager::UpdateOccupyRate();
	
			// イベント更新
			//EventManager::Update();
	
			//	戦車更新
			for( s32 i = 0; i < PANZER_MAX; i++ ){
				switch( GAME_MASTER->GetPlayerState( i ) ){
				case	RESPAWN:	
				case	COMBAT:		CombatTank( i );	break;
				case	DEATH:		DeathTank( i );		break;
				default:								break;
				}
			}

			//	弾の更新
			MissileManager::Update();
			GrenadeManager::Update();
			HomingMissileManager::Update();
			LaserManager::Update();
			ShishamoManager::Update();

			//	ギミックの更新
			GimmickManager::Update();

			PunchManager::Update();
			BurningManager::Update();

			//	シールド更新
			ShieldManager::Update();


				
#ifdef	USE_EFFECT
			//	エフェクト更新
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
	
			//	インターフェイス更新
			GameLog::Update();
			NamePlate::Update();
			ChatLog::Update();
			EnemyHPGage::Update();
		}

		//	サーバ送信の為の処理
		GAME_MASTER->Update();

		//	カメラ更新
		m_pCamera->Update();

		//	描画用ステージ管理クラス更新
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
		//	リスナーの更新
		{
			const	Vector3	pos			=	m_pCamera->GetPos();
			const	Vector3	velocity	=	Vector3( 0.0f, 0.0f, 0.0f );
			const	Vector3	vFront		=	Vector3( matView._13, matView._23, matView._33 );
			const	Vector3	vUp			=	Vector3( matView._12, matView._22, matView._32 );

			SoundManager::UpdateListener( pos, velocity, vFront, vUp );
		}
#endif

		//	描画スキップフラグ更新（他ウィンドウにフォーカスがある場合は無視される）
		if( InputManager::GetKeyState( DIK_V ) == KEY_STATE::KEY_STATE_PRESSED ){
			m_SkipRender	=	!m_SkipRender;
		}
		DWORD end = timeGetTime();
		//printf("ColTIme %d\n", end - start);
	}
	//*************************************************************************************************************************
	//		描画
	//*************************************************************************************************************************
	void	sceneMutch::Render( void )
	{	
		m_pCamera->GetView()->Clear();
		
		if(KEY_Get(KEY_SPACE))
			GAME_MASTER->dbgInfo();
		//	描画スキップ0
		if( m_SkipRender )
		{
			return;
		}
		//3D
		ShadingManager::ClearTexture();
		ShadingManager::SetEyePosition( m_pCamera->GetPos() );
		ShadingManager::SetEyeGaze( m_pCamera->GetTarget() );
		{
			//	ステージ描画（床と壁）
			RenderStageManager::RenderBase();

			//	弾の描画
			MissileManager::Render();
			GrenadeManager::Render();
			HomingMissileManager::Render();
			LaserManager::Render();
			ShishamoManager::Render();
			PunchManager::Render();

			// アイテムコイン描画
			ItemManager::Render3D();

			//	戦車描画
			for( s32 i = 0; i < PANZER_MAX; i++ ){
				tank[i]->Render();
			}

			//	ステージ描画（床と壁以外）
			RenderStageManager::RenderObstacle();

			//	ギミック描画
			GimmickManager::Render();
		}
		
		//	予約されたメッシュを描画
		ShadingManager::Render();

#ifdef	USE_EFFECT
		//	エフェクト描画
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

		//	シールド描画
		ShieldManager::Render();

		//バーニング描画
		BurningManager::Render();

		//	透過しているオブジェクトを描画
		RenderStageManager::RenderAlphaObstacle();
		ShadingManager::Render();

		//2D
		// アイテムスロット描画
		ItemManager::Render2D();

		//	頭上にＨＰバーを描画
		{
			//	描画予約
			for( int i = 0; i < PANZER_MAX; i++ ){
				Vector3		drawPos		=	tank[i]->GetPos();
				int			hp			=	(int)( tank[i]->GetHP()	    + 0.5f );
				int			damage		=	(int)( tank[i]->GetDamage() + 0.5f );
				D3DXCOLOR	gageColor	=	0xFF20dd20;

				//	描画予約
				EnemyHPGage::ReserveRender( gageColor, drawPos, hp, damage );
			}

			//	描画
			EnemyHPGage::Render();
		}

		//	頭上にプレイヤー名を描画
		{
			//	チームＩＤ取得
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
			//	描画パラメータ
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

			//	描画予約
			for( int i = 0; i < PANZER_MAX; i++ ){
				//	パラメータを計算
				LPCSTR	drawStr		=	c_Param[i].name;
				Vector3	drawPos		=	tank[i]->GetPos();
						drawPos.y	+=	30.0f;

				//	描画予約
				NamePlate::RenderReserve( drawPos, drawStr, c_Param[i].fillColor, c_Param[i].edgeColor );
			}

			//	予約されたデータを深度順にソートして描画
			NamePlate::Render();
		}

		//	ゲームログ描画
		GameLog::Render();

		//	チャットログ描画
		ChatLog::Render();

		//	コインの所持枚数表示
		DisplayPlayerData();

		//	カメラの操作ガイド表示
		DisplayCameraGuide();

		char	str[256];
		sprintf_s( str, "cameraX = %f\ncameraZ = %f", m_pCamera->GetPos().x, m_pCamera->GetPos().z );

		IEX_DrawText( str, 300, 300, 500, 500, 0xFFff0000 );
	}
	//*************************************************************************************************************************
	//		戦車
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
			static int playStep[PANZER_MAX]		= { 0 };	// 現在使用中スロット(AI用)
			static int waitTimer[PANZER_MAX]	= { 0 };	// タイマー
			static int stopCnt[PANZER_MAX]		= { 0 };	// 止まるリール( 上->中->下 )
			const int  probailityTime			= 10;		// 抽選間隔(  高いほど止まりにくい )
			const int  probaility				= 10;		// 止まる確率( 高いほど止まりにくい )
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
	//		サポート関数
	//*************************************************************************************************************************
	//	プレイヤーの情報を表示
	void	sceneMutch::DisplayPlayerData( void )
	{
		//	各チームのプレイヤー情報を取得
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

		//	名前の表示
		{
			char	redPlayerNameStr[256];
			char	bluePlayerNameStr[256];
			sprintf_s( redPlayerNameStr, "%s\n%s\n%s\n%s", redPlayerName[0], redPlayerName[1], redPlayerName[2], redPlayerName[3] );
			sprintf_s( bluePlayerNameStr, "%s\n%s\n%s\n%s", bluePlayerName[0], bluePlayerName[1], bluePlayerName[2], bluePlayerName[3] );

			IEX_DrawText( redPlayerNameStr,  400, 50, 1280, 720, 0xFFff0000 );
			IEX_DrawText( bluePlayerNameStr, 600, 50, 1280, 720, 0xFF0000ff );
		}

		//	コインの表示
		{
			char	redPlayerCoinStr[256];
			char	bluePlayerCoinStr[256];
			sprintf_s( redPlayerCoinStr, "%3d\n%3d\n%3d\n%3d", redPlayerCoin[0], redPlayerCoin[1], redPlayerCoin[2], redPlayerCoin[3] );
			sprintf_s( bluePlayerCoinStr, "%3d\n%3d\n%3d\n%3d", bluePlayerCoin[0], bluePlayerCoin[1], bluePlayerCoin[2], bluePlayerCoin[3] );

			IEX_DrawText( redPlayerCoinStr, 525, 50, 1280, 720, 0xFFffff00 );
			IEX_DrawText( bluePlayerCoinStr, 725, 50, 1280, 720, 0xFFffff00 );
		}
	}
	//	カメラの操作方法を表示
	void	sceneMutch::DisplayCameraGuide( void )
	{
		//	モードの切り替え方法
		{
			IEX_DrawText( "[ 1 ] キー      見下ろしカメラ\n[ 2 ] キー      フリーカメラ\n[ 3 ] キー      追跡カメラ",
				20, 200, 500, 500, 0xFFff0000 );
		}

		//	使用中のカメラ
		{
			static	LPCSTR	c_CameraName[]	=	{
				"見下ろしカメラ",
				"フリーカメラ",
				"追跡カメラ",
			};

			char	str[256];
			sprintf_s( str, "使用中のカメラ   =   %s", c_CameraName[ m_pCamera->GetMode() ] );
			IEX_DrawText( str, 20, 270, 500, 500, 0xFFffaf00 );
		}

		//	操作方法
		{
			static	LPCSTR	c_Str[]		=	{
				"[ NumPad2 ] [ NumPad4 ] [ NumPad6 ] [ NumPad8 ] キー  =   移動\n[ NumPad7 ] [ NumPad9 ] キー                          =   ズーム\n[ NumPad5 ] キー                                      =   リセット",
				"[ NumPad4 ] [ NumPad6 ] キー                          =   横回転\n[ NumPad2 ] [ NumPad8 ] キー                          =   縦回転\n[ ↑ ] [ ↓ ] [ ← ] [ → ] キー                      =   水平移動\n[ NumPad3 ] [ NumPad9 ] キー                          =   上下移動\n[ NumPad5 ] キー                                      =   リセット",
				"[ ← ] [ → ] キー                                    =   追跡するプレイヤーを変更\n[ NumPad4 ] [ NumPad6 ] キー                          =   横回転\n[ NumPad2 ] [ NumPad8 ] キー                          =   縦回転",
			};

			IEX_DrawText( ( LPSTR )c_Str[ m_pCamera->GetMode() ], 20, 600, 1000, 1000, 0xFFffff00 );
		}
	}
