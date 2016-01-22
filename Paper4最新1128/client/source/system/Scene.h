#ifndef __SCENE_H__
#define __SCENE_H__
#include	"../../IEX/iextreme.h"

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//
//		基底クラス＿シーン
//
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

class Scene
{
public:
	//	生成・解放
	Scene(){}
	virtual ~Scene(){}
	//	初期化
	virtual bool Initialize(){ return true; }
	//	更新・描画
	virtual bool Update(){ return false; }
	virtual void Render(){}
};

//*****************************************************************************
#endif
