
#pragma once

class Event;
// ƒCƒxƒ“ƒg‚ð‚Ü‚Æ‚ß‚é
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
