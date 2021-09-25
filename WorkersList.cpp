//Kanellaki Maria Anna - 1115201400060

#include "WorkersList.h"


WorkerNode::WorkerNode(int rfd, string ip)
{
    //values taken from constructor from whoServer
    read_fd = rfd;
    IP = ip;

    //both values set later by thread function
    port = -1;
    write_fd = -1;

    next = NULL;
}


WorkerNode::~WorkerNode()
{
    next = NULL;
    delete next;
}


WorkersList::WorkersList()
{
    head = NULL;
}


WorkersList::~WorkersList()
{
    WorkerNode *curr, *temp = head;

    while (temp)
    {
        curr = temp;
        temp = temp->getNext();
        delete curr;
    }
}


void WorkersList::push(WorkerNode *node)
{
    if (head)
        node->setNext(head);

    head = node;
}


WorkerNode* WorkersList::searchFd(int fd)
{
    WorkerNode *node = head;

    while (node)
    {
        if (node->getReadFd() == fd)
            return node;

        node = node->getNext();
    }
    return NULL;
}


//getters & setters
WorkerNode *WorkerNode::getNext() const
{
    return next;
}


void WorkerNode::setNext(WorkerNode *next)
{
    WorkerNode::next = next;
}


int WorkerNode::getReadFd() const
{
    return read_fd;
}


int WorkerNode::getPort() const
{
    return port;
}


const string &WorkerNode::getIp() const
{
    return IP;
}


int WorkerNode::getWriteFd() const
{
    return write_fd;
}


WorkerNode *WorkersList::getHead() const
{
    return head;
}


void WorkerNode::setPort(int port)
{
    WorkerNode::port = port;
}


void WorkerNode::setWriteFd(int writeFd)
{
    write_fd = writeFd;
}
