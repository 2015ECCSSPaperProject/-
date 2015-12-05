
#pragma once

class BasePlayer;

class Poster : public Paper_obj
{
private:
	enum class MOTION_NUMBER
	{
		WAITE = 0,
		REND = 1
	};

	struct 
	{
		float min_y, // ‰º•ûŒü
			max_y; // ã•ûŒü
		float wide; // ¶‰E
		float forward; // ‘O•û
	}range;

	enum class MODE
	{
		WAITE,
		REND,
		MAX
	}mode;
	// ƒ‚[ƒh
	class Mode
	{
	protected:
		Poster *me;
	public:
		Mode(Poster *me) : me(me){}
		virtual ~Mode(){}
		virtual void Initialize(){}
		virtual void Update(){}
		virtual void Render(){}
	}*mode_list[(int)MODE::MAX];
	// ‘Ò‹@’†
	class Mode_waite : public Mode
	{
	public:
		Mode_waite(Poster *me) : Mode(me){}
		void Initialize() override;
		void Update() override;
		void Render() override;
	};
	// ”j‚ç‚ê‚Ä‚é
	class Mode_rend : public Mode
	{
	public:
		Mode_rend(Poster *me) :Mode(me){}
		void Initialize() override;
		void Update() override;
		void Render() override;
	};

	void Change_mode(MODE mode);

public:
	Poster();
	~Poster();

	void Initialize(int model_type, iex3DObj *model, int point);
	void Release();
	void Update();
	void Render();

	// ƒQƒbƒ^[AƒZƒbƒ^[
	const Vector3& Get_pos(){ return position; }

	float Get_angle(){ return angle; }

	void Set_pose(const Vector3& forward, const Vector3& pos); // p¨‚ğŒvZ‚·‚é
	void Set_pose(const float angle, const Vector3& pos); // p¨‚ğŒvZ‚·‚é

	// ƒvƒŒƒCƒ„[‚©‚ç‚ÌƒAƒNƒVƒ‡ƒ“
	void Do_playeraction(BasePlayer *player, int number); // ”j‚é“\‚é
	void Rend(int number); // ”j‚é
	void Rend();
	void Paste(int number); // “\‚é

	bool Can_do(BasePlayer *player, int number); // player‚ªƒ|ƒXƒ^[‚É‰½‚©‚Å‚«‚éó‘Ô
	bool Can_do(BasePlayer *player);

	bool Can_rend(int number); // ”j‚¯‚é
	bool Can_rend();

	bool Can_paste(int number); // “\‚ê‚é

	bool Can_dist(const Vector3 &pos, float dist, int number);	// vs†“S–C‚Æ‚©‚Ég‚¤
	bool Can_dist(const Vector3 &pos, float dist);
	bool Check_dist(const Vector3 &pos, float dist);

	// ƒeƒXƒg
	void Change_user(int number);

	int Get_animation_frame(){ return model->GetFrame(); }

	//**************************************************

	unsigned int Get_send_data_size();

	void Get_send_data( char *out );

};
