#include	"iextreme.h"
#include	"system/system.h"
#include	"sceneMain.h"

#include "../player/Player_manager.h"
#include "../poster/Poster_manager.h"
#include "../stage/Stage.h"
#include "../camera/Camera.h"
#include "../Enum_public.h"
#include "../score/Score.h"

//*****************************************************************************************************************************
//
//	������
//
//*****************************************************************************************************************************
bool sceneMain::Initialize()
{
	//	���ݒ�
	iexLight::SetFog( 800, 1000, 0 );


	//	���s���ݒ�							
	Vector3	 dir ( 1.0f, -1.0f, 1.0f );
	iexLight::DirLight ( 0, &dir, 1.0f, 1.0f, 1.0f );
	//	�����ݒ�							
	iexLight::SetAmbient ( 0x888888 );

	score = new Score;

	poster_mng = new Poster_manager;
	poster_mng->Initialize(score);

	player_mng = new Player_manager;
	player_mng->Initialize(poster_mng);	// �����ǉ����܂���

	stage = new Stage;
	stage->Initialize();

	camera = new Camera;
	camera->Set(Vector3(0, 10.0f, -20.0f), Vector3(0, 0, 0));
	camera->Initialize(player_mng);

	return true;
}

sceneMain::~sceneMain()
{
	delete score;
	delete player_mng;
	delete poster_mng;
	delete stage;

	delete	camera;
}

//*****************************************************************************************************************************
//
//		�X�V
//
//*****************************************************************************************************************************

void	sceneMain::Update()
{
	player_mng->Update(stage, poster_mng);
	poster_mng->Update();
	stage->Update();

	camera->Update();
}

//*****************************************************************************************************************************
//
//		�`��֘A
//
//*****************************************************************************************************************************

void	sceneMain::Render()
{
	//	��ʃN���A
	camera->Activate();
	camera->Clear();

	//	�X�e�[�W
	player_mng->Render();
	poster_mng->Render();
	stage->Render();

	score->Render();
}
