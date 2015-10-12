
#include	"iextreme.h"
#include	"system/system.h"

#include	"Item.h"

#include	"Tank.h"
#include	"TankController.h"

#include	"Player.h"
#include	"NetPlayer.h"

#include	"InformationBuffer.h"

#include	"Effect.h"
#include	"SoundManager.h"

NetPlayer::NetPlayer( void ):Player()
{
	m_PrevAngle			=	0.0f;
	m_PrevAngleAmount	=	0.0f;
	m_EngineGain		=	0.0f;
	m_EnginePitch		=	1.0f;
}


//*************************************************************************************************************************
//		更新
//*************************************************************************************************************************
//■■■★　ボタン押す流れを飛ばしたver
void	NetPlayer::Update( void )
{
	//	情報クリア
	m_controlDesc	=	GetDefaultDesc();

	//	操作パラメータを設定
	CONTROL_DESC	controlDesc;
	{
		controlDesc.fire		=	m_isFire;
		controlDesc.acceleMode	=	TankController::ACCELE_MODE::ACCELE_MODE_STAY;
		controlDesc.bodyRot		=	0.0f;
		controlDesc.headRot		=	0;
		controlDesc.cannonRot	=	0;
	}

	//	戦車の個体パラメータ
	float	baseReloadTime			=	GetBaseReloadTime();
	float	reloadMag				=	GetReloadMag();

	float	advancedSpeed			=	GetAdvancedSpeed();
	float	backSpeed				=	GetBackSpeed();

	float	brakePower				=	GetBrakePower();

	float	rotAttenuance			=	GetRotAttenuance();
	float	rotAttenuancePower		=	GetRotAttenuancePower();

	float	acceleFrame				=	GetAcceleFrame();
	float	acceleFrameBack			=	GetAcceleFrameBack();

	float	bodyRotSpeed			=	GetBodyRotSpeed();

	float	headAngleLimit			=	GetHeadAngleLimit();
	float	headRotSpeed			=	GetHeadRotSpeed();

	float	cannonOverAngleLimit	=	GetCannonOverAngleLimit();
	float	cannonUnderAngleLimit	=	GetCannonUnderAngleLimit();
	float	cannonRotSpeed			=	GetCannonRotSpeed();

	float	bulletFiringRange		=	GetBulletFiringRange();
	float	bulletSpeed				=	GetBulletSpeed();

	int		numBurst				=	GetNumBurst();
	int		burstInterval			=	GetBurstInterval();

	LPCSTR	soundID					=	GetSoundID();
	float	pitchAmplitude			=	GetPitchAmplitude();
	float	fireSoundGain			=	GetFireSoundGain();

	float	enginePitchMax			=	GetEnginePitchMax();
	float	enginePitchMin			=	GetEnginePitchMin();

	//	発射タイプ
	FIRE_MODE	fireMode			=	GetFireMode();
	
	//	射撃開始
	if( controlDesc.fire ){
		//	リロードが完了しているかチェック
		if( m_ReloadCounter >= (int)( baseReloadTime / reloadMag + 0.5f ) ){
			//	射撃カウンターセット
			m_BurstCounter			=	numBurst;
			//	インターバルカウンター初期化
			m_BurstIntervalCounter	=	0;

			//	リロードカウンターセット
			m_ReloadCounter			=	0;

			m_isFire				=	true;
		}
	}
	//	リロード
	if( ++m_ReloadCounter >= (int)( baseReloadTime / reloadMag + 0.5f ) )
	{
		m_ReloadCounter	=	(int)( baseReloadTime / reloadMag + 0.5f );
		m_LoadedAmmo	=	numBurst;

		m_isFire		=	false;
	}

	//	発砲
	if( m_BurstCounter > 0 ){
		//	インターバルチェック
		if( --m_BurstIntervalCounter <= 0 ){
			//	発射
			Fire( bulletSpeed, bulletFiringRange, fireMode, soundID, pitchAmplitude, fireSoundGain );
			//	発射カウンターを減らす
			--m_BurstCounter;
			//	装填されている弾の数を減らす
			--m_LoadedAmmo;

			//	インターバルカウンターをリセット
			if( m_BurstCounter > 0 )	m_BurstIntervalCounter	=	burstInterval;
		}
	}

	//	タンクを更新
	Tank::Update();

	//	今フレームの移動量を保存
	m_PrevMoveAmount	=	m_Pos - m_PrevPos;
	//	今の座標を保存
	m_PrevPos			=	m_Pos;
	//	今フレームの回転量を保存
	m_PrevAngleAmount	=	m_Angle - m_PrevAngle;
	//	今の角度を保存
	m_PrevAngle			=	m_Angle;

	//	床擦りエフェクト
	if( m_PrevMoveAmount.Length()	> advancedSpeed / 5.0f
	||	abs( m_PrevAngleAmount )	> bodyRotSpeed  / 5.0f ){
		FloorRuddingEffect::Set( this );
	}

#ifdef	USE_SOUND_EFFECT
	//	エンジン音を鳴らす
	{
		static	const	float	c_MaxGain	=	0.25f;
		static	const	float	c_MinGain	=	0.2f;

		float	perSpeed	=	abs( m_PrevMoveAmount.Length() ) / advancedSpeed;

		float	gain		=	c_MinGain + ( c_MaxGain - c_MinGain ) * perSpeed;
		if( gain > c_MaxGain )	gain	=	c_MaxGain;
		if( gain < c_MinGain )	gain	=	c_MinGain;

		//	音量調整
		gain	*=	20.0f;

		//	音量更新
		m_EngineGain		+=	( gain - m_EngineGain ) * 0.5f;

		//	ピッチ更新
		float	pitch		=	enginePitchMin + ( enginePitchMax - enginePitchMin ) * perSpeed;
		m_EnginePitch		+=	( pitch - m_EnginePitch ) * 0.5f;

		SoundManager::SetGain( m_EngineSourceID, m_EngineGain );
		SoundManager::SetPitch( m_EngineSourceID, m_EnginePitch );
		SoundManager::SetPosition( m_EngineSourceID, m_Pos );
	}
#endif
}