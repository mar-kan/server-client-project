//Kanellaki Maria Anna - 1115201400060

#ifndef PATIENTRECORD_H
#define PATIENTRECORD_H

#include "Date.h"

class PatientList;

class PatientRecord {                                   //contains a patient's data

    string recordID;
    string status;                                      //ENTER or EXIT
    string patientFirstName;
    string patientLastName;
    string diseaseID;
    string country;
    Date *entryDate;
    Date *exitDate;
    int age;

    bool valid;

public:
    PatientRecord(char *, string, char*);
    ~PatientRecord();

    const string &getRecordId() const;
    Date *getEntryDate() const;
    const string &getDiseaseId() const;
    const string &getCountry() const;
    Date *getExitDate() const;
    bool isValid() const;
    void setExitDate(Date *exitDate);
    string getWhole();

    int getAge() const;
    string &getStatus();
};


#endif