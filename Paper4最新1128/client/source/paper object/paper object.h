
#pragma once

struct Vector3;
class iex3DObj;
class iexMesh;

class BasePlayer;

enum KIND_PAPER_OBJECT;

class HitEffect;

class Paper_obj
{
public:
	Paper_obj();
	virtual ~Paper_obj();

	void Update();

	void Render(iexShader *shader = nullptr, char *name = '\0');

	void Forward_render();

	void Set_pose(const float angle, const Vector3& pos);

	void Set_animframe( int frame )
	{};

	const Vector3& Get_pos();

	virtual KIND_PAPER_OBJECT Get_kind() = 0;

	bool Can_rend();

	virtual void Rend(); // ”j‚é
	virtual void Rend_by_skill(BasePlayer *player);

	//**************************************************

	int Get_point();	// return‚Ì•Ï”‚ª•Ï”éŒ¾‚æ‚èã‚ÅƒGƒ‰[‚ªo‚Ä‚½‚Ì‚Åcpp‚É‘‚«‚Ü‚µ‚½

	//**************************************************

	virtual unsigned int Get_receive_data_size() = 0;

	virtual void Set_receive_data( char *in ) = 0;

protected:
	bool broken;
	Vector3 position; // ˆÊ’u
	float angle; // Œü‚«‚ğ model ‚É“n‚·‚æ‚¤

	HitEffect *hiteffect;

	int point;

	// tuikasimasita
	int rend_delay_time;	// ”j‚ê‚é‚Æ‚«‚É’x‚ç‚¹‚é(‚²‚è‰Ÿ‚µ)

	virtual void Subclass_update() = 0;
	virtual void Subclass_render( iexShader *shader = nullptr, char *name = '\0' ) = 0;
};

class Paper_obj_Mesh : public Paper_obj
{
public:
	Paper_obj_Mesh();
	~Paper_obj_Mesh();
	virtual void Initialize( iexMesh *model, int point );

	void Set_pose( const float angle, const Vector3& pos );

protected:
	iexMesh *model; // 3Dƒ‚ƒfƒ‹
};

class Paper_obj_3DObj : public Paper_obj
{
public:
	Paper_obj_3DObj();
	~Paper_obj_3DObj();
	virtual void Initialize( iex3DObj *model, int point );

	void Set_pose( const float angle, const Vector3& pos );

	void Set_animframe( int frame );

protected:
	iex3DObj *model; // 3Dƒ‚ƒfƒ‹
};
