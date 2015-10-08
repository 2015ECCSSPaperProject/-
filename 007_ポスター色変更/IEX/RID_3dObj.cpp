#include "iextreme.h"
#include "RID_3dObj.h"

RID_mesh::RID_mesh() : iexMesh()
{
}
RID_mesh::RID_mesh(char *filename) : iexMesh(filename)
{
}

RID_mesh::~RID_mesh()
{}

//**************************************************

int	RID_mesh::RayPick(Vector3* out, Vector3* pos, Vector3* vec, float *Dist)
{
	int		ret = -1;

	Matrix inv;
	D3DXMatrixInverse(&inv, NULL, &this->TransMatrix);
	Matrix mat;

	ZeroMemory(&mat, sizeof(Matrix));
	mat._41 = pos->x, mat._42 = pos->y, mat._43 = pos->z;
	mat *= inv;
	Vector3 p(mat._41, mat._42, mat._43);

	ZeroMemory(&mat, sizeof(Matrix));
	mat._41 = vec->x, mat._42 = vec->y, mat._43 = vec->z;
	mat *= inv;
	vec->x = mat._41, vec->y = mat._42, vec->z = mat._43;
	Vector3 vv = *vec;

	float neart = *Dist;
	float dist = *Dist;
	dist = dist*dist;
	*out = p;
	//	情報取得	
	u32 fvf = lpMesh->GetFVF();
	//	頂点サイズ計算
	int VertexSize = D3DXGetFVFVertexSize(fvf) / sizeof(float);

	//	バッファロック
	float	*pVertices;
	u16		*pIndices;
	u32 NumIndices = lpMesh->GetNumFaces();
	lpMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVertices);
	lpMesh->LockIndexBuffer(D3DLOCK_READONLY, (void**)&pIndices);

	Vector3 v1, v2, v3;
	Vector3	n;
	Vector3	l1, l2, l3;
	Vector3	temp;
	Vector3	cp;

	Vector3 p1, p2, p3;

	for (u32 j = 0; j<NumIndices; j++)
	{
		//	面頂点取得
		int a = pIndices[j * 3 + 0] * VertexSize;
		v1.x = pVertices[a];	v1.y = pVertices[a + 1];	v1.z = pVertices[a + 2];

		int b = pIndices[j * 3 + 1] * VertexSize;
		v2.x = pVertices[b];	v2.y = pVertices[b + 1];	v2.z = pVertices[b + 2];

		int c = pIndices[j * 3 + 2] * VertexSize;
		v3.x = pVertices[c];	v3.y = pVertices[c + 1];	v3.z = pVertices[c + 2];

		l1.x = v2.x - v1.x;
		l1.y = v2.y - v1.y;
		l1.z = v2.z - v1.z;
		l2.x = v3.x - v2.x;
		l2.y = v3.y - v2.y;
		l2.z = v3.z - v2.z;

		//	外積による法線算出		
		Vector3Cross(n, l1, l2);
		//	内積の結果がプラスならば裏向き
		float dot = Vector3Dot(vv, n);
		if (dot >= 0) continue;
		//	交点算出
		p1.x = v1.x - p.x;
		p1.y = v1.y - p.y;
		p1.z = v1.z - p.z;
		float t = Vector3Dot(n, p1) / dot;
		if (t < .0f || t > neart) continue;

		cp.x = vv.x*t + p.x;
		cp.y = vv.y*t + p.y;
		cp.z = vv.z*t + p.z;
		//	内点判定
		p1.x = v1.x - cp.x;
		p1.y = v1.y - cp.y;
		p1.z = v1.z - cp.z;

		Vector3Cross(temp, p1, l1);
		if (Vector3Dot(temp, n) < .0f) continue;

		p2.x = v2.x - cp.x;
		p2.y = v2.y - cp.y;
		p2.z = v2.z - cp.z;
		Vector3Cross(temp, p2, l2);
		if (Vector3Dot(temp, n) < .0f) continue;

		l3.x = v1.x - v3.x;
		l3.y = v1.y - v3.y;
		l3.z = v1.z - v3.z;
		p3.x = v3.x - cp.x;
		p3.y = v3.y - cp.y;
		p3.z = v3.z - cp.z;
		Vector3Cross(temp, p3, l3);
		if (Vector3Dot(temp, n) < .0f) continue;

		*out = cp;
		*vec = n;
		ret = j;
		neart = t;
	}
	lpMesh->UnlockVertexBuffer();
	lpMesh->UnlockIndexBuffer();
	*Dist = neart;

	ZeroMemory(&mat, sizeof(Matrix));
	mat._41 = out->x, mat._42 = out->y, mat._43 = out->z;
	mat *= TransMatrix;
	out->x = mat._41, out->y = mat._42, out->z = mat._43;

	ZeroMemory(&mat, sizeof(Matrix));
	mat._41 = vec->x, mat._42 = vec->y, mat._43 = vec->z;
	mat *= TransMatrix;
	vec->x = mat._41, vec->y = mat._42, vec->z = mat._43;

	return	ret;
}

//**************************************************

// 3DObj

//**************************************************

void RID_3Dobj::Animation(float speed)
{
	if (speed > 1.0f)
		return;
	if (speed < 0.0f)
		return;

	int		param;
	u32	work;



	work = dwFrame;
	param = dwFrameFlag[dwFrame];
	if (param & 0x4000) param = 0xFFFF;
	if (param != 0xFFFF){
		//	アニメーションジャンプ
		if (param & 0x8000){
			SetMotion(param & 0xFF);
		}
		else dwFrame = param;
	}
	else {
		while (true)
		{
			if (speed < 1.0f)
			{
				biteween_frame += speed;
				if (biteween_frame >= 1.0f)
				{
					biteween_frame -= 1.0f;
					dwFrame++;
					if (dwFrame >= NumFrame) dwFrame = 0;
				}
					break;
			}
			else
			{
				speed -= 1.0f;
				dwFrame++;
			}

			if (dwFrame >= NumFrame) dwFrame = 0;
		}
	}

	if (dwFrame != work) bChanged = TRUE;

	param = dwFrameFlag[dwFrame];
	if ((param != 0xFFFF) && (param & 0x4000)) Param[(param & 0x0F00) >> 8] = (u8)(param & 0x00FF);

}

void RID_3Dobj::Update_01()
{
	/*	スキンメッシュ更新	*/
	UpdateSkinMeshFrame01((float)dwFrame, biteween_frame);
	UpdateBoneMatrix();
	UpdateSkinMesh();

	iexMesh::Update();
	RenderFrame = dwFrame;
	bChanged = FALSE;
}

void RID_3Dobj::UpdateSkinMeshFrame01(float frame, float per)
{
	u32			i, j;
	LPIEXANIME2	lpAnime;
	float		t;

	for (i = 0; i<NumBone; i++){
		lpAnime = &this->lpAnime[i];

		//	ポーズ設定
		if (lpAnime->rotNum == 0) CurPose[i] = orgPose[i];
		else if (lpAnime->rotNum == 1) CurPose[i] = lpAnime->rot[0];
		else {
			//	回転キー補間
			for (j = 0; j<lpAnime->rotNum - 1; j++)
			{
				//	現在位置検索
				if ((frame >= lpAnime->rotFrame[j]) && (frame<lpAnime->rotFrame[j + 1]))
				{
					//	経過フレーム計算
					t = (float)(frame - lpAnime->rotFrame[j] + per) / (float)(lpAnime->rotFrame[j + 1] - lpAnime->rotFrame[j]);
					//	補間
					CurPose[i] = QuaternionSlerp(lpAnime->rot[j], lpAnime->rot[j + 1], t);
					break;
				}
			}
			if (j == lpAnime->rotNum - 1) CurPose[i] = lpAnime->rot[lpAnime->rotNum - 1];
		}
		//	座標設定
		if (lpAnime->posNum == 0) CurPos[i] = orgPos[i];
		else {
			//	位置補間
			for (j = 0; j<lpAnime->posNum - 1; j++)
			{
				//	現在位置検索
				if ((frame >= lpAnime->posFrame[j]) && (frame<lpAnime->posFrame[j + 1]))
				{
					//	経過フレーム計算
					t = (float)(frame - lpAnime->posFrame[j] + per) / (float)(lpAnime->posFrame[j + 1] - lpAnime->posFrame[j]);
					//	補間
					CurPos[i] = lpAnime->pos[j] + (lpAnime->pos[j + 1] - lpAnime->pos[j])*t;
					break;
				}
			}
			if (j == lpAnime->posNum - 1) CurPos[i] = lpAnime->pos[lpAnime->posNum - 1];
		}
	}
}
