
#pragma once

//#include <vector>
//#include "iextreme.h"

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
	void Render(const Vector3 &pos, iexShader *shader, char *name);
	void Forward_render();

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
	void Rend_by_skill(int index, BasePlayer *player);

	//**************************************************
	/// Get Set
	//**************************************************
	
	int Get_numof();
		
	void Set_animframe(int index, int frame);

	const Vector3& Get_pos(int index);

	KIND_PAPER_OBJECT Get_kind( int index );

	int Get_point(int inedex);

	//**************************************************

	unsigned int Get_receive_data_size();

	void Set_receive_data( char *in );

protected:
	// クローン関数使う用
	iex3DObj *original_flyer;
	iexMesh *original_model;
	int number_of_objects;

	std::vector<Paper_obj*> obj_array;

	void Load();

	template<class POSTERCLASS, class MESHCLASS>void Load_timeposter_tmp( char *filename, MESHCLASS *model );
	template<class POSTERCLASS>void Load_poster_tmp( char *filename, iexMesh *model );
};

extern Paper_obj_mng *paper_obj_mng;
