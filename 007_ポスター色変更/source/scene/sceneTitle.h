class	sceneTitle : public Scene
{
private:
	iex2DObj* title ;	//　タイトル画像
public:
	~sceneTitle();
	//	初期化
	bool Initialize();
	//	更新・描画
	void Update();	//	更新
	void Render();	//	描画
};
