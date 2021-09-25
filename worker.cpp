//Kanellaki Maria Anna - 1115201400060


#include <iostream>
#include <cerrno>
#include <string>
#include <cstring>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "PatientList.h"
#include "HashTable.h"
#include "MasterWorkerFunctions.h"
#include "DateList.h"
#include "DiseaseList.h"

using namespace std;

#define entries 1000
#define bucketSize 20
#define socket_buff_size 500

int listen_port = 5000;


int main(int argc, char *argv[]) {
    //takes arguments ./worker -b bufferSize -i inputFiles -f fifo
    if (argc != 7 || strcmp(argv[1], "-b") != 0 || strcmp(argv[3], "-i") != 0 || strcmp(argv[5], "-f") != 0)
    {
        cout << "Invalid arguments for Worker" << endl;
        exit(-1);
    }
    int bufferSize;

    assert(bufferSize = atoi(argv[2]));
    string input_dir = argv[4];
    const char * fifo = argv[6];

    //signal handler struct
    struct sigaction sig_act;
    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_flags = SA_SIGINFO | SA_RESTART;
    sig_act.sa_sigaction = handle_signal;

    //initializes worker's data structures
    PatientList * patients = new PatientList();                                              //creates empty list of patients
    HashTable * diseaseTable = new HashTable(entries, bucketSize);                           //create empty disease hash table
    HashTable * countryTable = new HashTable(entries, bucketSize);                           //create empty country hash table
    DateList * myDates = new DateList();                                                     //stores all dates of worker with their diseases and age stats

    int age_stats[4];                                                                        //stores num of cases for each age group for summary stats
    for (int i=0; i<4; i++)
        age_stats[i] = 0;

    //opens fifo for read
    int fd = open(fifo, O_RDONLY);
    if (fd < 0)
    {
        cout << "open fifo"<< endl;
        exit(1);
    }

    string serverIP;
    int portNum;

    //reads all fifo until '0' is read as a size
    short int size;
    while (read(fd, &size, sizeof(short int)) == sizeof(short int))
    {
        if (size == 0)
        {
            cout << "ERROR"<< endl;
            break;
        }

        char dir_name[size + 1];
        if (size < bufferSize) {
            if (read(fd, dir_name, size) != size) {
                perror("read error (size)");
                exit(2);
            }
            dir_name[size] = '\0';
        }
        else    //builds the dir_name by bufferSize bytes per iteration
        {
            int bytes_read = 0, read_size;
            while (bytes_read < size)
            {
                if (size - bytes_read < bufferSize)
                    read_size = size - bytes_read;
                else
                    read_size = bufferSize;

                if (read(fd, dir_name + bytes_read, read_size) != read_size)
                {
                    perror("read error (bufferSize)");
                    exit(3);
                }
                bytes_read += read_size;
            }
            dir_name[bytes_read] = '\0';
        }

        /**store IP**/
        string temp = dir_name;
        if (temp.find("IP:") != string::npos)
        {
            temp.erase(0,3);            //erases identifier "IP:" from string
            serverIP = temp;
            continue;
        }

        /**store port num**/
        temp = dir_name;
        if (temp.find("port:") != string::npos)
        {
            temp.erase(0, 5);               //erases identifier "port:" from string
            assert(portNum = atoi(temp.c_str()));
            continue;
        }
        if (strcmp(dir_name, "ready!")==0)
            break;

        if (chdir(input_dir.c_str()) < 0)
        {
            perror("chdir");
            exit(EXIT_FAILURE);
        }

        DIR *d;
        d = opendir(dir_name);
        if (!d)
        {
            perror("open subdir");
            exit(4);
        }
        chdir(dir_name);

        struct dirent *file;
        while ((file = readdir(d)) != NULL)                                                    //reads every file's name
        {
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
                continue;

            string date = file->d_name;
            FILE *fp = fopen(date.c_str(), "r");                                        //opens each file for read
            if (!fp)
            {
                cout << "Error in opening file " << date << " of " << dir_name << endl;
                perror("fopen file");
                exit(5);
            }

            size_t lsize = 0;
            char *line = NULL;
            PatientRecord *rec = NULL;
            while (getline(&line, &lsize, fp) != -1)                                           //reads file line by line
            {
                //creates new patient record
                char *dat = new char[date.length()];
                strcpy(dat, date.c_str());

                rec = new PatientRecord(line, dir_name, dat);                                 //creates PatientRecord from line, dir and file names
                if (!rec->isValid())                                                          //if record is not valid deletes it and continues to next line
                {
                    //cout << "ERROR" << endl;
                    delete rec;
                    continue;
                }
                if (rec->getStatus() == "EXIT")     //checks if patient had entered
                {
                    PatientRecord*dtemp;
                    if ((dtemp = patients->SearchRecord(rec->getRecordId())))
                    {   //if id exists, updates its exit date
                        dtemp->setExitDate(rec->getExitDate());
                    }
                    else
                    {
                        //cout << "ERROR"<< endl;
                        delete dtemp;
                    }
                    delete rec;                     //deletes new record and continues
                    continue;
                }

                //inserts it in all data structs
                patients->push(new PatientNode(rec));                                         //inserts node in list

                diseaseTable->InsertToTable(rec,rec->getDiseaseId());                         //inserts record in disease table
                countryTable->InsertToTable(rec,rec->getCountry());                           //inserts record in country table

                if (!myDates->SearchDate(rec->getEntryDate()->getAsString()))                 //inserts record in date list
                    myDates->push(new DateNode(rec));

                DiseaseList * thisDateDis = myDates->SearchDate(rec->getEntryDate()->getAsString())->getMyDiseases();
                DiseaseNode * dnode;                                   //inserts record in disease list of this country
                if ((dnode = thisDateDis->SearchDisease(rec->getDiseaseId())))                //if it exists calculates age stats
                {
                    dnode->AddAgeToStats(rec->getAge());                                      //calculate summary stats
                    continue;
                }
                else
                {
                    delete dnode;
                    thisDateDis->push(new DiseaseNode(rec));
                }
            }
            fclose(fp);
            free(line);
        }
        closedir(d);
        chdir("../..");
    }


    /**connect to server**/
    int sock = check_error(socket(AF_INET, SOCK_STREAM, 0), "create socket");
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(serverIP.c_str());
    addr.sin_port = htons(portNum);

    check_error(connect(sock, (struct sockaddr *)&addr, sizeof(addr)), "connect to socket");
    char msg[socket_buff_size];

    /**send port num for this worker**/
    listen_port = listen_port + ((int)getpid());                                                       //unique port num for every process

    strcpy(msg, ("port:"+to_string(listen_port)).c_str());
    check_error(write(sock, msg, socket_buff_size), "write port num");                                  //writes port num

    /**send statistics to server**/
    DateNode * dtemp = myDates->getHead();                                                                                               //for each date
    while (dtemp)
    {
        strcpy(msg, dtemp->getDate()->getAsString().c_str());
        check_error(write(sock, msg, socket_buff_size), "write date");                                   //writes date name

        strcpy(msg, dtemp->getRecord()->getCountry().c_str());
        check_error(write(sock, msg, socket_buff_size), "write country");                                 //writes country name

        DiseaseNode * temp = dtemp->getMyDiseases()->getHead();
        while (temp)                                                      //for each disease
        {
            strcpy(msg, temp->getDisease().c_str());
            check_error(write(sock, msg, socket_buff_size), "write disease");//writes disease name

            //cases for each age range
            int age_group=0;
            for (int i=0; i<4; i++)
            {
                int cases = temp->getAgeStats(i);
                string str;                                               //creates string for each age range
                if (i==0)
                    str = "Age range "+to_string(age_group)+"-"+to_string(age_group+20)+" years: "+ to_string(cases)+" cases";
                else if (i==3)
                    str = "Age range "+to_string(age_group)+"+ years: "+ to_string(cases)+" cases";
                else
                    str = "Age range "+to_string(age_group+1)+"-"+to_string(age_group+20)+" years: "+ to_string(cases)+" cases";

                strcpy(msg, str.c_str());
                check_error(write(sock, msg, socket_buff_size), "write string");   //writes string
                age_group += 20;
            }
            temp = temp->getNext();
        }
        dtemp = dtemp->getNext();
    }
    strcpy(msg, "ready!");
    check_error(write(sock, msg, socket_buff_size), "write ready");      //writes ready when done


    /**create separate socket for reading**/
    struct sockaddr_in worker_addr;
    socklen_t workerlen;
    struct sockaddr *workerptr = (struct sockaddr *)&worker_addr;

    int new_sock = check_error(socket(AF_INET, SOCK_STREAM, 0), "create new socket");

    worker_addr.sin_family = AF_INET;
    worker_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    worker_addr.sin_port = htons(listen_port);

    check_error(bind(new_sock, workerptr, sizeof(worker_addr)), "bind new socket");
    check_error(listen(new_sock, 10), "listen on new socket");

    /**from this point on, worker is always ready to receive queries**/
    int total=0, success=0;

    while (true)
    {
        if (sigaction(SIGINT, &sig_act, &sig_act) < 0)
        {
            perror("sigaction error");
            exit(-2);
        }

        //accepts and reads requests
        int new_fd = check_error(accept(new_sock, workerptr, &workerlen), "accept worker fd");

        check_error(read(new_fd, msg, socket_buff_size),"read error");
        msg[strlen(msg)] = '\0';

        if (strcmp(msg, "ready!")==0)
            continue;

        cout << msg << endl;

        char *date1, *date2, *country, *disease;
        char delimit[]=" \t\n";
        string input = msg;

        //executes command and sends results back from the fifo
        if (input.find("/diseaseFrequency") != string::npos)                       //prints count of diseased people for given disease and given dates
        {                                                                             //and maybe country
            total++;
            strtok(msg, delimit);                                                     //splits command name
            if (!(disease = strtok(NULL, delimit))) {
                cout << "Error. Needs disease name to execute" << endl;
                continue;
            }
            if (!(date1 = strtok(NULL, delimit))) {
                cout << "Error. Needs dates to execute" << endl;
                continue;
            }
            if (!(date2 = strtok(NULL, delimit))) {
                cout << "Error. Needs two dates to execute" << endl;
                continue;
            }
            country = strtok(NULL, delimit);

            Date *d1 = new Date(date1);
            Date *d2 = new Date(date2);
            if (!d1->isValid() || !d2->isValid())
            {
                delete d1;
                delete d2;
                continue;
            }
            int count;
            if (country)                                                              //finds worker with country and writes its pipe
                count = countryTable->CountAllCountry(country, disease, d1, d2);
            else                                                                      //no country writes to all workers
                count = diseaseTable->CountAll(disease, d1, d2);

            strcpy(msg, to_string(count).c_str());
            check_error(write(sock, msg, socket_buff_size), "write answer to diseaseFreq");
            cout << msg << endl<<endl;
            success++;
        }
        else if (input.find("/topk-AgeRanges") != string::npos)
        {
            total++;
            strtok(msg, delimit);                                                    //splits command name
            int k;

            if (!(country = strtok(NULL, delimit)))
            {
                cout << "Error. Needs number to execute" << endl;
                continue;
            }
            else
                assert(k = atoi(country));

            if (!(country = strtok(NULL, delimit)))
            {
                cout << "Error. Needs country to execute" << endl;
                continue;
            }
            if (!(disease = strtok(NULL, delimit)))
            {
                cout << "Error. Needs disease to execute" << endl;
                continue;
            }
            if (!(date1 = strtok(NULL, delimit)))
            {
                cout << "Error. Needs dates to execute" << endl;
                continue;
            }
            if (!(date2 = strtok(NULL, delimit)))                              //if there is only one date then the argument is not valid
            {
                cout << "Error. Needs 2 dates to execute"<< endl;
                continue;
            }
            Date * d1 = new Date(date1);
            Date * d2 = new Date(date2);

            if (!d1->isValid() || !d2->isValid())
            {
                delete d1;
                delete d2;
                continue;
            }

            int age=0;
            int age_counts[2][4];                  //array that stores the counts for each age group
            for (int i=0; i<4; i++)
            {
                age_counts[0][i]=age;
                age_counts[1][i]=0;
                age += 20;
            }

            //calculates ages from patient list
            PatientNode *temp = patients->getHead();
            int total_count=0;
            while (temp)
            {
                if (temp->getRecord()->getCountry() == country && temp->getRecord()->getDiseaseId() == disease)
                {
                    if (d1->earlierThan(temp->getRecord()->getEntryDate()) && temp->getRecord()->getEntryDate()->earlierThan(d2) ||
                    temp->getRecord()->getExitDate() && d1->earlierThan(temp->getRecord()->getExitDate()) && temp->getRecord()->getExitDate()->earlierThan(d2))
                    {
                        if (temp->getRecord()->getAge() <= 20)
                            age_counts[1][0]++;
                        else if (temp->getRecord()->getAge() <= 40)
                            age_counts[1][1]++;
                        else if (temp->getRecord()->getAge() <= 60)
                            age_counts[1][2]++;
                        else
                            age_counts[1][3]++;

                        total_count++;
                    }
                }
                temp = temp->getNext();
            }

            //sort array (bubble)
            for (int i=0; i<4; i++)
            {
                for (int j=0; j<4-i; j++)
                {
                    if (age_counts[1][j+1] > age_counts[1][j])
                    {
                        int tmp = age_counts[0][j+1];
                        age_counts[0][j+1] = age_counts[0][j];
                        age_counts[0][j] = tmp;

                        tmp = age_counts[1][j+1];
                        age_counts[1][j+1] = age_counts[1][j];
                        age_counts[1][j] = tmp;
                    }
                }
            }
            //write top k in one string
            string all = "";
            for (int i=0; i<k; i++)
            {
                if (i == 4)
                    break;

                if (age_counts[0][i] == 0)
                    all.append("0-20: "+to_string(age_counts[1][i]*100/total_count)+"% \n");
                else if (age_counts[0][i] == 20)
                    all.append("21-40: "+to_string(age_counts[1][i]*100/total_count)+"% \n");
                else if (age_counts[0][i] == 40)
                    all.append("41-60: "+to_string(age_counts[1][i]*100/total_count)+"% \n");
                else
                    all.append("60+: "+to_string(age_counts[1][i]*100/total_count)+"%\n");
            }
            strcpy(msg, all.c_str());
            check_error(write(sock, msg, socket_buff_size), "write answer to top-k");
            cout << msg << endl<<endl;
            success++;
        }
        else if (input.find("/searchPatientRecord") != string::npos)
        {
            total++;
            strtok(msg, delimit);                                                    //splits command name

            if (!(country = strtok(NULL, delimit)))
            {
                cout << "Error. Needs recordID to execute" << endl;
                continue;
            }
            PatientRecord *rec = patients->SearchRecord(country);
            if (!rec)
            {
                //write_string_to_fifo(fd_send, bufferSize, "no_result");
                //write_string_to_fifo(fd_send, bufferSize, "ready!");
                continue;
            }
            strcpy(msg, rec->getWhole().c_str());
            check_error(write(sock, msg, socket_buff_size), "write answer to search");
            cout << msg << endl<<endl;
            success++;
        }
        else if (input.find("/numPatientAdmissions") != string::npos || input.find("/numPatientDischarges") != string::npos)
        {
            total++;
            strtok(msg, delimit);                                                     //splits command name
            if (!(disease = strtok(NULL, delimit))) {
                cout << "Error. Needs disease name to execute" << endl;
                continue;
            }
            if (!(date1 = strtok(NULL, delimit))) {
                cout << "Error. Needs dates to execute" << endl;
                continue;
            }
            if (!(date2 = strtok(NULL, delimit))) {
                cout << "Error. Needs two dates to execute" << endl;
                continue;
            }
            country = strtok(NULL, delimit);

            Date *d1 = new Date(date1);
            Date *d2 = new Date(date2);
            if (!d1->isValid() || !d2->isValid())
            {
                delete d1;
                delete d2;
                continue;
            }

            string status;
            if (input.find("/numPatientAdmissions") != string::npos)
                status = "ENTER";
            else
                status = "EXIT";

            int count;
            if (country)                //finds country and calculates its count
            {
                count = countryTable->CountEnterOrExitCountry(country, disease, d1, d2, status);

                //writes country name and count
                string countr = country;
                strcpy(msg, (countr+" "+to_string(count)).c_str());
                check_error(write(sock, msg, socket_buff_size), "write answer to admissions/discharges");
                cout << msg << endl<<endl;
            }
            else                                    //writes and counts same as above but for every country of worker
            {                                       //through the country hash table
                Bucket ** table = countryTable->getTable();
                msg[0] = '\0';
                for (int i=0; i<countryTable->getSize(); i++)
                {
                    count=0;
                    if (table[i])
                    {
                        table[i]->getTree()->countEnterOrExitCountry(table[i]->getTree()->getRoot(), d1, d2, disease, &count, status);
                        strcat(msg, (table[i]->getFirst()->getKey()+" "+to_string(count)+"\n").c_str());
                    }
                }
                check_error(write(sock, msg, socket_buff_size), "write answer to admissions/discharges");
                cout << msg << endl<<endl;
            }
            success++;
        }
        /*else
        {
            total++;
            cout << "ERROR" << endl;
        }*/
    }

    //deletes every structure
    delete diseaseTable;
    delete countryTable;
    delete patients;
    delete myDates;

    close(sock);
    close(fd);
    exit(0);
}