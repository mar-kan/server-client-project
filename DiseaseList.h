//Kanellaki Maria Anna - 1115201400060


#ifndef DISEASELIST_H
#define DISEASELIST_H

#include <string>
#include "PatientRecord.h"

using namespace std;

class DiseaseNode {                               //a node of list that contains a disease name, an array with counts for it
                                                  // and list's next element
    string disease;
    int age_stats[4];                             //1 position for each age group

    DiseaseNode * next;

public:
    DiseaseNode(PatientRecord*);
    ~DiseaseNode();
    void AddAgeToStats(int);

    string getDisease() const;
    DiseaseNode * getNext() const;
    void setNext(DiseaseNode *next);
    int getAgeStats(int);
};


class DiseaseList {                               //list that contains all diseases of a file (a date)

    DiseaseNode * head;

public:
    DiseaseList();
    ~DiseaseList();
    void push(DiseaseNode*);
    DiseaseNode * SearchDisease(string);
    string calculateTopkAges(int, string);

    DiseaseNode *getHead() const;
};


#endif