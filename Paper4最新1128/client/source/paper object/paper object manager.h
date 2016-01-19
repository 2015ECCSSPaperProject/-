
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
	void Render(iexShader *shader = nullptr, char *name = '\0');
	void Forward_render();

	//**************************************************
	/// �v���C���[����̃A�N�V����
	//**************************************************

	// �j���I�u�W�F�N�g�̔ԍ�
	int Can_do(BasePlayer *player);

	void Can_dist(const Vector3 &pos, float dist, int out[]); // �͈͓��S���Ԃ�

	// Can_do�̌�ŌĂ�
	bool Can_rend(int index);

	// �j��
	void Rend(int index);

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
	// �N���[���֐��g���p
	iex3DObj *original_flyer;
	iexMesh *original_model;
	int number_of_objects;

	std::vector<Paper_obj*> obj_array;

	void Load();

	void Load_flyer();
	template<class POSTERCLASS>void Load_poster_tmp( char *filename, iexMesh *model );
};

extern Paper_obj_mng *paper_obj_mng;
