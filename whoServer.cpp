//Kanellaki Maria Anna - 1115201400060

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <assert.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "ServerFunctions.h"
#include "BoundedBuffer.h"
#include "ArgumentStruct.h"
#include "WorkersList.h"

using namespace std;

pthread_mutex_t mutexs =  PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_print = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_write = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_read = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_worker_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_worker_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_list = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_read = PTHREAD_COND_INITIALIZER;

bool terminate_program;
WorkersList * myWorkers = new WorkersList();                  //stores fds and ports of workers that are sending stats


int main(int argc, char *argv[])
{
    //checks arguments
    if (argc != 9 || strcmp(argv[1], "-q") != 0 || strcmp(argv[3], "-s") != 0 || strcmp(argv[5], "-w") != 0 || strcmp(argv[7], "-b") != 0)
    {
        cout <<"Wrong format. Program should be called like this: ./whoServer –q queryPortNum -s statisticsPortNum –w numThreads –b bufferSize"<< endl;
        exit(-1);
    }
    int queryPortNum, statisticsPortNum, numThreads, bufferSize;

    assert(queryPortNum = atoi(argv[2]));
    assert(statisticsPortNum = atoi(argv[4]));
    assert(numThreads = atoi(argv[6]));
    assert(bufferSize = atoi(argv[8]));

    terminate_program = false;      //flag used to terminate program

    //create server
    int client_socket, worker_socket;
    struct sockaddr_in client_addr, worker_addr;
    socklen_t clientlen, workerlen;
    struct sockaddr *clientptr=(struct sockaddr *)&client_addr, *workerptr = (struct sockaddr *)&worker_addr;

    //create sockets
    check_error(client_socket = socket(AF_INET, SOCK_STREAM, 0), "client socket");
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client_addr.sin_port = htons(queryPortNum);

    check_error(worker_socket = socket(AF_INET, SOCK_STREAM, 0), "worker socket");
    worker_addr.sin_family = AF_INET;
    worker_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    worker_addr.sin_port = htons(statisticsPortNum);

    //bind sockets
    check_error(bind(client_socket, clientptr, sizeof(client_addr)), "bind client");
    check_error(bind(worker_socket, workerptr, sizeof(worker_addr)), "bind worker");

    //listen on both sockets
    check_error(listen(client_socket, 10), "listen on client");
    check_error(listen(worker_socket, 10), "listen on worker");

    //initialize buffer
    BoundedBuffer * buffer = new BoundedBuffer(bufferSize);

    //initialize arguments' struct
    arguments *args = (arguments*)malloc(sizeof(arguments));
    args->message = NULL;
    args->buffer = buffer;

    //create numThreads threads
    pthread_t *myThreads = new pthread_t[numThreads];
    for (int i=0; i<numThreads; i++)
        pthread_create(&myThreads[i], NULL, thread_function, args);

    fd_set read_fd_set, write_fd_set, active_fd_set;
    FD_ZERO(&active_fd_set);
    FD_SET(client_socket, &active_fd_set);
    FD_SET(worker_socket, &active_fd_set);

    struct timeval time_expire;
    time_expire.tv_sec = 30;
    time_expire.tv_usec = 0;

    //accept summary stats first
    while (true)
    {
        //select a thread to read
        read_fd_set = active_fd_set;
        int res;
        if ((res = select(FD_SETSIZE, &read_fd_set, NULL, NULL, &time_expire)) <= 0)    //expires after a time period
        {
            if(res == 0)
                break;

            if(errno==EINTR)
                continue;

            perror ("select");
            exit (EXIT_FAILURE);
        }

        //accept and add every socket accepted to buffer
        int fd;
        for (int i=0; i<FD_SETSIZE; i++)
        {
            if (check_error(FD_ISSET(i, &read_fd_set), "FD_ISSET"))
            {
                if (i == worker_socket)
                {
                    fd = check_error(accept(worker_socket, workerptr, &workerlen), "accept worker fd");

                    pthread_mutex_lock(&mutex_list);
                    myWorkers->push(new WorkerNode(fd, inet_ntoa(worker_addr.sin_addr)));       //inserts every worker fd accepted to workerlist
                    pthread_mutex_unlock(&mutex_list);

                    buffer->add(fd);                                                            //with its file desc and worker's ip address
                }
            }
        }
        time_expire.tv_sec = 1;
    }

    while (true)
    {
        if (terminate_program)
            free_server_memory(client_socket, worker_socket, numThreads, myThreads);

        //select a thread to read
        read_fd_set = active_fd_set;
        write_fd_set = active_fd_set;
        if (select(FD_SETSIZE, &read_fd_set, &write_fd_set, NULL, NULL) < 0)
        {
            if(errno==EINTR)
                continue;

            perror ("select");
            exit (EXIT_FAILURE);
        }

        //accept and add every socket accepted to buffer
        int fd;
        for (int i=0; i<FD_SETSIZE; i++)
        {
            if (check_error(FD_ISSET(i, &read_fd_set), "FD_ISSET"))
            {
                if(i == client_socket)
                {
                    fd = check_error(accept(client_socket, clientptr, &clientlen), "accept client fd");
                    buffer->add(fd);
                }
            }
        }
    }
}