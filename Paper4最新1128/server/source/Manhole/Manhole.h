#pragma once

class ManholeMng
{
public:
	static ManholeMng *GetInstance()
	{
		static ManholeMng i;
		return &i;
	}

	enum class LAND_TYPE
	{
		TIJOU, TIKA
	};

	struct
	{
		float angle;
		Vector3 pos;
	}data[2][6];

	bool CheckManhole(LAND_TYPE type, float dist, Vector3 *out_pos, float *out_angle, Vector3 *out_next_pos);

private:
	ManholeMng();
	ManholeMng(const ManholeMng&){}
	ManholeMng &operator=(const ManholeMng&){}

};

#define manhole_mng ( ManholeMng::GetInstance() )