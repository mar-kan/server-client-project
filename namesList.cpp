//Kanellaki Maria Anna - 1115201400060

#include <iostream>
#include "namesList.h"


nameNode::nameNode(string n)
{
    name = n;
    next = NULL;
}


nameNode::~nameNode()               //deletes only the pointer
{
    next = NULL;
    delete next;
}



namesList::namesList()
{
    head = NULL;
}


namesList::~namesList()             //deletes whole list and nodes
{
    nameNode *curr, *temp = head;

    while (temp)
    {
        curr = temp;
        temp = temp->getNext();
        delete curr;
    }
}


void namesList::push(nameNode *n)            //insertion at the beginning of the list for speed
{
    if (SearchName(n->getName()))            //ignores duplicate names
        return;

    if (head)
        n->setNext(head);

    head = n;
}


void namesList::pop(string n)
{
    nameNode * temp = head;
    if (head->getName() == n)
    {
        head = head->getNext();
        delete temp;
        return;
    }

    while (temp->getNext() && temp->getNext()->getName() != n)
        temp = temp->getNext();

    if (temp->getNext())
    {
        nameNode * del = temp->getNext();
        temp->setNext(del->getNext());
        delete del;
    }
    else
        cout << "Name "<<n<<" not found in subdir list"<< endl;
}


nameNode* namesList::SearchName(string id) //searches for a node with name = given id
{
    nameNode *node = head;

    while (node)
    {
        if (node->getName() == id)
            return node;

        node = node->getNext();
    }
    delete node;
    return NULL;
}


//getters and setters for both classes
nameNode *nameNode::getNext() const
{
    return next;
}


void nameNode::setNext(nameNode *next)
{
    nameNode::next = next;
}


string nameNode::getName() const
{
    return name;
}


nameNode *namesList::getHead() const
{
    return head;
}
