
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
//		�X�V
//*************************************************************************************************************************
//���������@�{�^������������΂���ver
void	NetPlayer::Update( void )
{
	//	���N���A
	m_controlDesc	=	GetDefaultDesc();

	//	����p�����[�^��ݒ�
	CONTROL_DESC	controlDesc;
	{
		controlDesc.fire		=	m_isFire;
		controlDesc.acceleMode	=	TankController::ACCELE_MODE::ACCELE_MODE_STAY;
		controlDesc.bodyRot		=	0.0f;
		controlDesc.headRot		=	0;
		controlDesc.cannonRot	=	0;
	}

	//	��Ԃ̌̃p�����[�^
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

	//	���˃^�C�v
	FIRE_MODE	fireMode			=	GetFireMode();
	
	//	�ˌ��J�n
	if( controlDesc.fire ){
		//	�����[�h���������Ă��邩�`�F�b�N
		if( m_ReloadCounter >= (int)( baseReloadTime / reloadMag + 0.5f ) ){
			//	�ˌ��J�E���^�[�Z�b�g
			m_BurstCounter			=	numBurst;
			//	�C���^�[�o���J�E���^�[������
			m_BurstIntervalCounter	=	0;

			//	�����[�h�J�E���^�[�Z�b�g
			m_ReloadCounter			=	0;

			m_isFire				=	true;
		}
	}
	//	�����[�h
	if( ++m_ReloadCounter >= (int)( baseReloadTime / reloadMag + 0.5f ) )
	{
		m_ReloadCounter	=	(int)( baseReloadTime / reloadMag + 0.5f );
		m_LoadedAmmo	=	numBurst;

		m_isFire		=	false;
	}

	//	���C
	if( m_BurstCounter > 0 ){
		//	�C���^�[�o���`�F�b�N
		if( --m_BurstIntervalCounter <= 0 ){
			//	����
			Fire( bulletSpeed, bulletFiringRange, fireMode, soundID, pitchAmplitude, fireSoundGain );
			//	���˃J�E���^�[�����炷
			--m_BurstCounter;
			//	���U����Ă���e�̐������炷
			--m_LoadedAmmo;

			//	�C���^�[�o���J�E���^�[�����Z�b�g
			if( m_BurstCounter > 0 )	m_BurstIntervalCounter	=	burstInterval;
		}
	}

	//	�^���N���X�V
	Tank::Update();

	//	���t���[���̈ړ��ʂ�ۑ�
	m_PrevMoveAmount	=	m_Pos - m_PrevPos;
	//	���̍��W��ۑ�
	m_PrevPos			=	m_Pos;
	//	���t���[���̉�]�ʂ�ۑ�
	m_PrevAngleAmount	=	m_Angle - m_PrevAngle;
	//	���̊p�x��ۑ�
	m_PrevAngle			=	m_Angle;

	//	���C��G�t�F�N�g
	if( m_PrevMoveAmount.Length()	> advancedSpeed / 5.0f
	||	abs( m_PrevAngleAmount )	> bodyRotSpeed  / 5.0f ){
		FloorRuddingEffect::Set( this );
	}

#ifdef	USE_SOUND_EFFECT
	//	�G���W������炷
	{
		static	const	float	c_MaxGain	=	0.25f;
		static	const	float	c_MinGain	=	0.2f;

		float	perSpeed	=	abs( m_PrevMoveAmount.Length() ) / advancedSpeed;

		float	gain		=	c_MinGain + ( c_MaxGain - c_MinGain ) * perSpeed;
		if( gain > c_MaxGain )	gain	=	c_MaxGain;
		if( gain < c_MinGain )	gain	=	c_MinGain;

		//	���ʒ���
		gain	*=	20.0f;

		//	���ʍX�V
		m_EngineGain		+=	( gain - m_EngineGain ) * 0.5f;

		//	�s�b�`�X�V
		float	pitch		=	enginePitchMin + ( enginePitchMax - enginePitchMin ) * perSpeed;
		m_EnginePitch		+=	( pitch - m_EnginePitch ) * 0.5f;

		SoundManager::SetGain( m_EngineSourceID, m_EngineGain );
		SoundManager::SetPitch( m_EngineSourceID, m_EnginePitch );
		SoundManager::SetPosition( m_EngineSourceID, m_Pos );
	}
#endif
}