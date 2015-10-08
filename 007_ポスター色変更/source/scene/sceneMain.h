//*****************************************************************************************************************************
//
//		メインシーン
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
	//	初期化
	bool Initialize();
	//	更新・描画
	void Update();	//	更新
	void Render();	//	描画
};


