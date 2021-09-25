//Kanellaki Maria Anna - 1115201400060

#ifndef NAMESLIST_H
#define NAMESLIST_H

#include <string>
using namespace std;

class nameNode {

    string name;
    nameNode *next;

public:
    nameNode(string);
    ~nameNode();

    string getName() const;
    nameNode * getNext() const;
    void setNext(nameNode *next);
};
class namesList {                                               //list created to store the names of all the subdirectories

    nameNode * head;

public:
    namesList();
    ~namesList();
    void push(nameNode*);
    void pop(string);
    nameNode * SearchName(string);

    nameNode *getHead() const;
};


#endif
