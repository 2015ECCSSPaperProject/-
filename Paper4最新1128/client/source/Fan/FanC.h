#pragma once

class FanEffect;

class FanMng
{
public:
	static FanMng *GetInstance()
	{
		static FanMng i;
		return &i;
	}

	int num_fan;
	Vector3 *fan_pos;

	int CheckFan(const Vector3 &pos, float dist);

	void Initialize();
	void Release();
	void Update();
	void RenderD(iexShader *shader=nullptr, char *name=nullptr);	// ディファー°
	void RenderF();	// フォワード

private:
	iex3DObj **obj;
	FanEffect **fan_effect;

	FanMng(){}
	FanMng(const FanMng&){}
	FanMng &operator=(const FanMng&){}

	void Load(char *filename);
};

#define Fan_mng ( FanMng::GetInstance() )