#ifndef RID3DOBJ_H
#define RID3DOBJ_H

class RID_mesh : public iexMesh
{
public:
	RID_mesh();
	RID_mesh(char *filename);
	~RID_mesh();

	//**************************************************

	int RayPick(Vector3* out, Vector3* pos, Vector3* vec, float *Dist);

private:
};

class RID_3Dobj : public iex3DObj
{
public:
	RID_3Dobj() :iex3DObj(){}
	RID_3Dobj(char* filename) :iex3DObj(filename){}
	~RID_3Dobj(){ iex3DObj::~iex3DObj(); }

	void Animation(float speed);
	void Update_01();

	void UpdateSkinMeshFrame01(float frame, float per);
private:
	float biteween_frame = 0;

};

#endif // !RID3DOBJ_H
