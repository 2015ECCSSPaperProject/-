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

private:
	Vector3 pos;
	Vector3 goal_pos;	// ���̍��W�܂ŗ����������ς���
	float angle;
	float speed;
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

private:
	std::vector<Ambulance*> List;
	iex3DObj *clone;
};