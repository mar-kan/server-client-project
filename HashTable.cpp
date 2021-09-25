//Kanellaki Maria Anna - 1115201400060

#include <iostream>
#include "HashTable.h"


BucketEntry::BucketEntry(PatientRecord * rec, string myKey)     //creates new bucket entry for disease/country from patient record
{
    key = myKey;
    nextEntry = NULL;
}


BucketEntry::~BucketEntry()                                     //deletes one entry
{
    nextEntry = NULL;
    delete nextEntry;
}


Bucket::Bucket(int size)                                        //initializes empty bucket
{
    remainingSpace = size - 4 - 3*8;                            //subtracting the int (size) and the 3 pointers (tree, first and nextBucket)
    tree = new Tree();                                          //initializes empty tree for this disease/country
    first = NULL;
    nextBucket = NULL;
}


Bucket::~Bucket()                                               //deletes a bucket, the tree and its entries without touching the next Bucket
{
    delete tree;
    nextBucket = NULL;
    delete nextBucket;

    BucketEntry * temp, * entry = first;
    while (entry)
    {
        temp = entry;
        entry = entry->getNextEntry();
        delete temp;
    }
}


void Bucket::InsertInBucket(PatientRecord *rec, int bucketSize, string myKey, treeNode *nodeT) //creates new BucketEntry and inserts it in Bucket. creates new Bucket if needed.
{
    BucketEntry * entry = new BucketEntry(rec, myKey);

    this->tree->insertInTree(nodeT);                            //inserts node in Tree

    if (!first)
    {
        first = entry;
        remainingSpace -= sizeof(&entry);
        return;
    }

    Bucket * temp = nextBucket;

    if (remainingSpace - sizeof(&entry) < 0)                    //if there is not enough space creates new bucket and inserts entry there
    {
        while (temp->nextBucket)
        {
            if (remainingSpace - sizeof(&entry) < 0)
                temp = temp->nextBucket;
            else                                                //if program finds a bucket where there is enough space, entry is inserted there
            {
                remainingSpace -= sizeof(&entry);
                entry->setNextEntry(first);                     //insertion in the beginning of bucket for speed
                first = entry;
                return;
            }
        }
        //no bucket with enough space: creates new bucket and inserts there
        Bucket * newBucket = new Bucket(bucketSize);
        newBucket->InsertInBucket(rec, bucketSize, myKey, nodeT);
        nextBucket = newBucket;
    }
    else                                                        //enough space here
    {
        remainingSpace -= sizeof(&entry);
        entry->setNextEntry(first);                             //insertion in the beginning of bucket for speed
        first = entry;
    }
}


HashTable::HashTable(int s, int bs)                             //initializes empty hash table with size and bucketsize given by the command line
{
    size = s;
    bucketSize = bs;                                            //in bytes
    table = new Bucket*[size];

    for(int i = 0; i < size; i++)
        table[i] = NULL;
}


HashTable::~HashTable()                                         //deletes whole hash table
{
    int i;
    for (i=0; i<size; i++)
    {
        if (table[i])
            delete table[i];
    }
    delete[] table;
}


void HashTable::InsertToTable(PatientRecord *rec, string myKey) //hashes rec key to find its index. inserts it in index's bucket. creates one if needed
{
    Bucket * bucket;

    int index = HashFunction(myKey);                            //country or disease

    if (!table[index])                                          //creates new bucket if necessary
    {
        bucket = new Bucket(bucketSize);
        table[index] = bucket;
    }
    else
        bucket = table[index];

    treeNode * nodeT = new treeNode(rec);                       //creates tree node from patient record
    bucket->InsertInBucket(rec, bucketSize, myKey, nodeT);      //calls function to insert rec in bucket
}


int HashTable::HashFunction(string id)                          //simple modular hash function
{
    int i, multiplier=1;
    long long int  code=0;

    //builds ascii code of id. stops before overflow if string too big
    for (i=0; i<id.length(); i++)
    {
        char x = id.at(i);
        if ((code*multiplier + int(x)) < 0)
            return code % size;

        code = code*multiplier + int(x);
        multiplier *= 100;
    }
    return code % size;                                         //simple modular hash function
}


int HashTable::CountAll(string disease, Date* date1, Date* date2) //prints count of a disease stored in the hash table through their trees
{
    int value = HashFunction(disease);
    int count = 0;
    table[value]->getTree()->countDiseasedDates(table[value]->getTree()->getRoot(), date1, date2, &count);

    return count;
    //cout << disease <<" "<< count << endl;//" between "<<date1->getAsString()<<" and "<<date2->getAsString()<<"."<< endl;
}


int HashTable::CountAllCountry(string country, string disease, Date * date1, Date * date2) //prints count of a disease for a country stored in the hash table through their trees
{
    int value = HashFunction(country);
    int count = 0;
    table[value]->getTree()->countDiseasedCountry(table[value]->getTree()->getRoot(), date1, date2, disease, &count);

    //cout <<"Number of cases recorded in "<< country<<" for "<< disease <<": "<< count <<" between "<<date1->getAsString()<<" and "<<date2->getAsString()<<"."<< endl;
    return count;
}


int HashTable::CountEnterOrExit(string disease, Date* date1, Date* date2, string status) //prints count of a disease stored in the hash table through their trees
{                                                                                        //prints only entered or exited patients depending on status given
    int value = HashFunction(disease);
    int count = 0;
    table[value]->getTree()->countEnterOrExit(table[value]->getTree()->getRoot(), date1, date2, &count, status);

    return count;
    //cout << disease <<" "<< count << endl;//" between "<<date1->getAsString()<<" and "<<date2->getAsString()<<"."<< endl;
}


int HashTable::CountEnterOrExitCountry(string country, string disease, Date * date1, Date * date2, string status) //prints count of a disease for a country stored in the hash table through their trees
{                                                                                                                 //prints only entered or exited patients depending on status given
    int value = HashFunction(country);
    int count = 0;
    table[value]->getTree()->countEnterOrExitCountry(table[value]->getTree()->getRoot(), date1, date2, disease, &count, status);

    //cout <<"Number of cases recorded in "<< country<<" for "<< disease <<": "<< count <<" between "<<date1->getAsString()<<" and "<<date2->getAsString()<<"."<< endl;
    return count;
}


//setters and getters for all classes related to HashTable
BucketEntry *BucketEntry::getNextEntry() const
{
    return nextEntry;
}


void BucketEntry::setNextEntry(BucketEntry *nextEntry)
{
    BucketEntry::nextEntry = nextEntry;
}


int HashTable::getSize() const
{
    return size;
}


int HashTable::getBucketSize() const
{
    return bucketSize;
}


BucketEntry *Bucket::getFirst() const
{
    return first;
}


Bucket *Bucket::getNextBucket() const
{
    return nextBucket;
}


const string &BucketEntry::getKey() const
{
    return key;
}


Tree *Bucket::getTree() const
{
    return tree;
}


Bucket **HashTable::getTable() const
{
    return table;
}
