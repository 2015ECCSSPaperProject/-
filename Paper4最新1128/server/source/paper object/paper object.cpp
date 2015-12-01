
#include "iextreme.h"
#include "paper object.h"

Paper_obj::Paper_obj() : model_type(0), model(nullptr), position(0, 0, 0), forward(0, 0, 1), angle(0), point(0)
{}

Paper_obj::~Paper_obj()
{
	delete model;
}

void Paper_obj::Initialize(int model_type, iex3DObj *model, int point)
{
	this->model_type = model_type;
	this->model = model->Clone();
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

int Paper_obj::Get_number()
{
	return number;
}

void Paper_obj::Set_number(int in)
{
	number = in;
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
	model->SetAngle(angle);

	this->position = pos;
	model->SetPos(position);
}