//*****************************************************************************************************************************
//
//		���C���V�[��
//
//*****************************************************************************************************************************

class Camera;
class Player_manager;
class Poster_manager;
class Stage;
class Score;

class	sceneMain : public Scene
{
private:
	Score *score;

	BYTE my_number;

	Camera *camera;
	Player_manager *player_mng;
	Poster_manager *poster_mng;
	Stage *stage;

public:
	~sceneMain();
	//	������
	bool Initialize();
	//	�X�V�E�`��
	void Update();	//	�X�V
	void Render();	//	�`��
};


