
class iexMesh;
struct Vector3;

#include "Area.h"
#include "../../IEX/iextreme.h"

Area::Area() : wall(nullptr)
{}

Area::Area(char *filename) : wall(nullptr)
{
	Set_mesh(filename);
}

Area::~Area()
{
	delete wall;
}



void Area::Set_mesh(char *filename)
{
	delete wall;
	wall = new iexMesh(filename);
}



int Area::RayPick(Vector3* out, Vector3* pos, Vector3* vec, float *Dist)
{
	return wall->RayPick(out, pos, vec, Dist);
}



void Area::Render()
{
	if (wall)
		wall->Render();
}
