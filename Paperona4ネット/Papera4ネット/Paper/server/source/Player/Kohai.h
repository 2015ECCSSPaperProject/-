
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

		// -1 : dist�͈͓��Ƀ|�X�^�[������
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

	// A�Ɍ������Ĉړ�
	void GotoA(const Vector3 &A);
	// dist��藣��Ă鎞�ړ�
	bool GotoA_far(const Vector3 &A, float dist);
	// dist���߂����ړ�
	bool GotoA_near(const Vector3 &A, float dist);
};
