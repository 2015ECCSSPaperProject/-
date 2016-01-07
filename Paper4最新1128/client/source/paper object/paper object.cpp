
#include "iextreme.h"
#include "paper object.h"

Paper_obj::Paper_obj() : position(0, 0, 0), angle(0)
{}

Paper_obj::~Paper_obj()
{}

void Paper_obj::Set_pose(const float angle, const Vector3& pos)
{
	this->angle = angle;
	this->position = pos;
}

void Paper_obj::Set_user(int number)
{
	this->number = number;
}

const Vector3 &Paper_obj::Get_pos()
{
	return position;
}

void Paper_obj::Render_mark(iex2DObj *obj, bool target)
{
	if (number == PLAYER_MAX)
		return;

	float tu[2];
	if (target)
	{
		tu[0] = 1.0f;
		tu[1] = 0.5f;
	}
	else
	{
		tu[0] = 0;
		tu[1] = 0.5f;
	}
	float tv[2] = { 0, 1 };
	Billboard::Draw3D(position + Vector3(0, 22, 0), obj, 4, 4, tu, tv, RS_COPY);
}



Paper_obj_Mesh::Paper_obj_Mesh() : model( nullptr )
{}

Paper_obj_Mesh::~Paper_obj_Mesh()
{
	delete model;
}

void Paper_obj_Mesh::Initialize( iexMesh *model )
{
	this->model = model->Clone();
}

void Paper_obj_Mesh::Set_pose( const float angle, const Vector3& pos )
{
	Paper_obj::Set_pose( angle, pos );
	model->SetAngle( angle );
	model->SetPos( position );
}



Paper_obj_3DObj::Paper_obj_3DObj() : model( nullptr )
{}

Paper_obj_3DObj::~Paper_obj_3DObj()
{
	delete model;
}

void Paper_obj_3DObj::Initialize( iex3DObj *model )
{
	this->model = model->Clone();
}

void Paper_obj_3DObj::Set_pose( const float angle, const Vector3& pos )
{
	Paper_obj::Set_pose( angle, pos );
	model->SetAngle( angle );
	model->SetPos( position );
}

void Paper_obj_3DObj::Set_animframe( int frame )
{
	this->model->SetFrame( frame );
}
