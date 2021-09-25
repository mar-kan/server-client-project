//Maria Anna Kanellaki - 1115201400060

#include <iostream>
#include <cstring>
#include "Date.h"


Date::Date(char * date)                                     //breaks string that represents date to int and stores it
{
    char delimiter[]="- \n";
    char * temp;

    valid = true;
    asString = date;

    if (!(temp = strtok(date, delimiter)))
        valid = false;
    else
        day = atoi(temp);

    if (!(temp = strtok(NULL, delimiter)))
        valid = false;
    else
        month = atoi(temp);

    if (!(temp = strtok(NULL, delimiter)))
        valid = false;
    else
        year = atoi(temp);

    if (!valid || year > 2020 || month > 12 || month < 1 || day > 31 || day < 1)
    {
        valid = false;
        //cout << "Error. Invalid date" << endl;
    }
}


bool Date::earlierThan(Date *date2)                         //checks if date of object is earlier than the given object's date
{
    if (date2->year < this->year)
        return false;

    if (date2->month < this->month && date2->year == this->year)
        return false;

    if (date2->day < this->day && date2->month == this->month && date2->year == this->year)
        return false;

    return true;
}


Date::~Date() {}


bool Date::isValid() const
{
    return valid;
}


const string &Date::getAsString() const
{
    return asString;
}