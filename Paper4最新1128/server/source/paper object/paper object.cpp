
#include "iextreme.h"
#include "paper object.h"

#include "./../../../share_data/Enum_public.h"

Paper_obj::Paper_obj() : model_type(0), position(0, 0, 0), forward(0, 0, 1), angle(0), point(0)
{}

Paper_obj::~Paper_obj()
{}

void Paper_obj::Initialize(int model_type, int point)
{
	this->broken = false;
	this->model_type = model_type;
	this->point = point;
}

int Paper_obj::Get_point()
{
	return point;
}

int Paper_obj::Get_type()
{
	return model_type;
}

bool Paper_obj::Is_broken()
{
	return broken;
}

void Paper_obj::Rend()
{
	broken = true;
}

float Paper_obj::Get_angle()
{
	return angle;
}

const Vector3 &Paper_obj::Get_pos()
{
	return position;
}

void Paper_obj::Set_pose(const float angle, const Vector3& pos)
{
	forward.x = sinf(angle);
	forward.y = 0;
	forward.z = cosf(angle);

	this->angle = angle;
	this->position = pos;
}

void Paper_obj::Set_broken_false()
{
	broken = false;
}



void Paper_obj_Mesh::Initialize( int model_type, iexMesh *model, int point )
{
	Paper_obj::Initialize( model_type, point );
	this->model = model->Clone();
}

Paper_obj_Mesh::Paper_obj_Mesh( int model_type, iexMesh *model, int point )
{
	Initialize( model_type, model, point );
}

Paper_obj_Mesh::~Paper_obj_Mesh()
{
	delete model;
}

void Paper_obj_Mesh::Set_pose( const float angle, const Vector3& pos )
{
	Paper_obj::Set_pose( angle, pos );
	model->SetAngle( angle );
	model->SetPos( position );
}

Paper_obj_Mesh::Paper_obj_Mesh() : model( nullptr )
{}



Paper_obj_3DObj::Paper_obj_3DObj() : model( nullptr )
{}

Paper_obj_3DObj::Paper_obj_3DObj( int model_type, iex3DObj *model, int point )
{
	Initialize( model_type, model, point );
}

void Paper_obj_3DObj::Initialize( int model_type, iex3DObj *model, int point )
{
	Paper_obj::Initialize( model_type, point );
	this->model = model->Clone();
}

Paper_obj_3DObj::~Paper_obj_3DObj()
{
	delete model;
}

void Paper_obj_3DObj::Set_pose( const float angle, const Vector3& pos )
{
	Paper_obj::Set_pose( angle, pos );
	model->SetAngle( angle );
	model->SetPos( position );
}
