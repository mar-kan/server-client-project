//Kanellaki Maria Anna - 1115201400060

#include <iostream>
#include <cstring>
#include "PatientRecord.h"


PatientRecord::PatientRecord(char * line, string countryName, char *date)       //gets a string (whole line), breaks it and stores its data
{
    char delimit[]=" \t\n";
    char * temp;
    valid = true;

    if ((temp = strtok(line, delimit)))
        recordID = temp;
    else
        valid = false;

    if ((temp = strtok(NULL, delimit)))
    {
        if (strcmp(temp, "ENTER") == 0 || strcmp(temp, "EXIT")==0)
            status=temp;
        else
            valid=false;
    }
    else
        valid = false;

    if ((temp = strtok(NULL, delimit)))
        patientFirstName = temp;
    else
        valid = false;

    if ((temp = strtok(NULL, delimit)))
        patientLastName = temp;
    else
        valid = false;

    if ((temp = strtok(NULL, delimit)))
        diseaseID = temp;
    else
        valid = false;

    if ((temp = strtok(NULL, delimit)))
    {
        age=atoi(temp);
        if (age > 120)
            valid=false;
    }
    else
        valid=false;

    country = countryName;

    Date * d = new Date(date);
    if (!d->isValid())
        valid=false;

    if (!valid || strtok(NULL, delimit))
    {
        //cout << "Error. Invalid patient record"<<endl;
        entryDate = NULL;
        exitDate = NULL;
        valid = false;
        return;
    }

    if (status == "ENTER")
    {
        entryDate = d;
        exitDate = NULL;
    }
    else
    {       //stores it like this and error checks it before the insertions
        exitDate = d;
        entryDate = NULL;
    }
}


PatientRecord::~PatientRecord()
{
    delete entryDate;
    delete exitDate;
}


string PatientRecord::getWhole()    //returns all data of record as string
{
    if (exitDate)
        return recordID+" "+patientFirstName+" "+patientLastName+" "+diseaseID+" "+to_string(age)+" "+entryDate->getAsString()+" "+exitDate->getAsString();
    else
        return recordID+" "+patientFirstName+" "+patientLastName+" "+diseaseID+" "+to_string(age)+" "+entryDate->getAsString();
}


//getters
const string &PatientRecord::getRecordId() const
{
    return recordID;
}


Date *PatientRecord::getEntryDate() const
{
    return entryDate;
}


const string &PatientRecord::getDiseaseId() const
{
    return diseaseID;
}


const string &PatientRecord::getCountry() const
{
    return country;
}


void PatientRecord::setExitDate(Date *exitDate)
{
    PatientRecord::exitDate = exitDate;
}


Date *PatientRecord::getExitDate() const
{
    return exitDate;
}


bool PatientRecord::isValid() const
{
    return valid;
}


int PatientRecord::getAge() const
{
    return age;
}


string &PatientRecord::getStatus()
{
    return status;
}
