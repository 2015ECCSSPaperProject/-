#pragma once
#include "iextreme.h"
//#include <Effekseer/Effekseer.h>
//#include <Effekseer/EffekseerRendererDX9.h>
#include "Effekseer.h"
#include "EffekseerRendererDX9.h"

#if _DEBUG
#pragma comment(lib, "Effekseer.Debug.lib" )
#pragma comment(lib, "EffekseerRendererDX9.Debug.lib" )
#else
#pragma comment(lib, "Effekseer.Release.lib" )
#pragma comment(lib, "EffekseerRendererDX9.Release.lib" )
#endif
/**************************************************
エフェクトクラス
**************************************************/

enum class Type
{
	None = -1,
	Damege,
	SIZE
};


class Effector
{
private:
	static const int EFFECT_KIND = 50;
public:
	Effector();
	~Effector();
	::Effekseer::Manager* manager;
	::Effekseer::Effect* eff[EFFECT_KIND];

	bool Init();
	bool LoadEffect(int ID, const EFK_CHAR* filename);

	void SetProjectionMatrix(Matrix mat);
	void SetViewMatrix(Matrix mat);
	void Update();
	void Render();
private:
	static const int EFFECT_MAX = 200;
	static const int SPRITE_MAX = 5000;
	::EffekseerRenderer::Renderer* renderer;
};

extern Effector *effek;
