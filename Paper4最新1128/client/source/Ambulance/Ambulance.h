#pragma once


class Ambulance
{
public:
	enum class MOVE_TYPE
	{
		ROUND_TRIP,		// 往復
		AROUND_OUTER,	// 外周を回る
		MAX
	};

	//===============================================
	//	初期化と解放
	//===============================================
	Ambulance();
	~Ambulance();
	void Initialize(iex3DObj *clone, Ambulance::MOVE_TYPE type, const Vector3 &start, const Vector3 &goal, float speed);


	//===============================================
	//	更新と描画
	//===============================================
	void Update();
	void Render(iexShader *shader = nullptr, char *name = '\0');

	//===============================================
	//	ゲッター,セッター
	//===============================================

private:
	Vector3 pos;
	Vector3 goal_pos;	// この座標まで来たら方向を変える
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
	//	初期化と解放
	//===============================================
	AmbulanceMng();
	~AmbulanceMng();
	void Initialize();

	//===============================================
	//	更新と描画
	//===============================================
	void Update();
	void Render(iexShader *shader = nullptr, char *name = '\0');

	//===============================================
	//	ゲッター,セッター
	//===============================================
	void Append(Ambulance::MOVE_TYPE type, const Vector3 &start, const Vector3 &goal, float speed);

private:
	std::vector<Ambulance*> List;
	iex3DObj *clone;
};