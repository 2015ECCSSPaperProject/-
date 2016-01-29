
#pragma once

struct Vector3;
class iex3DObj;

class BasePlayer;
class Paper_obj;

enum KIND_PAPER_OBJECT;

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

	// 破れるオブジェクトの番号を返す
	int Can_do(BasePlayer *player);
	// 範囲内全部返す
	void Can_dist(const Vector3 &pos, float dist, int out[]);

	// Can_doの後で呼ぶ
	bool Can_rend(int index); // オブジェクトの番号

	// 破る
	void Rend(int index); // オブジェクトの番号

	int Paper_obj_mng::Can_targeting(BasePlayer *player, float range_dist, int range_degree); // カメラ注目できるか

	//**************************************************
	/// Get Set
	//**************************************************
	
	int Get_numof(); // オブジェクトの個数
	
	bool Is_broken( int index );

	int Get_point(int index);

	int Get_type(int index);

	int Get_animation_frame(int index);

	float Get_angle(int index);

	const Vector3& Get_pos(int index);

	KIND_PAPER_OBJECT Get_kind(int index);
	
	//**************************************************

	unsigned int Get_send_data_size();

	void Get_send_data( char *out );

protected:
	iex3DObj *original_model; // クローン関数使う用
	int number_of_objects; // オブジェクトの個数

	std::vector<Paper_obj*> obj_array; // オブジェクトの配列

	void Load(); // テキストファイルから配置を読み込む

	void Load_flyer(); // フライヤーだけ処理が別
	template<class POSTERCLASS>void Load_timeposter_tmp( char *filename, int telopid ); // オブジェクトごとの読み込み
	template<class POSTERCLASS>void Load_poster_tmp( char *filename ); // オブジェクトごとの読み込み
};

extern Paper_obj_mng *paper_obj_mng;
