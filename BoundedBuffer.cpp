//Kanellaki Maria Anna - 1115201400060

#include <cstdlib>
#include <iostream>
#include "BoundedBuffer.h"
#include "ServerFunctions.h"

using namespace std;

pthread_cond_t  cond_not_full = PTHREAD_COND_INITIALIZER,
               cond_not_empty = PTHREAD_COND_INITIALIZER;

extern pthread_mutex_t mutexs;
extern pthread_cond_t cond_read;


BoundedBuffer::BoundedBuffer(int s)                 //constructor takes size of buffer array as an argument
{
    size = s;
    files = new int[size];                          //array of fds

    start = 0;
    end = -1;
    count = 0;
}


BoundedBuffer::~BoundedBuffer()
{
    delete[] files;
}


void BoundedBuffer::add(int fd)                     //adds items to buffer
{
    pthread_mutex_lock(&mutexs);

    if (count >= size)
        pthread_cond_wait(&cond_not_full, &mutexs);

    files[end+1] = fd;
    end = (end+1) % size;
    count++;

    pthread_cond_signal(&cond_not_empty);
    pthread_mutex_unlock(&mutexs);
}


int BoundedBuffer::remove()                     //removes items from buffer
{
    pthread_mutex_lock(&mutexs);
    if (count <= 0) {
        pthread_cond_wait(&cond_not_empty, &mutexs);
    }
    int fd = files[start];
    start = (start+1) % size;
    count--;

    pthread_cond_signal(&cond_not_full);
    pthread_mutex_unlock(&mutexs);
    return fd;
}



int BoundedBuffer::getSize() const
{
    return size;
}
