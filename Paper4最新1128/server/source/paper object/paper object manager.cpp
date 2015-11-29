
#include "paper object manager.h"
#include "poster/Poster_manager.h"

Paper_obj_mng::Paper_obj_mng() : poster(nullptr)
{}

Paper_obj_mng::~Paper_obj_mng()
{
	delete poster;
}

void Paper_obj_mng::Initialize()
{
	poster = new Poster_mng;
	poster->Initialize();
}

void Paper_obj_mng::Update()
{
	poster->Update();
}

void Paper_obj_mng::Render()
{
	poster->Render();
}

//**************************************************

int Paper_obj_mng::Can_do(BasePlayer *player)
{
	return poster->Can_do(player);
}

bool Paper_obj_mng::Can_rend(int index)
{
	return poster->Can_rend(index);
}

const Vector3& Paper_obj_mng::Get_pos(int index)
{
	return poster->Get_pos(index);
}

float Paper_obj_mng::Get_angle(int index)
{
	return poster->Get_angle(index);
}

void Paper_obj_mng::Rend_poster(int number, int index)
{
	poster->Rend_poster(number, index);
}

void Paper_obj_mng::Can_dist(const Vector3 &pos, float dist, int number, int out[])
{
	poster->Can_dist(pos, dist, number, out);
}

Paper_obj_mng *paper_obj_mng = nullptr;
