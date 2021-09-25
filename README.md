# server-client-project
A server/client project written in C/Cpp for the university course System Programming about threads

----------------

This project is an extension of the diseaseAggregator project (https://github.com/mar-kan/diseaseAggregator). The same bash script was used to create the input directories and files.

It constists of 4 executables, Master and Worker, which is executed by master by forks και execl (exactly like in project diseaseAggregator), and whoServer and whoClient.

The communications between the sockets have a fixed size, defined as 500.

Every message exchanged, excluding the summary stats, are first concatenated in one string, to read and write once.

Worker-clients use the same socket for reading and writing, while server-workers use different ones.

The project can also be executed in different machines.

------------

compilation instructions: $ make
execution instructions:  

    whoServer first:   $ ./whoServer –q <queryPortNum> -s <statisticsPortNum> –w <numThreads> –b <bufferSize>
    then master:       $ ./master –w <numWorkers> -b <bufferSize> –s <serverIP> –p <serverPort> -i <input_dir>
    finally whoClient: $ ./whoClient –q <queryFile> -w <numThreads> –sp <servPort> –sip <servIP>
    
---------------------

# master

Master is basically diseaseAggregatorMain from the previous project.

# worker

Worker is basically workerMain from the previous project.

# whoServer

 Creates the sockets and the server's threads. Accepts and prints via its threads the summary statistics given by the worker. Then the server waits for queries from the client, which he sends to all the workers. The workers answer only if they find a result. The server receives the answer, prints it and then sends it to the client, who prints it and terminates.

A boundedBuffer is used for the communication between server-client and server-worker, where every fd that is accepted is inserted, και popped when read
by the threads.

# whoClient

 WhoClient connects to the server, opens the query file and starts reading and creating a thread for each line of the file. The main thread signals to the threads when they have all been created, to start writing, awaits them to terminate and terminates as well. The rest of the threads, execute the function send_query, where they write the queries to the sockets. Then they await for the answers, print them and terminate.
