//Kanellaki Maria Anna - 1115201400060

#ifndef PATIENTLIST_H
#define PATIENTLIST_H

#include <string>
#include "Date.h"
#include "PatientRecord.h"
using namespace std;

class PatientNode {                               //a node of list that contains a patientRecord and list's next element

    PatientRecord * record;
    PatientNode * next;

public:
    PatientNode(PatientRecord*);
    ~PatientNode();

    PatientRecord * getRecord() const;
    PatientNode * getNext() const;
    void setNext(PatientNode *next);
};


class PatientList {                               //list that contains all patients

    PatientNode * head;

public:
    PatientList();
    void push(PatientNode*);
    PatientRecord * SearchRecord(string);
    ~PatientList();

    PatientNode *getHead();
};


#endif