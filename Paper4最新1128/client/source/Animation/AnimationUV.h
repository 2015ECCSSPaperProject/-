#pragma once
#include "iextreme.h"
#include "../system/System.h"

/*
*UVアニメーションクラス
*/

/* 使い方 */
/* ・UVアニメーションをさせたいメッシュを読み込む				*/
/* ・更新にUpdate関数・描画にRender関数をそれぞれ置く			*/
/* ・最後にAction関数を実行するとUVアニメーションが始まります。	*/

class AnimationUV
{
public:
	// 過去の仕様
	// メッシュの名前　1フレームに動くX＆Y　終了フレーム　最後に透明になるかのフラグ　透明になっていく始まりのフレーム　ループをするか
	//AnimationUV(char* name, float moveTU, float moveTV, int EndFlame, bool AlphaFlag = false, int AlphaNear = 0, bool IsRoop = false);
	
	// 現在の仕様
	AnimationUV(char* name, float moveTU, float moveTV, int EndFlame,bool IsRoop = false);
	AnimationUV(char* name, float moveTU, float moveTV, int EndFlame, bool IsRoop, int AlphaNear, int AlphaFar);
	~AnimationUV();

	void Action();		// アニメ実行
	void Stop();		// アニメ停止

	void ActionRoop();  // ループアニメ実行
	void StopRoop();	// ループアニメストップ

	void Update(Vector3 pos = Vector3(0.0f, 0.0f, 0.0f),
		Vector3 angle = Vector3(0.0f, 0.0f, 0.0f), float scale = 1.0f);	// 更新
	void Update(Vector3 pos ,Vector3 angle , Vector3 scale);			// 更新スケールをvector型に変えたもの
	void Render();			// 描画
	void Render_Barrier();	// バリアー描画
	void Render_AT();		// AT描画

private:
	iexMesh*	obj;
	float		tu, tv;			// 毎フレームの横縦のUVの座標
	float		moveTu, moveTv;	// 毎フレームの横縦のUVの動き
	int			endFlame;		// 描画終りのフレーム
	int			nowFlame;		// 今のフレーム

	bool		isAction;		// アニメを実行しているか
	bool		isRoop;			// ループエフェクトか

	bool		alphaFlag;		// 最後透明にするフラグ
	int			alphaNear;		// 透明じゃなくなる　始めるフレーム
	int			alphaFar;		// 透明じゃなくなる　終わるフレーム　
	float		alpha;			// 透明度
};
