//Kanellaki Maria Anna - 1115201400060

#ifndef SYSPRO3_SERVERFUNCTIONS_H
#define SYSPRO3_SERVERFUNCTIONS_H

#include <string>
#include <pthread.h>
#include "BoundedBuffer.h"
#include "ArgumentStruct.h"
using namespace std;

//functions used in server executable

int check_error(int, string);
void free_server_memory(int, int, int, pthread_t*);
void *thread_function(void*);


#endif
