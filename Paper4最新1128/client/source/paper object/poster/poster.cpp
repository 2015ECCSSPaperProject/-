
#include "iextreme.h"
#include "../../system/System.h"
#include "../paper object.h"
#include "Poster.h"
//#include "../score/Score.h"
//#include "../player/Player.h"

#include	"../../sound/SoundManager.h"

Poster::Poster()
{}
Poster::~Poster()
{}

void Poster::Initialize(iex3DObj *model)
{
	Paper_obj::Initialize(model);
}

void Poster::Update()
{
	if (number == PLAYER_MAX) return;

	model->Update();
}

void Poster::Render()
{
	if (number == PLAYER_MAX) return;

	model->Update();
	model->Render();
}
