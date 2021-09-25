//Maria Anna Kanellaki - 1115201400060

#include <iostream>
#include <cerrno>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/wait.h>
#include <assert.h>

#include "Date.h"
#include "namesList.h"
#include "MasterWorkerFunctions.h"
using namespace std;


string fifo = "fifo_to_worker";
string receive_fifo = "fifo_to_aggr";
int signal_id = 0;


int main(int argc, char* argv[]) {
    //checks arguments
    if (argc != 11 || strcmp(argv[1], "-w") != 0 || strcmp(argv[3], "-b") != 0 || strcmp(argv[5], "-s") != 0 ||
        strcmp(argv[7], "-p") != 0 || strcmp(argv[9], "-i") != 0)
    {
        cout<<"Wrong format. Program should be called like this: ./master –w numWorkers -b bufferSize –s serverIP –p serverPort -i input_dir"<<endl;
        exit(-1);
    }

    int bufferSize, numWorkers, serverPort;

    assert(numWorkers = atoi(argv[2]));
    assert(bufferSize = atoi(argv[4]));
    string serverIP = argv[6];
    assert(serverPort = atoi(argv[8]));
    char *input_dir = argv[10];

    //signal handler struct
    struct sigaction sig_act;
    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_flags = SA_SIGINFO | SA_RESTART;
    sig_act.sa_sigaction = handle_signal;

    int i, j;
    pid_t cpids[numWorkers];                                          //array that stores ids of children that are created
    int fd_to[numWorkers];                                            //array that stores fds of fifos
    int fd_from[numWorkers];                                          //array that stores fds of receiver fifos
    bool ready[numWorkers];                                           //array that stores if a worker is ready or not
    namesList *allCountries[numWorkers];                             //array that stores a list of countries for each worker
    namesList *dir_names = new namesList;                            //list that stores all subdir names

    for (i = 0; i < numWorkers; i++) {
        allCountries[i] = new namesList();
        ready[i] = true;
    }

    //counts subdirectories and creates a list with their names
    int count = 0;                                                      //counts subdirectories
    DIR *d;
    d = opendir(input_dir);
    if (!d) {
        perror("open: input dir");
        exit(1);
    }

    struct dirent *dir;
    while ((dir = readdir(d)) != nullptr) {
        if (strcmp(dir->d_name, "..") == 0 || strcmp(dir->d_name, ".") == 0)
            continue;
        nameNode *dirNode = new nameNode(dir->d_name);
        dir_names->push(dirNode);
        count++;
    }

    //creates one fifo for each worker to send dir names
    for (i = 0; i < numWorkers; i++) {
        if (mkfifo((fifo + to_string(i)).c_str(), 0666) == -1) {
            if (errno != EEXIST) {
                perror("receiver: mkfifo");
                exit(2);
            }
        }
    }

    //calculates files per worker considering the case where some workers take less than others
    int dirs_per_folder = count / numWorkers;
    int modular = count % numWorkers;
    if (modular)
        dirs_per_folder++;

    pid_t pid;
    bool flag = false;
    for (i = 0; i < numWorkers; i++) {
        //creates numWorkers processes from parent which execute "Worker" executable
        pid = fork();
        cpids[i] = pid;                                               //stores child id
        if (pid == -1) {
            perror("Fork error");
            exit(6);
        }
        if (pid != 0)   //parent writes to each child's fifo and continues to make workers
        {
            //opens each fifo for write
            fd_to[i] = open((fifo + to_string(i)).c_str(), O_WRONLY);
            if (fd_to[i] == -1) {
                perror("opening fifo");
                exit(3);
            }
            if (!flag && modular && i >= modular)    //when modulars are covered the subdirs per worker are reduced
            {                                       //happens only once
                dirs_per_folder--;
                flag = true;
            }

            int size;
            for (j = 0; j < dirs_per_folder; j++)       //writes dirs_per_folder dir names in fifo
            {
                //for one dir writes: sizeof dir name and dir name
                write_string_to_fifo(fd_to[i], bufferSize, dir_names->getHead()->getName());    //writes 1st element
                allCountries[i]->push(new nameNode(dir_names->getHead()->getName()));           //adds it to worker's position in array
                dir_names->pop(dir_names->getHead()->getName());                                //and pops it
            }

            /** added IP address and port num to data sent to every worker **/
            write_string_to_fifo(fd_to[i], bufferSize, "IP:" + serverIP);                         //passes "IP" with string to distinguish it from the countries' names
            write_string_to_fifo(fd_to[i], bufferSize, "port:" + to_string(serverPort));
            write_string_to_fifo(fd_to[i], bufferSize, "ready!");                               //writes ready when done
            ready[i] = false;                                                                   //worker busy with reading these
        } else            //child writes dirs_per_folder subdir names in fifo and execs worker main
            execl("./worker", "./worker", "-b", argv[4], "-i", argv[10], "-f", (fifo + to_string(i)).c_str(), NULL);
    }

    /**added signal handling for SIGCHLD (signal handling wasn't implemented in project 2)**/
    struct sigaction child_act;
    sigemptyset(&child_act.sa_mask);
    child_act.sa_flags = SA_SIGINFO | SA_RESTART;
    child_act.sa_sigaction = handle_signal;
    if (sigaction(SIGCHLD, &child_act, NULL) == -1)
        cout << "Error with sigaction: " << errno << endl;
    signal(SIGPIPE, SIG_IGN);


    while (true)
    {
        sleep(5);
        //stays awake to handle SIGCHLD
    }

    //kills children and frees all memory
    int status;
    for (i=0; i<numWorkers; i++)
    {
        if(kill(cpids[i], SIGKILL) == -1)
        {
            perror("kill error");
            exit(20);
        }
        close(fd_to[i]);
        close(fd_from[i]);
        unlink((fifo+to_string(i)).c_str());
        unlink((receive_fifo+to_string(i)).c_str());

        delete allCountries[i];
        wait(&status);
    }
    delete dir_names;
    closedir(d);
    cout << endl<< "exiting"<<endl;
    exit(0);
}