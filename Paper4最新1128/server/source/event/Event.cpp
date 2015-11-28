
#include "Event.h"
#include "../timer/Timer.h"
#include "../timer/Timer.h"

Event::Event() : start_time(0)
{};

Event::Event(int time) : start_time(time)
{}

void Event::Set_time(unsigned int time)
{
	start_time = time;
}

bool Event::Update()
{
	if (start_time <= timer->Get_delta())
	{
		Start();
		return true;
	}
	
	return false;
}



#include "../stage/Area.h"

Event_open_area::Event_open_area(int time, Area_mng *area, int index) : Event(time), area(area), index(index)
{}

void Event_open_area::Start()
{
	area->Open(index);
}



Event_list::Event_list() : root(nullptr)
{}

Event_list::~Event_list()
{
	if (root)
		All_node_deleat(root);
}

void Event_list::push(Event *in)
{
	root = new Node(in, nullptr, root);

	if (root->next)
		root->next->back = root;
}

void Event_list::Update()
{
	Node *node_pointer = root;
	while (true)
	{
		if (node_pointer == nullptr)
			break;

		if (node_pointer->ev->Update())
		{
			if (node_pointer == root)
			{
				node_pointer = root->next;
				delete root;
				root = node_pointer;
			}
			else
			{
				Node *next_pointer = node_pointer->next;
				node_pointer->back->next = node_pointer->next;
				delete node_pointer;
				node_pointer = next_pointer;
			}
		}
		else
			node_pointer = node_pointer->next;
	}
}

void Event_list::All_node_deleat(Node *root)
{
	if (root->next)
		All_node_deleat(root->next);
	delete root;
}



Event_list::Node::Node(Event *ev, Node *back, Node *next) : ev(ev), back(back), next(next)
{}

Event_list::Node::~Node()
{
	delete ev;
}

Event_list *event_list = nullptr;
