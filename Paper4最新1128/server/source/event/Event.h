
#pragma once

// �w�肵���^�C�~���O�ŃC�x���g�𔭐�������
class Event
{
public:
	Event();
	Event(int time);
	virtual ~Event(){};

	virtual void Start() = 0;
	// �C�x���g���������鎞��<�Q�[���J�n����̌o�ߎ��� (1/1000�b)>
	void Set_time(unsigned int time);
	// �X�V
	virtual bool Update();

protected:
	;
	unsigned int start_time;
};



class Area_mng;
// �G���A�����
class Event_open_area : public Event
{
public:
	Event_open_area(int time, Area_mng *area, int index);

	void Start();

private:
	int index;
	Area_mng *area;
};



// �C�x���g���܂Ƃ߂�
class Event_list
{
public:
	Event_list();
	~Event_list();

	void push(Event *in);

	void Update();

private:
	class Node
	{
	public:
		Node(Event *ev, Node *back, Node *next);
		~Node();

		Event *ev;
		Node *back, *next;
	}*root;

	void All_node_deleat(Node *root);
};



extern Event_list *event_list;
