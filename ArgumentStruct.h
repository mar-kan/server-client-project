//Kanellaki Maria Anna - 1115201400060

#ifndef ARGUMENTSTRUCT_H
#define ARGUMENTSTRUCT_H

#include <string>
#include "BoundedBuffer.h"
using namespace std;

//argument struct for passing multiple arguments to pthread functions
typedef struct arguments{
    int port;
    char* message;
    BoundedBuffer * buffer;
    string IP;
}arguments;


#endif
