
#include "Poster_frame.h"
#include "iextreme.h"

Poster_frame::Poster_frame() : model(nullptr)
{}

Poster_frame::~Poster_frame()
{
	delete model;
}

void Poster_frame::Initialize(iexMesh *mesh)
{
	model = mesh;
}

void Poster_frame::Render(Vector3 &pos, float angle)
{
	model->SetAngle(angle);
	model->SetPos(pos);
	model->Update();
	model->Render();
}
