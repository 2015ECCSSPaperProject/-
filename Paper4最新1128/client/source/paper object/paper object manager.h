
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
	void Render(BasePlayer *player, iexShader *shader = nullptr, char *name = '\0');

	//**************************************************
	/// �v���C���[����̃A�N�V����
	//**************************************************

	// �j���I�u�W�F�N�g�̔ԍ�
	int Can_do(BasePlayer *player);

	void Can_dist(const Vector3 &pos, float dist, int out[]); // �͈͓��S���Ԃ�

	// Can_do�̌�ŌĂ�
	bool Can_rend(int index);

	// �j��
	void Rend_poster(int index);

	//**************************************************
	/// Get Set
	//**************************************************
	
	int Get_numof();
		
	void Set_user(int index, int number);

	void Set_animframe(int index, int frame);

	const Vector3& Get_pos(int index);

	//**************************************************

	unsigned int Get_receive_data_size();

	void Set_receive_data( char *in );

protected:
	iex3DObj *original_model; // �N���[���֐��g���p
	int number_of_objects;

	std::vector<Paper_obj*> obj_array;

	void Load();

	void Load_poster();

	void Load_flyer();

	// ���
	iex2DObj *mark;
};

extern Paper_obj_mng *paper_obj_mng;
