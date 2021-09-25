//Kanellaki Maria Anna - 1115201400060

#include "DiseaseList.h"


DiseaseNode::DiseaseNode(PatientRecord *rec)        //initializes a DiseaseNode with given disease id
{
    disease = rec->getDiseaseId();

    for (int & age_stat : age_stats)                //initializes array to 0s
        age_stat = 0;

    AddAgeToStats(rec->getAge());                   //increases the age range where rec's age belongs

    next = NULL;
}


DiseaseNode::~DiseaseNode()                         //deletes next's pointer, without deleting that element
{
    next = NULL;
    delete next;
}


void DiseaseNode::AddAgeToStats(int age)            //increases counter of the age range where age belongs in
{
    if (age <= 20)
        age_stats[0]++;
    else if (age <= 40)
        age_stats[1]++;
    else if (age <= 60)
        age_stats[2]++;
    else
        age_stats[3]++;
}


DiseaseList::DiseaseList()                          //initializes empty list
{
    head = NULL;
}


DiseaseList::~DiseaseList()                         //deletes whole list
{
    DiseaseNode *curr, *temp = head;

    while (temp)
    {
        curr = temp;
        temp = temp->getNext();
        delete curr;
    }
}


void DiseaseList::push(DiseaseNode *dnode)            //insertion at the beginning of the list for speed
{
    if (SearchDisease(dnode->getDisease()))           //ignores duplicate entries
        return;

    if (head)
        dnode->setNext(head);

    head = dnode;
}


DiseaseNode * DiseaseList::SearchDisease(string name)
{
    DiseaseNode *dnode = head;
    while (dnode)
    {
        if (dnode->getDisease() == name)
            return dnode;

        dnode = dnode->getNext();
    }
    delete dnode;
    return NULL;
}


string DiseaseList::calculateTopkAges(int k, string disease)
{
    DiseaseNode *temp = SearchDisease(disease);
    string str;
    if (!temp)
        return nullptr;


    return str;
}


//getters and setters for both classes
DiseaseNode *DiseaseNode::getNext() const
{
    return next;
}


void DiseaseNode::setNext(DiseaseNode *next)
{
    DiseaseNode::next = next;
}


string DiseaseNode::getDisease() const
{
    return disease;
}


DiseaseNode *DiseaseList::getHead() const
{
    return head;
}


int DiseaseNode::getAgeStats(int pos) {
    return age_stats[pos];
}
