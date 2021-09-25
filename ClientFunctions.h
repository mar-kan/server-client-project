//Kanellaki Maria Anna - 1115201400060

#ifndef SERVERFUNCTIONS_H
#define SERVERFUNCTIONS_H

#include <string>
#include <pthread.h>
#include "BoundedBuffer.h"
#include "ArgumentStruct.h"
using namespace std;


int check_error(int, string);
void free_client_memory(int, pthread_t*);
void *send_query(void*);

#endif
