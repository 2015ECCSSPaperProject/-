#include"iextreme.h"
#include<vector>
#include"Ambulance.h"
#include"../sound/SoundManager.h"
#include"../fstream/fstream_paper.h"
//**************************************************************************************************************
//
//		救	急	車	ク	ラ	ス
//
//**************************************************************************************************************

//=============================================================================================
//		初	期	化
Ambulance::Ambulance() :pos(Vector3(0, 0, 0)), goal_pos(Vector3(0, 0, 0)), angle(0), obj(nullptr), se_receive(0), move(Vector3(0, 0, 0)), scale(1.0f)
{
	MoveFunk[(int)MOVE_TYPE::ROUND_TRIP] = &Ambulance::MoveRoundTrip;
	MoveFunk[(int)MOVE_TYPE::AROUND_OUTER] = &Ambulance::MoveAroundOuter;
}
void Ambulance::Initialize(iex3DObj *clone, Ambulance::MOVE_TYPE type, const Vector3 &start, const Vector3 &goal, float speed)
{
	obj = clone->Clone();
	pos = start_pos = start;
	goal_pos = goal;
	angle = atan2(goal.x - start.x, goal.z - start.z);
	move_type = type;
	MOVE_LENGTH = (pos - goal_pos).Length();
	this->speed = speed;

	se_receive = se->Play("救急車", pos, Vector3(sinf(angle), 0, cosf(angle)), Vector3(0, 0, 0), true);
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
	
	obj->Animation();
	obj->SetPos(pos);
	obj->SetAngle(angle);
	obj->SetScale(scale);

	// 音源情報更新(ドップラーの肝)
	se->Set_param("救急車", se_receive, pos, move);
}


//=============================================================================================
//		描		画
void Ambulance::Render(iexShader *shader, char *name)
{
	obj->Update();
	(shader) ? obj->Render(shader, name) : obj->Render();

#ifdef _DEBUG
	//if (move_type == MOVE_TYPE::AROUND_OUTER)
	//{
	//	Text::Draw(320, 320, 0xffffff00, "X:%.2f", pos.x);
	//	Text::Draw(320, 340, 0xffffff00, "Z:%.2f", pos.z);
	//}
#endif
}

//=============================================================================================
//		移動	関数
void Ambulance::MoveRoundTrip()
{
	move.x = sinf(angle)*speed;
	move.z = cosf(angle)*speed;
	pos += move;

	Vector3 v = goal_pos - pos;
	v.Normalize();
	if (sinf(angle)*v.x + cosf(angle)*v.z < 0)	// 今の座標→目標座標のベクトルと向きベクトルの内積で目標を通りすぎたかの判定。
	{
		Vector3 temp(start_pos);
		start_pos = goal_pos;
		goal_pos = temp;
		angle += PI;
	}
}

void Ambulance::MoveAroundOuter()
{
	move.x = sinf(angle)*speed;
	move.z = cosf(angle)*speed;
	pos += move;

	Vector3 v = goal_pos - pos;
	v.Normalize();
	if (sinf(angle)*v.x + cosf(angle)*v.z <= 0)	// 今の座標→目標座標のベクトルと向きベクトルの内積で目標を通りすぎたかの判定。
	{
		pos = goal_pos;
		angle += 1.0471975511966f;	// 60°回転
		goal_pos.x = pos.x + sinf(angle)*MOVE_LENGTH;
		goal_pos.z = pos.z + cosf(angle)*MOVE_LENGTH;
	}
}


//=============================================================================================
//		ゲッターとか
Vector3 Ambulance::GetPos()
{
	return pos;
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
	Load("DATA/Ambulance/set.txt");
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

void AmbulanceMng::Load(LPSTR filename)
{
	std::ifstream infs(filename);
	// 位置とか
	Vector3 pos(0, 0, 0);
	Vector3 goal_pos(0, 0, 0);
	int type(0);
	while (!infs.eof())
	{
		// 移動タイプ
		infs >> type;
		// 位置
		infs >> pos;
		infs >> goal_pos;
		// リストにプッシュ
		this->Append((Ambulance::MOVE_TYPE)type, pos, goal_pos, 4.0f);
	}
	infs.close();
}

//=============================================================================================
//		リスト追加
void AmbulanceMng::Append(Ambulance::MOVE_TYPE type, const Vector3 &start, const Vector3 &goal, float speed)
{
	Ambulance *set = new Ambulance;
	set->Initialize(clone, type, start, goal, speed);
	List.push_back(set);
}



//=============================================================================================
//		判定系
int AmbulanceMng::CheckDist(const Vector3 &pos, float dist)
{
	for (UINT i = 0; i < List.size(); i++)
	{
		if ((List[i]->GetPos() - pos).LengthSq() < dist*dist) return i;
	}

	return -1;
}

Ambulance *AmbulanceMng::GetAmbulance(int no)
{
	assert((UINT)no < List.size() || (UINT)no >= 0);
	return List[no];
}

AmbulanceMng *ambulance_mng;