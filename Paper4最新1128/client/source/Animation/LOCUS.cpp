#include	"iextreme.h"
#include	"LOCUS.h"

//*****************************************************************************************************************************
//
//	剣の軌跡エフェクト
//
//*****************************************************************************************************************************

//*********************************************
//	剣の残像
//*********************************************
LOCUS::LOCUS(unsigned int max, unsigned int n) : tex(NULL){
	frame_max = max;
	separate_num = n;//結び目
	locus_max = (frame_max - 1)*(separate_num - 1) + frame_max;
	kako_zahyou1 = new Vector3[locus_max];
	kako_zahyou2 = new Vector3[locus_max];
	tu = new float[locus_max];
	tex = new iex2DObj("DATA/Effect/LocusTex.png");
	for (unsigned int i = 0; i < locus_max; i++){
		tu[i] = i*(1.0f / (locus_max - 1.0f));
	}
}
LOCUS::~LOCUS(){
	if (tex) delete tex;
	if (kako_zahyou1) delete[] kako_zahyou1;
	if (kako_zahyou2) delete[] kako_zahyou2;
	if (tu) delete[] tu;
}

void LOCUS::Update(const Vector3& p1, const Vector3& p2)
{
	//kako_zahyou[0] = p1;

	// 頂点送り
	// 過去座標[1]に１フレーム前の座標
	// 過去座標[2]に２フレーム前の座標
	// となるように、過去の座標を保存する
	for (int i = locus_max - 1; i>0; i--)
	{
		kako_zahyou1[i] = kako_zahyou1[i - 1];
		kako_zahyou2[i] = kako_zahyou2[i - 1];

	}
	kako_zahyou1[0] = p1;
	kako_zahyou2[0] = p2;

}

void LOCUS::Render()
{


	DWORD col = 0xFFFFFFFF;

	iexSystem::Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	LVERTEX vvv[4];
	for (unsigned int i = 0; i<locus_max - 1; i++)
	{
		vvv[0].x = kako_zahyou2[i].x;
		vvv[0].y = kako_zahyou2[i].y;
		vvv[0].z = kako_zahyou2[i].z;
		vvv[0].color = col;
		vvv[0].tu = tu[i];
		vvv[0].tv = 0.0f;

		vvv[1].x = kako_zahyou1[i].x;
		vvv[1].y = kako_zahyou1[i].y;
		vvv[1].z = kako_zahyou1[i].z;
		vvv[1].color = col;
		vvv[1].tu = tu[i];
		vvv[1].tv = 1.0f;

		vvv[2].x = kako_zahyou2[i + 1].x;
		vvv[2].y = kako_zahyou2[i + 1].y;
		vvv[2].z = kako_zahyou2[i + 1].z;
		vvv[2].color = col;
		vvv[2].tu = tu[i + 1];
		vvv[2].tv = 0.0f;

		vvv[3].x = kako_zahyou1[i + 1].x;
		vvv[3].y = kako_zahyou1[i + 1].y;
		vvv[3].z = kako_zahyou1[i + 1].z;
		vvv[3].color = col;
		vvv[3].tu = tu[i + 1];
		vvv[3].tv = 1.0f;

		//ｔｅｘｔｕｒｅ
		iexPolygon::Render3D(vvv, 2, tex, RS_COPY_NOZ);
	}
	iexSystem::Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}