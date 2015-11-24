
#pragma once

class Area
{
public:
	Area();
	Area(char *filename);
	~Area();

	void Set_mesh(char *filename);

	int RayPick(Vector3* out, Vector3* pos, Vector3* vec, float *Dist);

	void Render();

protected:
	iexMesh *wall;
};
