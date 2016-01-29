#pragma once

class FanMng
{
public:
	~FanMng();

	static FanMng *GetInstance()
	{
		static FanMng i;
		return &i;
	}

	int num_fan;
	Vector3 *fan_pos;

	int CheckFan(float dist, const Vector3 &pos);
	bool CheckHeight(int no, float dist, float pos_y);

private:
	FanMng();
	FanMng(const FanMng&){}
	FanMng &operator=(const FanMng&){}

	void Load(char *filename);
};

#define Fan_mng ( FanMng::GetInstance() )