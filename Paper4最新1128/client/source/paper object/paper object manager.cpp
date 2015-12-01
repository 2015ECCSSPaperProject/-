
#include <vector>
#include "iextreme.h"
#include "paper object manager.h"

#include "paper object.h"

Paper_obj_mng::Paper_obj_mng() : original_model(nullptr), number_of_objects(0)
{
	obj_array.clear();
}

Paper_obj_mng::~Paper_obj_mng()
{
	delete[] original_model;
	for (unsigned i = 0; i < obj_array.size(); i++) delete obj_array[i];
}

void Paper_obj_mng::Initialize()
{
	Load();
}

void Paper_obj_mng::Release()
{
	delete[] original_model; original_model = nullptr;
	for (unsigned i = 0; i < obj_array.size(); i++) delete obj_array[i];
	obj_array.clear();
}

void Paper_obj_mng::Update()
{
	for (auto it = obj_array.begin(); it != obj_array.end(); it++)
	{
		(*it)->Update();
	}
}

void Paper_obj_mng::Render()
{
	for (auto it : obj_array)
	{
		it->Render();
	}
}

//**************************************************

int Paper_obj_mng::Get_numof()
{
	return number_of_objects;
}

void Paper_obj_mng::Set_user(int index, int number)
{
	obj_array[index]->Set_user(number);
}

void Paper_obj_mng::Set_animframe(int index, int frame)
{
	obj_array[index]->Set_animframe(frame);
}

const Vector3& Paper_obj_mng::Get_pos(int index)
{
	return obj_array[index]->Get_pos();
}

//**************************************************

#include "poster/Poster.h"
#include "test_box/test box.h"
#include "../fstream/fstream_paper.h"
#include "IEX_Math2.h"

void Paper_obj_mng::Load()
{
	// 仮

	number_of_objects = 0;

	char *filename;

	filename = "DATA/MATI/poster_pos.txt";

	std::ifstream infs(filename);

	//**************************************************
	/// 3DObjのファイルをロード
	//**************************************************
	//char work[256];

	unsigned int num_models = 2;							// モデルの種類
	original_model = new iex3DObj[num_models];
	for (unsigned i = 0; i < num_models; i++)
	{
		// ロードしたい
		original_model[0].LoadObject("DATA/Poster/posuta-.IEM");
		original_model[1].LoadObject("DATA/TEST/box.IEM");
		break;
	}

	// ポスターの位置とか
	Paper_obj *p(nullptr);
	float angle(0);
	Vector3 pos(0, 0, 0);
	int point(0);
	while (!infs.eof())
	{
		infs >> angle;
		angle = Degree_to_radian(angle);
		infs >> pos;
		infs >> point;
		p = new Poster;
		p->Initialize(&original_model[0]);
		p->Set_pose(angle, pos);
		obj_array.push_back(p);
		this->number_of_objects++;
	}

	for (int i = 0; i < 10; i++)
	{
		p = new Test_box;
		p->Initialize(&original_model[1]);
		p->Set_pose(0, Vector3(i * 20.0f - 100.0f, 0.0f, 50.0f));
		obj_array.push_back(p);
		this->number_of_objects++;
	}
}

Paper_obj_mng *paper_obj_mng = nullptr;
