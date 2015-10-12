
#pragma	once
#include	"system/Scene.h"
#include	"../system/State.h"
//*****************************************************************************************************************************
//
//		�N���X�̐錾
//
//*****************************************************************************************************************************
	//		�V�[��
	//*************************************************************************************************************************
	class	ServerCamera;
	class	sceneMutch	:	public Scene
	{
	private:
		ServerCamera*	m_pCamera;			//	�J����

		bool			m_SkipRender;		//	�`����X�L�b�v���邩�ǂ���
	private:
		void	DisplayPlayerData( void );
		void	DisplayCameraGuide( void );
	public:
		~sceneMutch( void );
		bool	Initialize( void );
		void	CombatTank( int id );
		void	DeathTank( int id );
		void	Update( void );
		void	Render( void );
	};
	