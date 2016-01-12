
#include "Event list.h"

#include "Event.h"

Event_list::Event_list() : root(nullptr)
{}

Event_list::~Event_list()
{
	if (root)
		Deleat_all_node( root );
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

void Event_list::Deleat_all_node( Node *root )
{
	if (root->next)
		Deleat_all_node( root->next );
	delete root;
}



Event_list::Node::Node(Event *ev, Node *back, Node *next) : ev(ev), back(back), next(next)
{}

Event_list::Node::~Node()
{
	delete ev;
}

Event_list *event_list = nullptr;
