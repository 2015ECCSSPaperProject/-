
#include "iextreme.h"
#include "../paper object.h"
#include "test box.h"
#include "../../Player/BasePlayer.h"

void Test_box::Initialize(iex3DObj *model)
{
	Paper_obj::Initialize(model);
}

void Test_box::Update()
{
	if (number == PLAYER_MAX)
		return;

	if (model == nullptr)
		return;

	model->iexMesh::Update();
}

void Test_box::Render()
{
	if (number == PLAYER_MAX)
		return;

	if (model == nullptr)
		return;

	model->iexMesh::Render();
}
