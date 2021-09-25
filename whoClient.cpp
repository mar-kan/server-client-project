//Kanellaki Maria Anna - 1115201400060

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <dirent.h>
#include <sys/wait.h>
#include <assert.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ClientFunctions.h"
#include "ArgumentStruct.h"

using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_send = PTHREAD_COND_INITIALIZER;


int main(int argc, char *argv[])
{
    //checks arguments
    if (argc != 9 || strcmp(argv[1], "-q") != 0 || strcmp(argv[3], "-w") != 0 || strcmp(argv[5], "-sp") != 0 || strcmp(argv[7], "-sip") != 0)
    {
        cout <<"Wrong format. Should be called like this: ./whoClient –q queryFile -w numThreads –sp servPort –sip servIP"<< endl;
        exit(-1);
    }
    int numThreads, servPort;

    string queryFile = argv[2];
    assert(numThreads = atoi(argv[4]));
    assert(servPort = atoi(argv[6]));
    string servIP = argv[8];

    //client's threads' array
    pthread_t *myThreads = new pthread_t[numThreads];
    int count_threads=0;

    //open query file for reading
    FILE *qfile = fopen(queryFile.c_str(), "r");
    if (!qfile)
    {
        cout << "Can't read file "<< queryFile << endl;
        exit(-1);
    }
    char * line = NULL;
    size_t size = 0;
    while (getline(&line, &size, qfile) != -1)
    {
        if (count_threads == numThreads)                                                //assumes that numThreads' num inputed is equal to the num of lines of the query file
        {
            cout << "Wrong thread number given"<< endl;
            exit(-2);
        }
        line[strlen(line)-1]='\0';  //replaces \n with \0

        //initialize arguments' struct
        arguments *args = (arguments*)malloc(sizeof(arguments));
        args->port = servPort;
        args->message = (char*)malloc(strlen(line)+1);
        strcpy(args->message, line);
        args->buffer = NULL;
        args->IP = servIP;

        //create one thread for each line
        pthread_create(&myThreads[count_threads], NULL, &send_query, args);
        count_threads++;

        strcpy(line, "");
    }
    free(line);
    fclose(qfile);

    //signal to threads to start sending data
    for (int i=0; i<numThreads; i++)
    {
        pthread_mutex_lock(&mutex);
        check_error(pthread_cond_signal(&cond_send), "signal");
        pthread_mutex_unlock(&mutex);
    }

    //wait for all threads to finish and exit
    for (int i=0; i<numThreads; i++)
        pthread_join(myThreads[i], NULL);

    //free_client_memory(sock, myThreads);
    exit(0);
}