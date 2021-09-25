//Maria Anna Kanellaki - 1115201400060

#include "DateList.h"
#include <iostream>
#include <string>
#include <cstring>

using namespace std;


DateNode::DateNode(PatientRecord *rec)        //initializes a DateNode with given Date
{
    record = rec;

    if (record->getEntryDate())
        date = record->getEntryDate();
    else
        date = record->getExitDate();

    myDiseases = new DiseaseList();           //initializes its disease list
    myDiseases->push(new DiseaseNode(rec));   //and pushes the 1st element
    next = NULL;
}


DateNode::~DateNode()                         //deletes next's pointer, without deleting that element
{
    //delete date;  //deleted by patient list
    delete myDiseases;
    next = NULL;
    record = NULL;
    delete next;
}


DateList::DateList()                          //initializes empty list
{
    head = NULL;
}


DateList::~DateList()                         //deletes whole list
{
    DateNode *curr, *temp = head;

    while (temp)
    {
        curr = temp;
        temp = temp->getNext();
        delete curr;
    }
}


DateNode* DateList::SearchDate(string d) //searches for a DateNode with date = given d
{
    DateNode *dat = head;

    while (dat)
    {
        if (dat->getDate()->getAsString() == d)
            return dat;

        dat = dat->getNext();
    }
    delete dat;
    return NULL;
}


void DateList::push(DateNode *dat)            //insertion at the beginning of the list for speed
{
    DateNode *temp;
    if ((temp = SearchDate(dat->getDate()->getAsString())))                      //ignores duplicate dates
    {
        temp->getMyDiseases()->push(new DiseaseNode(dat->getRecord()));          //insert new disease in my diseases list
        return;
    }
    delete temp;

    if (head)
        dat->setNext(head);

    head = dat;
}


//getters and setters for both classes
DateNode *DateNode::getNext() const
{
    return next;
}


void DateNode::setNext(DateNode *next)
{
    DateNode::next = next;
}


Date *DateNode::getDate() const
{
    return date;
}


PatientRecord *DateNode::getRecord() const
{
    return record;
}


DiseaseList *DateNode::getMyDiseases()
{
    return myDiseases;
}


DateNode *DateList::getHead()
{
    return head;
}
