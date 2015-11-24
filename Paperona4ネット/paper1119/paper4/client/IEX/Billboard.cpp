#include	"iextreme.h"

void	Billboard::Draw3D(Vector3 pos, iex2DObj* obj, float width, float height, float tu[2], float tv[2], u32 dwFlags)
{
	//↓今だけ(仮)で　下の画像が上にはみ出るので
	tv[0] = 0.02f;

	Vector3 right(matView._11, matView._21, matView._31);
	Vector3 up(matView._12, matView._22, matView._32);
	right.Normalize();
	up.Normalize();

	//頂点作成
	LVERTEX v[4] = {
		{ pos.x - (right.x * width) + up.x * height, pos.y - (right.y * width) + up.y * height,
		pos.z - (right.z * width) + up.z * height, 0xFFFFFFFF, tu[0], tv[0] },

		{ pos.x + (right.x * width) + up.x * height, pos.y + (right.y * width) + up.y * height,
		pos.z + (right.z * width) + up.z * height, 0xFFFFFFFF, tu[1], tv[0] },

		{ pos.x - (right.x * width) - up.x * height, pos.y - (right.y * width) - up.y * height,
		pos.z - (right.z * width) - up.z * height, 0xFFFFFFFF, tu[0], tv[1] },

		{ pos.x + (right.x * width) - up.x * height, pos.y + (right.y * width) - up.y * height,
		pos.z + (right.z * width) - up.z * height, 0xFFFFFFFF, tu[1], tv[1] },

	};
	//描画
	iexPolygon::Render3D(v, 2, obj, dwFlags);
}


/**/
void	Billboard::Draw_MatBoard(Matrix m, Vector3 pos, iex2DObj* obj, float width, float height, float tu[2], float tv[2], u32 dwFlags)
{

	Vector3 tempright(m._11, m._12, m._13);
	Vector3 tempfront(m._31, m._32, m._33);
	tempfront.Normalize();
	tempright.Normalize();


	Vector3 front = tempfront + tempright*0.7f;
	//仮ベクトル
	Vector3 up(.0f, 1.0f, .0f);

	Vector3 right;
	Vector3Cross(right, up, front);

	//頂点作成
	LVERTEX v[4] = {
		{ pos.x - (right.x * width) + up.x * height, pos.y - (right.y * width) + up.y * height,
		pos.z - (right.z * width) + up.z * height, 0xFFFFFFFF, tu[0], tv[0] },

		{ pos.x + (right.x * width) + up.x * height, pos.y + (right.y * width) + up.y * height,
		pos.z + (right.z * width) + up.z * height, 0xFFFFFFFF, tu[1], tv[0] },

		{ pos.x - (right.x * width) - up.x * height, pos.y - (right.y * width) - up.y * height,
		pos.z - (right.z * width) - up.z * height, 0xFFFFFFFF, tu[0], tv[1] },

		{ pos.x + (right.x * width) - up.x * height, pos.y + (right.y * width) - up.y * height,
		pos.z + (right.z * width) - up.z * height, 0xFFFFFFFF, tu[1], tv[1] },

	};

	//描画
	iexPolygon::Render3D(v, 2, obj, dwFlags);
}

void	Billboard::Draw_VecBoard(Vector3 up, Vector3 right, Vector3 pos, iex2DObj* obj, float width, float height, float tu[2], float tv[2], u32 dwFlags)
{

	//頂点作成
	LVERTEX v[4] = {
		{ pos.x - (right.x * width) + up.x * height, pos.y - (right.y * width) + up.y * height,
		pos.z - (right.z * width) + up.z * height, 0xFFFFFFFF, tu[0], tv[0] },

		{ pos.x + (right.x * width) + up.x * height, pos.y + (right.y * width) + up.y * height,
		pos.z + (right.z * width) + up.z * height, 0xFFFFFFFF, tu[1], tv[0] },

		{ pos.x - (right.x * width) - up.x * height, pos.y - (right.y * width) - up.y * height,
		pos.z - (right.z * width) - up.z * height, 0xFFFFFFFF, tu[0], tv[1] },

		{ pos.x + (right.x * width) - up.x * height, pos.y + (right.y * width) - up.y * height,
		pos.z + (right.z * width) - up.z * height, 0xFFFFFFFF, tu[1], tv[1] },

	};
	//描画
	iexPolygon::Render3D(v, 2, obj, dwFlags);
}


