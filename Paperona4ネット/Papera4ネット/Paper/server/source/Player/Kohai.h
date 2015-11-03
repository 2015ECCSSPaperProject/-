
#pragma once

class BasePlayer;

class Kohai : public BasePlayer
{
public:
	Kohai(int id);
	~Kohai() override;
	void Release() override;

	void Initialize(iex3DObj *obj) override;
	void Update() override;

	int Get_action() override;
	void Change_action(int part) override;

private:
	enum class ACTION_PART
	{
		CHASE,
		PASTE,
		MAX
	}action_part;
	ActionBase<Kohai> *action[(unsigned int)ACTION_PART::MAX];

	class ActionChase : public ActionBase<Kohai>
	{
	public:
		ActionChase(Kohai*me);

		void Initialize() override;
		void Update() override;
	}action_chase;
	class ActionPaste : public ActionBase<Kohai>
	{
	public:
		ActionPaste(Kohai*me);

		void Initialize() override;
		void Update() override;

		// -1 : dist範囲内にポスターが無い
		int Nearest_poster(float dist);

	private:
		enum class PROCESS : unsigned char
		{
			WALK,
			PASTE
		}process;
		int timer;
		int target_poster;
	}action_paste;

	// Aに向かって移動
	void GotoA(const Vector3 &A);
	// distより離れてる時移動
	bool GotoA_far(const Vector3 &A, float dist);
	// distより近い時移動
	bool GotoA_near(const Vector3 &A, float dist);
};
