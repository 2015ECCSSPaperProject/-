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
	Vector3 GetPos();


private:
	Vector3 pos;
	Vector3 start_pos;
	Vector3 goal_pos;	// この座標まで来たら方向を変える
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


	//===============================================
	//	判定
	//===============================================
	int CheckDist(const Vector3 &pos, float dist);
	Ambulance *GetAmbulance(int no);

private:
	std::vector<Ambulance*> List;
	iex3DObj *clone;

	void Load(LPSTR filename);
};

extern AmbulanceMng *ambulance_mng;