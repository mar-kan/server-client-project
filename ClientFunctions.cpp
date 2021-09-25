//Kanellaki Maria Anna - 1115201400060

#include <zconf.h>
#include <wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include "ClientFunctions.h"

extern pthread_mutex_t mutex;
extern pthread_cond_t cond_send;

#define socket_buff_size 500

int check_error(int error, string message)                                  //checks for perror and if there is exits. else returns its int argument
{
    if (error < 0)
    {
        perror(message.c_str());
        exit(EXIT_FAILURE);
    }
    return error;
}


void free_client_memory(int client_socket, pthread_t* threadArray)          //frees all memory the client allocated
{
    close(client_socket);
    pthread_mutex_destroy(&mutex);
    delete[] threadArray;
}


void * send_query(void * args)                                              //client threads' function. sends a query through a socket to the server
{
    arguments * argus = (arguments*)args;
    int port = argus->port;
    char msg[socket_buff_size];
    strcpy(msg, argus->message);
    string IP = argus->IP;

    //connect to server
    int sock = check_error(socket(AF_INET, SOCK_STREAM, 0), "create socket");
    struct sockaddr_in client;

    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr(IP.c_str());
    client.sin_port = htons(port);

    pthread_mutex_lock(&mutex);
    check_error(pthread_cond_wait(&cond_send, &mutex), "cond wait");//function waits here until all threads are created and main thread signals them to start sending

    check_error(connect(sock, (struct sockaddr *)&client, sizeof(client)), "connect to socket");

    check_error(write(sock, msg, socket_buff_size), "write query");      //writes a query to the socket
    free(argus->message);
    free(argus);
    pthread_mutex_unlock(&mutex);

    //read answer
    while (check_error(read(sock, msg, socket_buff_size),"read error") == 0)         //read message
        continue;
    msg[strlen(msg)] = '\0';

    //print answer
    pthread_mutex_lock(&mutex);
    cout << msg << endl;
    pthread_mutex_unlock(&mutex);

    close(sock);
    pthread_exit(NULL);                                                     //thread exits
}