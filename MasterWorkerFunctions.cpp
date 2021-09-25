//Maria Anna Kanellaki - 1115201400060

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <assert.h>
#include <wait.h>
#include "MasterWorkerFunctions.h"

extern int signal_id;


int check_error(int error, string message)
{
    if (error < 0)
    {
        perror(message.c_str());
        exit(errno);
    }
    return error;
}


void write_string_to_fifo(int fd, int bufferSize, string message)       //writes a string to a fifo
{                                                                       //writes first string's size then content
    //writes size
    int size=message.size();
    if (write(fd, &size, sizeof(short int)) != sizeof(short int))
    {
        cout << "size :"<<size<< "message: "<<message<<endl;
        perror("write size in fifo");
        exit(9);
    }

    //writes str
    if (size > bufferSize)              //writes str by bufferSize bytes per iteration
    {
        char str[size+1];
        strcpy(str, message.c_str());

        int bytes_written=0, write_size;
        while (bytes_written < size)
        {
            if (size - bytes_written < bufferSize)
                write_size = size-bytes_written;
            else
                write_size = bufferSize;

            if (write(fd, str+bytes_written, write_size) != write_size)
            {
                perror("write in fifo (bufferSize)");
                exit(8);
            }
            bytes_written += write_size;
        }
    }
    else
    {
        if (write(fd, message.c_str(), size) != size)
        {
            cout << message<< endl;
            perror("write in fifo");
            exit(10);
        }
    }
}


void read_and_print_fifo(int fd, int bufferSize)                    //reads from fifo and prints every string read
{
    //reads and prints each line read
    int size;
    while (read(fd, &size, sizeof(short int)) == sizeof(short int))     //reads sizeof str
    {
        if (size == 0)
        {
            cout << "size error in read"<<endl;
            break;
        }
        char str[size + 1];

        if (size < bufferSize)
        {
            if (read(fd, str, size) != size)                            //reads str
            {
                perror("read error (size)");
                exit(2);
            }
            str[size] = '\0';
        }
        else    //builds the str by bufferSize bytes per iteration
        {
            int bytes_read = 0, read_size;
            while (bytes_read < size)
            {
                if (size - bytes_read < bufferSize)
                    read_size = size - bytes_read;
                else
                    read_size = bufferSize;

                if (read(fd, str + bytes_read, read_size) != read_size)
                {
                    perror("read error (bufferSize)");
                    exit(3);
                }
                bytes_read += read_size;
            }
            str[bytes_read] = '\0';
        }
        if (strcmp(str, "no_result")==0 || strcmp(str, "ready!")==0)
            return;

        //prints line by line read
        cout <<str << endl;
    }
}


void read_and_print_fifo_pair(int fd, int bufferSize)                   //exactly as above but prints 2 values in the same line
{
    //reads and prints each line read
    int size=0, count=1;
    while (read(fd, &size, sizeof(short int)) == sizeof(short int))     //reads sizeof str
    {
        if (size == 0)
        {
            cout << "size error in read"<<endl;
            break;
        }
        char str[size + 1];
        if (size < bufferSize)
        {
            if (read(fd, str, size) != size)                            //reads str
            {
                perror("read error (size)");
                exit(2);
            }
            str[size] = '\0';
        }
        else    //builds the str by bufferSize bytes per iteration
        {
            int bytes_read = 0, read_size;
            while (bytes_read < size)
            {
                if (size - bytes_read < bufferSize)
                    read_size = size - bytes_read;
                else
                    read_size = bufferSize;

                if (read(fd, str + bytes_read, read_size) != read_size)
                {
                    perror("read error (bufferSize)");
                    exit(3);
                }
                bytes_read += read_size;
            }
            str[bytes_read] = '\0';
        }
        if (strcmp(str, "no_result")==0 || strcmp(str, "ready!")==0)
            return;

        //prints line by line read
        cout << str << " ";

        if (count % 2 == 0)         //changes line every 2 elements
            cout << endl;

        count++;
    }
}


int read_int_fifo(int fd, int bufferSize)                               //same as above but for integers
{                                                                       //returns them instead of printing them
    //reads and sums lines. prints their count
    int size, count=0;
    while (read(fd, &size, sizeof(short int)) == sizeof(short int))     //reads sizeof str
    {
        if (size == 0)
        {
            cout << "size error in read"<<endl;
            break;
        }

        char str[size + 1];
        if (size < bufferSize)
        {
            if (read(fd, str, size) != size)                            //reads str
            {
                perror("read error (size)");
                exit(2);
            }
            str[size] = '\0';
        }
        else    //builds the str by bufferSize bytes per iteration
        {
            int bytes_read = 0, read_size;
            while (bytes_read < size)
            {
                if (size - bytes_read < bufferSize)
                    read_size = size - bytes_read;
                else
                    read_size = bufferSize;

                if (read(fd, str + bytes_read, read_size) != read_size)
                {
                    perror("read error (bufferSize)");
                    exit(3);
                }
                bytes_read += read_size;
            }
            str[bytes_read] = '\0';
        }
        if (strcmp(str, "no_result")==0 || strcmp(str, "ready!")==0)
            break;

        //prints line by line read
        int num;
        assert(num = atoi(str));
        count += num;
    }
    return count;
}


void createLogFile(pid_t pid, HashTable* countryTable, int total, int successful)
{
    string filename = "log_file."+to_string(pid);
    FILE *fp = fopen(filename.c_str(), "w");
/*
    for (int i=0; i<countryTable->getSize(); i++)
    {
        if (!countryTable->getTable()[i])
            continue;

        BucketEntry * entry = countryTable->getTable()[i]->getFirst();
        while (entry)
        {
            fprintf(fp, "%s\n", entry->getKey().c_str());
            entry = entry->getNextEntry();
        }
        fprintf(fp, "TOTAL %d\n", total);
        fprintf(fp, "SUCCESS %d\n", successful);
        fprintf(fp, "FAIL %d\n", total-successful);
    }*/
}


//signal handler
void handle_signal(int signum, siginfo_t * info, void *context)
{
    cout << signum << endl;
    switch (signum) {
        case SIGUSR1:
            break;
        case SIGCHLD:
            int wstatus;
            //signal_id = info->si_pid;
            waitpid(info->si_pid, &wstatus, WNOHANG);
            break;
        case SIGINT:
            break;
        case SIGQUIT:
            break;
        default:
            break;
    }
}