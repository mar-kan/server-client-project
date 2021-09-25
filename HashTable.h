//Kanellaki Maria Anna - 1115201400060

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "PatientRecord.h"
#include "Tree.h"

class BucketEntry {                             //represents a disease/country entry of a bucket

    string key;                                 //diseaseType or country
    BucketEntry * nextEntry;

public:
    BucketEntry(PatientRecord *, string);
    ~BucketEntry();

    BucketEntry *getNextEntry() const;
    void setNextEntry(BucketEntry *nextEntry);
    const string &getKey() const;
};


class Bucket {                                  //represents a bucket of a hash table

    int remainingSpace;

    Tree * tree;
    BucketEntry * first;
    Bucket * nextBucket;

public:
    Bucket(int);
    void InsertInBucket(PatientRecord*, int bucketSize, string, treeNode*);
    ~Bucket();

    BucketEntry *getFirst() const;
    Bucket *getNextBucket() const;
    Tree *getTree() const;
};


class HashTable {                               //represents a hash table with size[size] and bucket size [bucketSize] (in bytes).

    int size;
    int bucketSize;

    Bucket ** table;

public:
    HashTable(int, int);
    void InsertToTable(PatientRecord*, string);
    int HashFunction(string);

    int CountAll(string, Date*, Date*);
    int CountAllCountry(string, string, Date*, Date*);
    int CountEnterOrExit(string, Date*, Date*, string);
    int CountEnterOrExitCountry(string, string, Date*, Date*, string);
    ~HashTable();

    int getSize() const;
    int getBucketSize() const;

    Bucket **getTable() const;
};


#endif