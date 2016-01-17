#include"iextreme.h"
#include<vector>
#include"Ambulance.h"

//**************************************************************************************************************
//
//		救	急	車	ク	ラ	ス
//
//**************************************************************************************************************

//=============================================================================================
//		初	期	化
Ambulance::Ambulance() :pos(Vector3(0, 0, 0)), goal_pos(Vector3(0, 0, 0)), angle(0), obj(nullptr)
{
	MoveFunk[(int)MOVE_TYPE::ROUND_TRIP] = &Ambulance::MoveRoundTrip;
	MoveFunk[(int)MOVE_TYPE::AROUND_OUTER] = &Ambulance::MoveAroundOuter;
}
void Ambulance::Initialize(iex3DObj *clone, Ambulance::MOVE_TYPE type, const Vector3 &start, const Vector3 &goal, float speed)
{
	obj = clone->Clone();
	pos = start;
	goal_pos = goal;
	angle = atan2(goal.x - start.x, goal.z - start.z);
	move_type = type;
	this->speed = speed;
}

//=============================================================================================
//		解		放
Ambulance::~Ambulance()
{
	delete obj, obj = nullptr;
}


//=============================================================================================
//		更		新
void Ambulance::Update()
{
	(this->*MoveFunk[(int)move_type])();	// モード分岐
}


//=============================================================================================
//		描		画
void Ambulance::Render(iexShader *shader, char *name)
{
	(shader) ? obj->Render(shader, name) : obj->Render();
}

//=============================================================================================
//		移動	関数
void Ambulance::MoveRoundTrip()
{

}

void Ambulance::MoveAroundOuter()
{

}





//**************************************************************************************************************
//
//		管	理	ク	ラ	ス
//
//**************************************************************************************************************

//=============================================================================================
//		初	期	化
AmbulanceMng::AmbulanceMng() :clone(nullptr)
{
	List.clear();
}
void AmbulanceMng::Initialize()
{
	clone = new iex3DObj("DATA/Ambulance/A列車くんの歴史が詰まったすばらしい箱/hako.IEM");
}

//=============================================================================================
//		解		放
AmbulanceMng::~AmbulanceMng()
{
	for (UINT i = 0; i < List.size(); i++) delete List[i];
	delete clone;
}

//=============================================================================================
//		更		新
void AmbulanceMng::Update()
{
	for (auto it = List.begin(); it != List.end(); it++)(*it)->Update();
}

//=============================================================================================
//		描		画
void AmbulanceMng::Render(iexShader *shader, char *name)
{
	if (shader)for (auto it : List)it->Render(shader, name);
	else for (auto it : List)it->Render();
}



//=============================================================================================
//		リスト追加
void AmbulanceMng::Append(Ambulance::MOVE_TYPE type, const Vector3 &start, const Vector3 &goal, float speed)
{
	Ambulance *set = new Ambulance;
	set->Initialize(clone, type, start, goal, speed);
	List.push_back(set);
}
