//Maria Anna Kanellaki - 1115201400060

#include <string>
#include <stdlib.h>
using namespace std;

#ifndef DATE_H
#define DATE_H


class Date {                            //stores a date in ints to facilitate error checking

    string asString;
    int day;
    int month;
    int year;
    bool valid;

public:
    Date(char *);
    bool earlierThan(Date*);
    ~Date();

    bool isValid() const;
    const string &getAsString() const;
};


#endif
