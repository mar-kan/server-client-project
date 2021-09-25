//Maria Anna Kanellaki - 1115201400060

#ifndef DATELIST_H
#define DATELIST_H

#include "PatientRecord.h"
#include "Date.h"
#include "DiseaseList.h"

class DateNode {                                  //stores date, record and a list of diseases
    Date * date;
    PatientRecord * record;
    DiseaseList * myDiseases;                     //list that stores all diseases recorded for a date

    DateNode * next;

public:
    DateNode(PatientRecord*);
    ~DateNode();

    DateNode * getNext() const;
    void setNext(DateNode *next);
    Date *getDate() const;
    PatientRecord *getRecord() const;
    DiseaseList *getMyDiseases();
};


class DateList {                                    //list that stores all dates a worker read successfully
    DateNode * head;

public:
    DateList();
    void push(DateNode*);
    DateNode * SearchDate(string);
    ~DateList();

    DateNode *getHead();
};


#endif