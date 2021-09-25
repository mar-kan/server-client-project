//Kanellaki Maria Anna - 1115201400060

#include <errno.h>
#include <iostream>
#include <zconf.h>
#include <wait.h>
#include <cstring>
#include <assert.h>
#include <string>
#include <arpa/inet.h>
#include "ClientFunctions.h"
#include "ServerFunctions.h"
#include "WorkersList.h"

#define socket_buff_size 500

extern pthread_mutex_t mutexs;
extern pthread_cond_t cond_read;
extern pthread_mutex_t mutex_print;
extern pthread_mutex_t mutex_write;
extern pthread_mutex_t mutex_read;
extern pthread_mutex_t mutex_worker_1;
extern pthread_mutex_t mutex_worker_2;
extern pthread_mutex_t mutex_list;

extern WorkersList * myWorkers;
extern bool terminate_program;

int check_error(int error, string message)                              //checks for perror and if there is exits. else returns its int argument
{
    if (error < 0)
    {
        perror(message.c_str());
        exit(error);
    }
    return error;
}


void free_server_memory(int server_socket, int worker_socket, int arr_size, pthread_t* threadArray)      //frees all memory of server
{
    pthread_mutex_destroy(&mutexs);
    pthread_mutex_destroy(&mutex_list);
    pthread_mutex_destroy(&mutex_print);
    pthread_mutex_destroy(&mutex_write);
    pthread_mutex_destroy(&mutex_read);
    pthread_mutex_destroy(&mutex_worker_1);
    pthread_mutex_destroy(&mutex_worker_2);

    delete[] threadArray;
    close(server_socket);
    close(worker_socket);
}


void* thread_function(void *args)                                 //reads from socket and prints every string read
{
    arguments * argus = (arguments*)args;                         //takes buffer as argument
    BoundedBuffer * buff = argus->buffer;
    //free(argus->message);
    //free(argus);

    //while(true)
    {
        //remove fd from buffer
        int fd = buff->remove();

        int portNum = -1;
        char msg[socket_buff_size];

        //read from fd
        while (read(fd, msg, socket_buff_size) > 0) //reads until 'ready!' is received
        {
            msg[strlen(msg)] = '\0';
            string temp = msg;

            if (temp.find("port:") != string::npos)
            {
                temp.erase(0, 5);               //erases identifier "port:" from string
                assert(portNum = atoi(temp.c_str()));

                WorkerNode * wnode = myWorkers->searchFd(fd);
                if (wnode)
                {
                    pthread_mutex_lock(&mutex_list);
                    wnode->setPort(portNum);
                    pthread_mutex_unlock(&mutex_list);
                }
                continue;
            }
            if (strcmp(msg, "ready!") == 0)
                break;

            //prints every message received
            pthread_mutex_lock(&mutex_print);            //locks before print to not mix outputs
            cout<<msg<<endl;
            pthread_mutex_unlock(&mutex_print);

            if (portNum > 0)                             //means this is a statistics msg
                continue;                                //server doesn't answer anywhere

            /**this part is for queries**/
            WorkerNode * node = myWorkers->getHead();
            while (node)                                 //sends query to every worker
            {
                //create new socket to send to worker
                int new_fd = check_error(socket(AF_INET, SOCK_STREAM, 0), "create new socket");

                struct sockaddr_in client;
                int wport = node->getPort();
                string IP = node->getIp();

                client.sin_family = AF_INET;
                client.sin_addr.s_addr = inet_addr(IP.c_str());
                client.sin_port = htons(wport);

                //if (node->getWriteFd() < 0)
                {
                    check_error(connect(new_fd, (struct sockaddr *)&client, sizeof(client)), "connect to new socket");

                    pthread_mutex_lock(&mutex_list);
                    node->setWriteFd(new_fd);
                    pthread_mutex_unlock(&mutex_list);
                }
/*                else
                    new_fd = node->getWriteFd();                    //has already connected*/

                //pthread_mutex_lock(&mutex_worker_1);
                check_error(write(new_fd, msg, socket_buff_size), "send query to worker");
                //pthread_mutex_unlock(&mutex_worker_1);

                //receives worker's answer
                pthread_mutex_lock(&mutex_worker_2);
                int rfd = node->getReadFd();
                check_error(read(rfd, msg, socket_buff_size), "read worker reply");
                pthread_mutex_unlock(&mutex_worker_2);

                //prints worker's answer                            //locks before print to not mix outputs
                pthread_mutex_lock(&mutex_print);
                cout << msg << endl;
                pthread_mutex_unlock(&mutex_print);

                //sends answer to client
                pthread_mutex_lock(&mutex_write);
                check_error(write(fd, msg, socket_buff_size), "answer to client");      //writes a query to the socket
                pthread_mutex_unlock(&mutex_write);

                close(new_fd);
                node = node->getNext();
            }
        }
    }
}


void sigkillServer(int signaling)
{
    terminate_program = true;
    signal(SIGINT, &sigkillServer);
    signal(SIGQUIT, &sigkillServer);
}