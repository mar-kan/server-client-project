//Maria Anna Kanellaki - 1115201400060

#include <string>
#include <fcntl.h>
#include <signal.h>
#include "HashTable.h"

using namespace std;


//contains general functions used by master and worker executables

int check_error(int, string);
void write_string_to_fifo(int, int, string);
void read_and_print_fifo(int, int);
void read_and_print_fifo_pair(int, int);
int read_int_fifo(int, int);
void createLogFile(pid_t, HashTable*, int, int);
void handle_signal(int, siginfo_t*, void*);