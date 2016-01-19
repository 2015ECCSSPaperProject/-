
#include "iextreme.h"
#include "paper object.h"
#include "../HitEffect/HitEffect.h"

Paper_obj::Paper_obj() : position( 0, 0, 0 ), angle( 0 ), broken( false )
{
	hiteffect = new HitEffect;
}

Paper_obj::~Paper_obj()
{
	delete hiteffect;
}

void Paper_obj::Update()
{
	hiteffect->Update( this->position );
}

void Paper_obj::Render( iexShader *shader, char *name)
{
	Subclass_render( shader, name );
}

void Paper_obj::Forward_render()
{
	hiteffect->Render();
}

void Paper_obj::Set_pose(const float angle, const Vector3& pos)
{
	this->angle = angle;
	this->position = pos;
}

const Vector3 &Paper_obj::Get_pos()
{
	return position;
}

void Paper_obj::Rend()
{
	hiteffect->Action();
	broken = true;
}

bool Paper_obj::Can_rend()
{
	return (!broken);
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
