
#pragma	once
#include	"system/Scene.h"
#include	"../system/State.h"
//*****************************************************************************************************************************
//
//		クラスの宣言
//
//*****************************************************************************************************************************
	//		シーン
	//*************************************************************************************************************************
	class	ServerCamera;
	class	sceneMutch	:	public Scene
	{
	private:
		ServerCamera*	m_pCamera;			//	カメラ

		bool			m_SkipRender;		//	描画をスキップするかどうか
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
	