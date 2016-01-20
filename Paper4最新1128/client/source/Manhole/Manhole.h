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
		Vector3 pos;
	}data[2][6];

	int CheckManhole(LAND_TYPE type, const Vector3 &pos, float dist);

private:
	ManholeMng();
	ManholeMng(const ManholeMng&){}
	ManholeMng &operator=(const ManholeMng&){}

	void Load(char *filename);

};

#define manhole_mng ( ManholeMng::GetInstance() )