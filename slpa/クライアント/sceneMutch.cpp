
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
//		初期化
//*****************************************************************************************************************************
bool	sceneMutch::Initialize()
{
	//	描画マネージャー初期化フラグをクリア
	m_InitShadingManager	=	false;

	//	空の色セット
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
		//■■■　相手と自分で分ける
		for (s32 i = 0; i <PANZER_MAX; ++i)
		{
			if( i == SOCKET_MANAGER->GetID())
				tank[i] = new Player();
			else
				tank[i] = new NetPlayer();
		}
		m_pSky = new iexMesh("data\\stage\\sky.x");
	}

	//	アイテム初期化
	{	
		ItemManager::Init( 8, "data\\item\\ItemCoinX.x" );

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
		//	入力情報クリア
		InputManager::Clear();

		//	エフェクト初期化
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
	
		//	インターフェイス初期化
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
	
		//	マウスへのアクセス権を取得

		InfoBuffer::SetPlayerList((const Player**)tank);
		Camera::Initialize();
		curlocalState		= DEATH;

		selectSlotTimer = 0;
		GameMasterClientA::SetSlotState( SELECT );
		
		slotButton = new SlotButton();
		motionState = new ViewerMotion(ViewerMotion::USAGE::WRITE);
		motionState->Load("DATA/Slot/bone.ykd");
		motionDirector = new ysSkeltonDirector( motionState->GetSkeltonSize() );

		//	終了イベント用クラスの生成
		m_end = new SceneEnd(this);
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

	//	スクラップ初期化
	TankScrapper::Initialize();
	TankScrapper::SettingCenterGravity();

	//	描画用ステージ管理クラス初期化
	RenderStageManager::InitializeForGame();
	
	//
	GimmickManager::Init();	

	//
	m_pThread		= new EL::Thread( ThreadFunc, this );

	InputManager::SetMouseExclusion( true );

	//	同期をとる
	SOCKET_MANAGER->InitGame();//ここで

	//	ネットワークを開始する
	m_pThread->Run();

#ifdef	USE_SOUND_EFFECT
	//	ＢＧＭ再生開始
	{
		bool	loop	=	true;
		float	pitch	=	1.0f;
		float	gain	=	0.1f;
		m_BGMSourseID	=	SoundManager::Play( "戦闘ＢＧＭ", loop, pitch, gain );
	}
#endif

	return	true;
}
void	sceneMutch::InitializeShadingManager( void )
{
	//	描画マネージャー初期化
	ShadingManager::InitializeForGame();

	//	描画パラメータの初期化
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

	//	占有サークル
	ShadingManager::SetCircleColor	( 0xFFffffff );
	ShadingManager::SetCirclePos	( 0.0f, 0.0f );	
	ShadingManager::SetCircleWidth	( 10.0f );	
	ShadingManager::SetCircleRadius	( 200.0f );
}

//*****************************************************************************************************************************
//		解放
//*****************************************************************************************************************************
sceneMutch::~sceneMutch()
{
#ifdef	USE_SOUND_EFFECT
	//	ＢＧＭ停止
	SoundManager::Stop( m_BGMSourseID );
#endif

	//	スレッド終了
	delete m_pThread;
	delete m_end;
	delete m_pSky;

	//	戦車解放
	for ( s32 i=0; i<PANZER_MAX ; ++i )
		delete tank[i];

	//	カメラ解放
	Camera::Release();
	
	//	描画マネージャー解放
	ShadingManager::ReleaseForGame();
	
	//	エフェクト解放
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

	//	インターフェイス解放
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

	//	グレネード管理クラス解放
	GrenadeManager::ReleaseForGame();
	//	誘導ミサイル管理クラス解放
	HomingMissileManager::ReleaseForGame();
	//	レーザー管理クラス解放
	LaserManager::ReleaseForGame();
	//	ししゃも管理クラス解放
	ShishamoManager::ReleaseForGame();
	//	シールド管理クラス開放
	ShieldManager::ReleaseForGame();

	//ウェポンカプセル管理クラス開放
	PunchManager::Release();
	BurningManager::Release();

	//	スクラップ開放化
	TankScrapper::Release();

	//	描画用ステージ管理クラス解放
	RenderStageManager::ReleaseForGame();
	
	//	ギミック解放
	GimmickManager::Release();
	//	リスポーン用カメラ
	SAFE_DELETE( slotButton		);
	SAFE_DELETE( motionState	);		
	SAFE_DELETE( motionDirector );

}

//*****************************************************************************************************************************
//		更新
//*****************************************************************************************************************************
void	sceneMutch::Update( void ) 
{
	//	描画マネージャーの初期化
	if( m_InitShadingManager == false ){
		InitializeShadingManager();
		m_InitShadingManager	=	true;
	}

	//	描画マネージャー更新
	ShadingManager::Update();

	//	試合中
	if( !m_end->m_isEndMutch )
	{
		//	ネットワークの情報を反映させる
		GameMasterClientA::UpdateServerInfo();
		GAME_STATE_TYPE curType = GameMasterClientA::GetGameState();
		if( curType == GAME_STATE_TYPE::WAIT ) return;

		slotButton->Update();

		// イベント更新
		//EventManager::Update();

		//	スロット更新
		GameMasterClientA::UpdateSlots();

		//	戦車更新
		UpdateTanks();

		//	ゲーム情報更新
		switch(curlocalState)
		{
		case COMBAT:	UpdateCombatTank();		break;
		case DEATH:		UpdateDeathTank();		break;
		case RESPAWN:	UpdateRespawnTank();	break;
		default:								break;
		}

		//	グレネード用のガイド
		if( GameMasterClientA::GetMyTankState() != PLAYER_STATE::DEATH ){
			BallisticGuide::Set( SOCKET_MANAGER->GetID() );
		}
			
		//	描画用ステージ管理クラス更新
		RenderStageManager::Update( SOCKET_MANAGER->GetID() );

		//if( InputManager::GetKeyState( DIK_X ) )	ShieldManager::Activate( SOCKET_MANAGER->GetID() );
		//else										ShieldManager::Disactivate( SOCKET_MANAGER->GetID() );

		//ShieldManager::Activate( SOCKET_MANAGER->GetID() );

		//	オブジェクト更新
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

		//	エフェクト更新	
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

		//	インターフェイス更新
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

		//	ギミックの更新
		GimmickManager::Update();

		//	終了確認
		if( m_end->CheckEnd())
			SoundManager::Stop( m_BGMSourseID );
	}
	else
	{
		m_end->Update();
	}
}


//*****************************************************************************************************************************
//		描画
//*****************************************************************************************************************************
void	sceneMutch::Render( void )
{
	//	描画マネージャーが初期化されていなければスキップ
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
		//	描画用テクスチャをクリア
		ShadingManager::ClearTexture( SkyColor );

		//	ステート
		
		switch( curlocalState ){
		case	RESPAWN:	
		case	COMBAT:		SetUpCombatCamera();	break;
		case	DEATH:		SetUpDethTankCamera();	break;
		default:			break;
		}

#if		!defined( ANTI_ALIASING_DEFAULT )
		//	レンダーターゲットを変更
		ShadingManager::SetRenderTarget();
#endif

		m_pSky->Render();


		//	ミサイル描画
		MissileManager::Render();
		GrenadeManager::Render();
		HomingMissileManager::Render();
		LaserManager::Render();
		ShishamoManager::Render();

		iexSystem::Device->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

		//	イベントオブジェクト描画
		//EventManager::Render3D();

		//	ステージ描画（床と壁）
		RenderStageManager::RenderBase();

		//	ステージ描画（床と壁以外）
		RenderStageManager::RenderObstacle();

		//	スロット描画
		GameMasterClientA::RenderSlots();

		//■■■	タンクの描画（自分以外）
		for( int i = 0; i < PANZER_MAX; i++ ){
			if( i == SOCKET_MANAGER->GetID() )	continue;

			//	生きてる戦車だけを描画する
//			float	hp		=	tank[i]->GetHP();
//			float	damage	=	tank[i]->GetDamage();
//			if( damage < hp )	tank[i]->Render();
			if( tank[i]->IsAlive() == true )
				tank[i]->Render();
		}

		//■■■	タンクの描画（自分だけ）
		if( GameMasterClientA::GetMyTankState() != PLAYER_STATE::DEATH ){
			tank[ SOCKET_MANAGER->GetID() ]->Render();
		}

		// アイテムコイン描画
		ItemManager::Render3D();

	 	//破片描画
		TankScrapper::Render();
		
		//	ギミック描画
		GimmickManager::Render();

		//	ここで一旦、予約された描画処理を実行
		ShadingManager::Render();

		//	エフェクト描画
		SmokeManager::Render();
		MissileSmokeEffect::Render();
		ExplosionEffect::Render();
		MissileExplosionEffect::Render();
		UnionEffect::Render();

		//	シールド描画
		ShieldManager::Render();

		//バーニング描画
		BurningManager::Render();



		//	頭上のパラメータを描画
		EnemyHPGage::Render();
		NamePlate::Render();
		PrizeView::Render();

		iexParticle::Render();

		//	透過している障害物を描画
		RenderStageManager::RenderAlphaObstacle();
		ShadingManager::Render();

		//	前方ガイドを描画
		if( GameMasterClientA::GetMyTankState() != PLAYER_STATE::DEATH ){
			FrontGuide::Render();
		}

		//	加算エフェクト
		ImpactEffect::Render();
		ImpactEffect2::Render();
		MuzzleFlashEffect::Render();
		SparkEffect::Render();
		LaserEffect::Render();
		LaserHitEffect::Render();
		DotEffect::Render();


#if		!defined( ANTI_ALIASING_DEFAULT )
		//	レンダーターゲットを描画
		ShadingManager::RenderRenderTarget();
#endif

		// アイテムスロット描画
		ItemManager::Render2D();

		// イベント描画(2D)
		//EventManager::Render2D();

	
		//	スロットボタン
		slotButton->Render();

		Render2D();
		// 左右にある名前、コイン情報表示
		PlayerGameInfoCard::RenderPlayerCard();
		if( GameMasterClientA::GetMyTankState() == PLAYER_STATE::COMBAT )
			PlayerEffectCoin::Render();
		GameMasterClientA::RenderCenterCardImage();
		GameMasterClientA::RenderGameInfo();

		if( curlocalState != PLAYER_STATE::DEATH ){
			//	ダメージガイド描画
			DamageGuide::Render();
	
			//	ロックオンカーソル描画
			LockOnCursor::Render();

			//	照準描画
			if( tank[ SOCKET_MANAGER->GetID() ]->GetBulletType() !=	 BULLET_TYPE_GRENADE ){
				Alignment::RenderProcForPlayer( SOCKET_MANAGER->GetID() );
			}

			//	ミニマップ描画
			MiniMap::Render();

			//	ＨＰゲージ描画
			PlayerHPGageType2::Render();
			ReloadGauge::Render( SOCKET_MANAGER->GetID() );
		}

		//	ゲームログ描画
		GameLog::Render();

		//	チャットＵＩ描画
		ChatBox::Render();
		ChatLog::Render();

		if( GameMasterClientA::GetMyTankState() != PLAYER_STATE::DEATH ){
			//	ダメージフィルター描画
			DamageFilter::Render();
		}

		if( curlocalState != PLAYER_STATE::DEATH ){
			//サイト表示
			static iex2DObj* site = new iex2DObj("DATA\\Interface\\site.png");
			site->Render(2, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight);
		}
		
		//	合体演出描画
		m_union.Render();

		//	終了文字表示テスト
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

	/*フレームワーク*/
	DWORD dwFrameTime( clock() );
	while(true)
	{
		//	スレッド終了
		if( *isEnd )
			return;

		//	フレーム制限	
		DWORD CurrentTime = clock()*10;
		if( CurrentTime < dwFrameTime+167 ) 
			continue;

		//	経過時間
		DWORD	dTime = CurrentTime - dwFrameTime;
		if( dTime > 2000 ) dwFrameTime = CurrentTime;
		dwFrameTime += 167;

		//	ネットワーク更新
		SOCKET_MANAGER->InitUser();
		SOCKET_MANAGER->UpdateUser();
		//SOCKET_MANAGER->UpdateEvent();

		//	アイテム更新
		SOCKET_MANAGER->ReceiveGetItemInfo();	//	取得時のイベント用
		SOCKET_MANAGER->ReceiveItem();			//	効果反映用

		//	チャット更新
		std::string str = ChatBox::GetLog();

		if( str.size() > 0 ){
			SOCKET_MANAGER->SendChat(str);
		}

		SOCKET_MANAGER->ReceiveChat();

		//	ログ更新
		SOCKET_MANAGER->ReceiveLog();
	}
}


//==============================================================================
//	methods
//==============================================================================
void	sceneMutch::UpdateServerInfo()
{
//	GameMasterClientA::UpdateServerInfo();
//	上記に関数の中身を移動
}
void	sceneMutch::UpdateTanks()
{
	//　自分の描画と相手の描画を分ける
	if( curlocalState != DEATH )
		tank[ GameMasterClientA::GetMyID() ]->Update();//自分

	for( int i = 0; i < PANZER_MAX; i++ )
	{	
		if( i == GameMasterClientA::GetMyID() ) continue;
		tank[ i ]->Update();//相手
	}
}
 
//*****************************************************************************************************************************
//	ステートマシン : ゲームメイン
//*****************************************************************************************************************************
void	sceneMutch::UpdateCombatTank()
{
	//	カメラ更新
	Camera::Update( InfoBuffer::GetPlayerList()[SOCKET_MANAGER->GetID()]->GetPos() );
	
	//	瀕死時の画面赤化
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

			//	周期計算
			int		interval	=	( int )( c_MaxInterval + ( c_MinInterval - c_MaxInterval ) * ( 1.0f - percentage / c_WarningLine ) + 0.5f );

			//	カウンター更新
			++s_Counter;

#ifdef	USE_SOUND_EFFECT
			//	警告音再生
			if( s_Counter >= interval ){
				//SoundManager::Play( "警告音", false, 1.0f, 0.2f );
			}
#endif

			//	フィルターセット＆カウンターリセット
			if( s_Counter >= interval ){
				DamageFilter::Set( interval, 0.5f );
				DamageFilter::Set( interval * 2, 0.25f );
				s_Counter	=	0;
			}
		}
	}

	//	ステートを更新（キー入力又は時間経過でリスポーンへ）
	{
		static	const	int		c_WaitTime		=	120;
		static			int		s_WaitCounter	=	0;

		if( GameMasterClientA::GetMyTankState() != PLAYER_STATE::COMBAT ){
			//	待機判定
			if( ++s_WaitCounter > c_WaitTime
			||	InputManager::GetKeyState( DIK_SPACE ) == KEY_STATE::KEY_STATE_PRESSED ){
				//	シーンの状態を更新
				curlocalState 	=	GameMasterClientA::GetMyTankState();

				//	カウンターをリセット
				s_WaitCounter	=	0;
			}

			//	照準無効化
			Alignment::Disactivate();
			BallisticGuide::Disactivate();

			FrontGuide::Clear();
		}
	}

	//	パラメータをクリア（死亡したと判断）
	if( curlocalState != COMBAT ){
		//	戦車のパラメータをクリア
		tank[ SOCKET_MANAGER->GetID() ]->Clear();

		//	インターフェイスを無効化
		ReloadGauge::Clear();
		MiniMap::Clear();
		PlayerHPGageType2::Clear();
	}
}
void	sceneMutch::SetUpCombatCamera()
{
	//	画面をクリア
	Camera::Clear( SkyColor );
	//	描画パラメータを設定
	{
		ShadingManager::SetEyePosition( Camera::GetPos() );
		ShadingManager::SetEyeGaze( Camera::GetGaze() );
	}

#ifdef	USE_SOUND_EFFECT
	//	リスナーの情報を更新
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
//	ステートマシン : スロット操作
//*****************************************************************************************************************************
BYTE	sceneMutch::SelectSlot()
{	
	bool	isClick					=	InputManager::GetMouseButton(0) == KEY_STATE_PRESSED
									||	GetPadKeyDecision();
	if( isClick == false )	return 0xFF;
		
	POINT			  cursorPos		= InputManager::GetMouseCursorCoord();
	Global::TEAM_NAME myTeam		= GameMasterClientA::GetMyTeam();

	//　マウスをを３Ｄ座標に変換する行列作成
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

	// マウスの３Ｄ座標からray方向決定
	Vector3 lookAt;
	D3DXVECTOR3	mousePosNear, mousePosFar;
	Vector3		camPos = makeCamera.GetPos();
	D3DXVec3TransformCoord( &mousePosNear, &D3DXVECTOR3((float)cursorPos.x,(float)cursorPos.y, 0.0f), &invViewProjTrans );
	D3DXVec3TransformCoord( &mousePosFar,  &D3DXVECTOR3((float)cursorPos.x,(float)cursorPos.y, 1.0f), &invViewProjTrans );

	lookAt.x = mousePosFar.x - mousePosNear.x;
	lookAt.y = mousePosFar.y - mousePosNear.y;
	lookAt.z = mousePosFar.z - mousePosNear.z;
	lookAt.Normalize();

	// スロットとの交差判定
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
			//	効果音再生
			SoundManager::Play( "決定音", false, 1.0f, 0.075f );
#endif

			return i;
		}
	}

	return  0xFF;
}
void	sceneMutch::UpdateDeathTank()
{
	//	パッドによるマウスカーソルの操作
	UpdateCursorCoordAtPadAxis( 0.2f, 10.0f );

	// 復活までのステップ処理
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

		// 要求としてサーバーに送る
		GameMasterClientA::SetSendSelectSlot( select );
		if( select != 0xFF )
		{
			GameMasterClientA::SetSlotState( SLOT_STATE::SERVER_ENQUIRY );	
			selectSlotTimer = 0;			
		}
		else
		{
			// 選択していないことをサーバーに送る
			GameMasterClientA::SetSendSelectSlot( 0xFF );
		}
	}
	break;
 	case SERVER_ENQUIRY:// サーバーに問合せ中...
	{		
	}
	break;
	case PLAY:
	{
		// ボタン入力開始			
		BYTE playSlot		= GameMasterClientA::GetSelectSlot();				// 使用中スロット取得
		BYTE curReqwst		= GameMasterClientA::GetReqest();					// 現在の状態取得
		PLAYER_STATE state	= GameMasterClientA::GetMyTankState();
		slotButton->Activate(playSlot);
		GameMasterClientA::GetSlot(playSlot)->Activate();
		if( state == RESPAWN )
		{
			slotButton->DeActivate();
			GameMasterClientA::GetSlot(playSlot)->DeActivate();			
			GameMasterClientA::SendReqestClear();								// 要求情報クリア
			GameMasterClientA::SetSlotState( UNION );							// 次のステップへ
		}
	}
	break;
	case UNION:
	{
		// ボタン入力終了
		slotButton->DeActivate();

		BYTE	select	= GameMasterClientA::GetSelectSlot();
		Vector3	slotPos = GameMasterClientA::GetSlot(select)->GetPos();
		if( UnionTank(slotPos) == true )
		{
			//	戦車復活処理
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
		//	カメラセット
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

	//	描画パラメータを設定
	{
		ShadingManager::SetEyePosition( makeCamera.GetPos() );
		ShadingManager::SetEyeGaze( makeCamera.GetTarget() );
	}

#ifdef	USE_SOUND_EFFECT
	//	リスナーの情報を更新
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
//	ステートマシン : 復活( 無敵時間中 
//*****************************************************************************************************************************
void	sceneMutch::UpdateRespawnTank()
{
	//	カメラ更新
	Camera::Update( InfoBuffer::GetPlayerList()[SOCKET_MANAGER->GetID()]->GetPos() );
	//	ステートを更新
	curlocalState = GameMasterClientA::GetMyTankState();
}
//	
void	sceneMutch::Render2D()
{
	//	頭上にＨＰバーを表示（描画予約のみ）
	{
		//	描画予約
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


	//	頭上に名前を表示（描画予約のみ）
	{
		//	自分のチームIDを取得
		Global::TEAM_NAME	myTeam		=	GameMasterClientA::GetPlayerTeam( SOCKET_MANAGER->GetID() );

		//	チームＩＤ取得
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
		//	描画パラメータ
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

		//	描画
		for( int i = 0; i < PANZER_MAX; i++ ){
			if( i == SOCKET_MANAGER->GetID() )	continue;
			if( tank[i]->IsAlive() == false )	continue;
			LPCSTR	drawStr		=	c_Param[i].name;
			Vector3	drawPos		=	tank[i]->GetPos();
					drawPos.y	+=	30.0f;

			NamePlate::RenderReserve( drawPos, drawStr, c_Param[i].fillColor, c_Param[i].edgeColor );
		}
	}

	//	頭上に賞金額表示（描画予約のみ）
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
//  タンク作成演出関数
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

		//	落下中
		if( m_union.value < moveValue - psnzerHeight )
		{		
			tank[ id ]->DownfallParts( m_union.nowParts, downSpeed );
		}
		else
		{
			//	エフェクトをセット
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
			//	合体音再生
			{
				bool	loop		=	false;
				float	pitch		=	0.9f + 0.2f * ( rand() / ( float )RAND_MAX );
				float	gain		=	0.3f;

				if( m_union.nowParts == 0 )	SoundManager::Play( "合体音２", loop, pitch, gain );
				else						SoundManager::Play( "合体音", loop, pitch, gain );
			}
#endif

			m_union.nowParts++;
			m_union.value = 0;
		}


		//	合体終了
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
			SoundManager::Play( "合体音３" );
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


		//	戦車復活
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
