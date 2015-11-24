
#pragma once

class iexMesh;
struct Vector3;

class Poster_frame
{
private:
	iexMesh *model;

public:
	Poster_frame();
	~Poster_frame();

	void Initialize(iexMesh *mesh);
	void Render(Vector3 &pos, float angle);
};
