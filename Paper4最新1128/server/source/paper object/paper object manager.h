
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
	/// �v���C���[����̃A�N�V����
	//**************************************************

	// �j���I�u�W�F�N�g�̔ԍ���Ԃ�
	int Can_do(BasePlayer *player);
	// �͈͓��S���Ԃ�
	void Can_dist(const Vector3 &pos, float dist, int out[]);

	// Can_do�̌�ŌĂ�
	bool Can_rend(int index); // �I�u�W�F�N�g�̔ԍ�

	// �j��
	void Rend(int index); // �I�u�W�F�N�g�̔ԍ�

	int Paper_obj_mng::Can_targeting(BasePlayer *player, float range_dist, int range_degree); // �J�������ڂł��邩

	//**************************************************
	/// Get Set
	//**************************************************
	
	int Get_numof(); // �I�u�W�F�N�g�̌�
	
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
	iex3DObj *original_model; // �N���[���֐��g���p
	int number_of_objects; // �I�u�W�F�N�g�̌�

	std::vector<Paper_obj*> obj_array; // �I�u�W�F�N�g�̔z��

	void Load(); // �e�L�X�g�t�@�C������z�u��ǂݍ���

	void Load_flyer(); // �t���C���[������������
	template<class POSTERCLASS>void Load_timeposter_tmp( char *filename, int telopid ); // �I�u�W�F�N�g���Ƃ̓ǂݍ���
	template<class POSTERCLASS>void Load_poster_tmp( char *filename ); // �I�u�W�F�N�g���Ƃ̓ǂݍ���
};

extern Paper_obj_mng *paper_obj_mng;
