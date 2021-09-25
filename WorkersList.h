//Kanellaki Maria Anna - 1115201400060

#ifndef SYSPRO3_WORKERSLIST_H
#define SYSPRO3_WORKERSLIST_H

#include <string>
using namespace std;


class WorkerNode {
    int read_fd;
    string IP;

    int port;
    int write_fd;

    WorkerNode * next;

public:
    WorkerNode(int, string);
    ~WorkerNode();

    WorkerNode *getNext() const;
    void setNext(WorkerNode *next);
    int getReadFd() const;
    int getPort() const;
    const string &getIp() const;
    int getWriteFd() const;
    void setPort(int port);
    void setWriteFd(int writeFd);
};


class WorkersList {
    WorkerNode *head;

public:
    WorkersList();
    ~WorkersList();

    void push(WorkerNode*);
    WorkerNode * searchFd(int);
    WorkerNode *getHead() const;
};


#endif //SYSPRO3_WORKERSLIST_H
