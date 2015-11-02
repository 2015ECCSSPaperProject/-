
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
		ActionChase(Kohai*me) :ActionBase(me){}

		void Initialize() override;
		void Update() override;
	};
	class ActionPaste : public ActionBase<Kohai>
	{
	public:
		ActionPaste(Kohai*me) :ActionPaste(me){}

		void Initialize() override;
		void Update() override;
		// -1 : dist範囲内にポスターが無い
		int Nearest_poster(float dist);

	private:
		int target_poster;
	};

};
