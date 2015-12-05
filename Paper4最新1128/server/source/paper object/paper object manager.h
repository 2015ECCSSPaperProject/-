
#pragma once

struct Vector3;
class iex3DObj;

class BasePlayer;
class Paper_obj;

class Paper_obj_mng
{
public:
	Paper_obj_mng();
	~Paper_obj_mng();

	void Initialize();
	void Release();
	void Update();
	void Render();

	//**************************************************
	/// プレイヤーからのアクション
	//**************************************************

	// 破れるオブジェクトの番号
	int Can_do(BasePlayer *player);

	void Can_dist(const Vector3 &pos, float dist, int out[]); // 範囲内全部返す

	// Can_doの後で呼ぶ
	bool Can_rend(int index);

	// 破る
	void Rend(int index);

	int Paper_obj_mng::Can_targeting(BasePlayer *player, float range_dist, int range_degree);

	//**************************************************
	/// Get Set
	//**************************************************
	
	int Get_numof();
	
	int Get_number(int index);

	int Get_point(int index);

	int Get_type(int index);

	int Get_animation_frame(int index);

	float Get_angle(int index);

	const Vector3& Get_pos(int index);
	
	//**************************************************

	unsigned int Get_send_data_size();

	void Get_send_data( char *out );

protected:
	iex3DObj *original_model; // クローン関数使う用
	int number_of_objects;

	std::vector<Paper_obj*> obj_array;

	void Load();
};

extern Paper_obj_mng *paper_obj_mng;
