//Kanellaki Maria Anna - 1115201400060

#include "PatientList.h"
#include <iostream>
#include <string>
#include <cstring>

using namespace std;


PatientNode::PatientNode(PatientRecord *rec)        //initializes a record with given PatientRecord
{
    record = rec;
    next = NULL;
}


PatientNode::~PatientNode()                         //deletes next's pointer, without deleting that element, and record
{
    next = NULL;
    delete next;
    delete record;
}


PatientList::PatientList()                          //initializes empty list
{
    head = NULL;
}


PatientList::~PatientList()                         //deletes whole list
{
    PatientNode *rec, *temp = head;

    while (temp)
    {
        rec = temp;
        temp = temp->getNext();
        delete rec;                                 //PatientRecords are deleted only by the list
    }
}


PatientRecord* PatientList::SearchRecord(string id) //searches for a record with recordID = given id
{
    PatientNode *rec = head;

    while (rec)
    {
        if (rec->getRecord()->getRecordId() == id)
            return rec->getRecord();

        rec = rec->getNext();
    }
    delete rec;
    return NULL;
}


void PatientList::push(PatientNode *rec)            //insertion at the beginning of the list for speed
{
    if (SearchRecord(rec->getRecord()->getRecordId()))
    {
        cout << "ERROR" << endl;
        return;
    }

    if (head)
        rec->setNext(head);

    head = rec;
}


//getters and setters for both classes
PatientRecord *PatientNode::getRecord() const
{
    return record;
}


PatientNode *PatientNode::getNext() const
{
    return next;
}


void PatientNode::setNext(PatientNode *next)
{
    PatientNode::next = next;
}


PatientNode *PatientList::getHead()
{
    return head;
}
