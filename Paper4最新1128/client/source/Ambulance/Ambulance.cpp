#include"iextreme.h"
#include<vector>
#include"Ambulance.h"

//**************************************************************************************************************
//
//		�~	�}	��	�N	��	�X
//
//**************************************************************************************************************

//=============================================================================================
//		��	��	��
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
//		��		��
Ambulance::~Ambulance()
{
	delete obj, obj = nullptr;
}


//=============================================================================================
//		�X		�V
void Ambulance::Update()
{
	(this->*MoveFunk[(int)move_type])();	// ���[�h����
}


//=============================================================================================
//		�`		��
void Ambulance::Render(iexShader *shader, char *name)
{
	(shader) ? obj->Render(shader, name) : obj->Render();
}

//=============================================================================================
//		�ړ�	�֐�
void Ambulance::MoveRoundTrip()
{

}

void Ambulance::MoveAroundOuter()
{

}





//**************************************************************************************************************
//
//		��	��	�N	��	�X
//
//**************************************************************************************************************

//=============================================================================================
//		��	��	��
AmbulanceMng::AmbulanceMng() :clone(nullptr)
{
	List.clear();
}
void AmbulanceMng::Initialize()
{
	clone = new iex3DObj("DATA/Ambulance/A��Ԃ���̗��j���l�܂������΂炵����/hako.IEM");
}

//=============================================================================================
//		��		��
AmbulanceMng::~AmbulanceMng()
{
	for (UINT i = 0; i < List.size(); i++) delete List[i];
	delete clone;
}

//=============================================================================================
//		�X		�V
void AmbulanceMng::Update()
{
	for (auto it = List.begin(); it != List.end(); it++)(*it)->Update();
}

//=============================================================================================
//		�`		��
void AmbulanceMng::Render(iexShader *shader, char *name)
{
	if (shader)for (auto it : List)it->Render(shader, name);
	else for (auto it : List)it->Render();
}



//=============================================================================================
//		���X�g�ǉ�
void AmbulanceMng::Append(Ambulance::MOVE_TYPE type, const Vector3 &start, const Vector3 &goal, float speed)
{
	Ambulance *set = new Ambulance;
	set->Initialize(clone, type, start, goal, speed);
	List.push_back(set);
}
