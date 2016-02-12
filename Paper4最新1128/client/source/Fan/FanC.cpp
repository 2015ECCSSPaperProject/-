#include "iextreme.h"
#include "FanC.h"
#include "FanEffect.h"
#include "../fstream/fstream_paper.h"
#include "../sound/SoundManager.h"
void FanMng::Load(char *filename)
{
	std::ifstream infs(filename);

	// IEMファイル名
	char iem_path[MAX_PATH];
	infs >> iem_path;

	Vector3 pos_array[32];
	num_fan = 0;

	while (!infs.eof())
	{
		infs >> pos_array[num_fan];
		float yomitobasi(0);
		infs >> yomitobasi;
		num_fan++;
	}
	infs.close();

	clone = new iex3DObj(iem_path);
	clone->SetMotion(0);
	clone->SetScale(1.0f);
	clone->SetAngle(0);

	obj = new iex3DObj*[num_fan];
	fan_pos = new Vector3[num_fan];
	fan_effect = new FanEffect*[num_fan];


	// 位置とか
	for (int i = 0; i < num_fan; i++)
	{
		fan_pos[i] = pos_array[i];
		fan_effect[i] = new FanEffect;
		obj[i] = clone->Clone();
	}
}

void FanMng::Initialize()
{
	Load("DATA/Fan/set.txt");
}

void FanMng::Release()
{
	delete fan_pos;
	for (int i = 0; i < num_fan; i++)
	{
		delete fan_effect[i];
		delete obj[i];
	}
	delete[] fan_effect;
	delete[] obj;
	delete clone;
}

void FanMng::Update()
{
	for (int i = 0; i < num_fan; i++)
	{
		fan_effect[i]->Update(fan_pos[i], .25f);
		obj[i]->Animation();
		obj[i]->SetPos(fan_pos[i]);
		obj[i]->Update();
	}
}

void FanMng::SoundON()
{
	for (int i = 0; i < num_fan; i++)
	{
		se->Play("扇風機", fan_pos[i], Vector3(0, 0, 0), Vector3(0, 0, 0), true);
	}
}

void FanMng::RenderD(iexShader *shader, char *name)
{
	if (shader)
	{
		for (int i = 0; i < num_fan; i++)
		{
			obj[i]->Render(shader, name);
		}
	}
	else
	{
		for (int i = 0; i < num_fan; i++)
		{
			obj[i]->Render();
		}
	}
}

void FanMng::RenderF()
{
	for (int i = 0; i < num_fan; i++)
	{
		fan_effect[i]->Render();
	}
}

int FanMng::CheckFan(const Vector3 &pos, float dist)
{
	for (int i = 0; i < num_fan; i++)
	{
		// xとzでとる
		if ((pos.x - fan_pos[i].x)*(pos.x - fan_pos[i].x) + (pos.z - fan_pos[i].z)*(pos.z - fan_pos[i].z) < dist*dist)
		{
			return i;
		}
	}
	return -1;
}