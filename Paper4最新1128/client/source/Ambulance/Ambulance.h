#pragma once


class Ambulance
{
public:
	enum class MOVE_TYPE
	{
		ROUND_TRIP,		// ����
		AROUND_OUTER,	// �O�������
		MAX
	};

	//===============================================
	//	�������Ɖ��
	//===============================================
	Ambulance();
	~Ambulance();
	void Initialize(iex3DObj *clone, Ambulance::MOVE_TYPE type, const Vector3 &start, const Vector3 &goal, float speed);


	//===============================================
	//	�X�V�ƕ`��
	//===============================================
	void Update();
	void Render(iexShader *shader = nullptr, char *name = '\0');

	//===============================================
	//	�Q�b�^�[,�Z�b�^�[
	//===============================================
	Vector3 GetPos();


private:
	Vector3 pos;
	Vector3 start_pos;
	Vector3 goal_pos;	// ���̍��W�܂ŗ����������ς���
	Vector3 move;
	float angle;
	float speed;
	float MOVE_LENGTH;
	float scale;
	int se_receive;
	MOVE_TYPE move_type;

	void MoveRoundTrip();
	void MoveAroundOuter();
	void(Ambulance::*MoveFunk[(int)MOVE_TYPE::MAX])();

	iex3DObj* obj;
};

class AmbulanceMng
{
public:

	//===============================================
	//	�������Ɖ��
	//===============================================
	AmbulanceMng();
	~AmbulanceMng();
	void Initialize();

	//===============================================
	//	�X�V�ƕ`��
	//===============================================
	void Update();
	void Render(iexShader *shader = nullptr, char *name = '\0');

	//===============================================
	//	�Q�b�^�[,�Z�b�^�[
	//===============================================
	void Append(Ambulance::MOVE_TYPE type, const Vector3 &start, const Vector3 &goal, float speed);


	//===============================================
	//	����
	//===============================================
	int CheckDist(const Vector3 &pos, float dist);
	Ambulance *GetAmbulance(int no);

private:
	std::vector<Ambulance*> List;
	iex3DObj *clone;

	void Load(LPSTR filename);
};

extern AmbulanceMng *ambulance_mng;