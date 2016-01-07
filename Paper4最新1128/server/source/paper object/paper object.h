
// #include "iextreme.h"

#pragma once

struct Vector3;
class iexMesh;
class iex3DObj;

class BasePlayer;

enum KIND_PAPER_OBJECT;

class Paper_obj
{
public:
	Paper_obj();
	virtual ~Paper_obj();

	virtual void Initialize(int model_type, int point);

	virtual void Update() = 0;

	virtual void Render() = 0;


	virtual bool Can_do(BasePlayer *player) = 0;

	virtual bool Can_rend() = 0;

	virtual bool Can_dist(const Vector3 &pos, float dist) = 0;

	virtual void Rend() = 0;

	//**************************************************

	int Get_point();

	int Get_type();

	int Get_number();

	void Set_number(int in);

	virtual int Get_animation_frame() = 0;

	float Get_angle();

	const Vector3& Get_pos();

	virtual void Set_pose(const float angle, const Vector3& pos);

	//**************************************************

	virtual unsigned int Get_send_data_size() = 0;

	virtual void Get_send_data( char *out ) = 0;

	virtual KIND_PAPER_OBJECT Get_kind() = 0;

protected:
	int number;

	int model_type;

	Vector3 position; // 位置
	Vector3 forward; // ポスターの向き<表>
	float angle; // 向きを model に渡すよう

	int point; // 点数
};

class Paper_obj_Mesh : public Paper_obj
{
public:
	Paper_obj_Mesh();
	Paper_obj_Mesh( int model_type, iexMesh *model, int point );
	virtual void Initialize( int model_type, iexMesh *model, int point );
	virtual ~Paper_obj_Mesh();

	void Set_pose( const float angle, const Vector3& pos );

protected:
	iexMesh *model; // 3Dモデル
};

class Paper_obj_3DObj : public Paper_obj
{
public:
	Paper_obj_3DObj();
	Paper_obj_3DObj( int model_type, iex3DObj *model, int point );
	virtual void Initialize( int model_type, iex3DObj *model, int point );
	virtual ~Paper_obj_3DObj();

	void Set_pose( const float angle, const Vector3& pos );

protected:
	iex3DObj *model; // 3Dモデル
};
