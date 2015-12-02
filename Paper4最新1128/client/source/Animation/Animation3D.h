#pragma once
/*３Dアニメーションクラス*/

class Animation3D
{
private:
	iex2DObj* handle;

	float width;//イラストのXのサイズ
	float Dist;//アニメの間隔サイズ
	int ANIMATION_MAX;//アニメーション数
	bool isAction;//アニメを実行するか

	int count;
	int flame;
	int ANIME_SPEED;//スピード

	bool isRoop;//ループしているか？

public:
	//画像の名前　その画像のX幅　切り取りサイズ　アニメーション数　アニメ速度　ループするか
	Animation3D(char* name, float XSize, float DIST, float MAX_ANIMATION, int SPEED, bool isRoop=false);
	~Animation3D();

	void Action();//アニメ実行
	void Stop();//アニメ止める

	void Update();
	void Render(Vector3 pos, float scale = 1.0f, u32 dwFlags=RS_ADD);
};

