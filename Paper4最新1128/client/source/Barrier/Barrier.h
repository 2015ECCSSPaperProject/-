#pragma once
#include "iextreme.h"
#include "../Deferred/Deferred.h"
#include "../Animation/AnimationUV.h"

class Barrier
{
public:
	Barrier();
	~Barrier();

	void Update(Vector3 Pos, Vector3 ViewVec, float Scale);
	void Render();

	void Action();
	void Stop();

private:

	AnimationUV* barrier;



};
