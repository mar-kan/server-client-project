all: master worker server client

master: master.o namesList.o MasterWorkerFunctions.o Date.o
	g++ -g3 master.o namesList.o MasterWorkerFunctions.o Date.o -o master

worker: worker.o Date.o PatientList.o PatientRecord.o HashTable.o Tree.o DateList.o DiseaseList.o MasterWorkerFunctions.o
	g++ -g3 worker.o Date.o PatientList.o PatientRecord.o HashTable.o Tree.o DateList.o DiseaseList.o MasterWorkerFunctions.o -o worker

client: whoClient.o ClientFunctions.o
	g++ whoClient.o ClientFunctions.o -o client -g3 -pthread

server: whoServer.o ServerFunctions.o BoundedBuffer.o WorkersList.o
	g++ whoServer.o ServerFunctions.o BoundedBuffer.o WorkersList.o -o server -g3 -pthread

client.o: whoClient.cpp
	g++ -std=c++11 -g3 -c whoClient.cpp

server.o: whoServer.cpp
	g++ -std=c++11 -g3 -c whoServer.cpp

master.o: master.cpp
	g++ -std=c++11 -g3 -c master.cpp

worker.o: worker.cpp
	g++ -std=c++11 -g3 -c worker.cpp

Date.o: Date.cpp Date.h
	g++ -std=c++11 -g3 -c Date.cpp

PatientList.o: PatientList.cpp PatientList.h
	g++ -g3 -std=c++11 -c PatientList.cpp

PatientRecord.o: PatientRecord.cpp PatientRecord.h
	g++ -g3 -std=c++11 -c PatientRecord.cpp

HashTable.o: HashTable.cpp HashTable.h
	g++ -std=c++11 -g3 -c HashTable.cpp

Tree.o: Tree.cpp Tree.h
	g++ -g3 -std=c++11 -c Tree.cpp

namesList.o: namesList.cpp namesList.h
	g++ -g3 -std=c++11 -c namesList.cpp

CountryList.o: DateList.cpp DateList.h
	g++ -g3 -std=c++11 -c DateList.cpp

DiseaseList.o: DiseaseList.cpp DiseaseList.h
	g++ -g3 -std=c++11 -c DiseaseList.cpp

MasterWorkerFunctions.o: MasterWorkerFunctions.cpp MasterWorkerFunctions.h
	g++ -g3 -std=c++11 -c MasterWorkerFunctions.cpp

BoundedBuffer.o: BoundedBuffer.cpp BoundedBuffer.h
	g++ BoundedBuffer.cpp -g3 -std=c++11 -c -pthread

ClientFunctions.o: ClientFunctions.cpp ClientFunctions.h
	g++ -g3 -std=c++11 -c ClientFunctions.cpp

ServerFunctions.o: ServerFunctions.cpp ServerFunctions.h
	g++ -g3 -std=c++11 -c ServerFunctions.cpp

WorkersList.o: WorkersList.cpp WorkersList.h
	g++ -g3 -std=c++11 -c WorkersList.cpp

clean:
	rm *.o master worker client server