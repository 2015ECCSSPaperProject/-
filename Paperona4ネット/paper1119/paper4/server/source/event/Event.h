
#pragma once

// 指定したタイミングでイベントを発生させる
class Event
{
public:
	Event();
	Event(int time);
	virtual ~Event(){};

	virtual void Start() = 0;
	// イベントが発生する時間<ゲーム開始からの経過時間 (1/1000秒)>
	void Set_time(unsigned int time);
	// 更新
	virtual bool Update();

protected:
	;
	unsigned int start_time;
};



class Area;
// エリアを解放
class Event_open_area : public Event
{
public:
	Event_open_area(Area *area);
	Event_open_area(int time, Area *area);

	void Start();

private:
	Area *area;
};



// イベントをまとめる
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
