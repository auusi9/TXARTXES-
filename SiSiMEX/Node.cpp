#include "Node.h"


Node::Node()
{
}


Node::~Node()
{
}


void Node::initialize()
{
	// Initialize items
	_itemList.randomInitialization();
}

bool Node::HasItem(int item)
{
	for (auto items : _itemList.items) 
	{
		if (items == item)
			return true;
	}

	return false;
}
