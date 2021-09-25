cmake_minimum_required(VERSION 3.16)
project(syspro3)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

add_executable(master
        master.cpp
        Date.cpp Date.h
        MasterWorkerFunctions.h MasterWorkerFunctions.cpp
        namesList.cpp namesList.h
        )

add_executable(worker
        worker.cpp
        DateList.h DateList.cpp
        DiseaseList.h DiseaseList.cpp
        HashTable.cpp HashTable.h
        Tree.h Tree.cpp
        PatientList.cpp PatientList.h
        MasterWorkerFunctions.h MasterWorkerFunctions.cpp
        PatientRecord.cpp PatientList.h
        Date.cpp Date.h)

add_executable(client
        whoClient.cpp
        ClientFunctions.cpp
        ClientFunctions.h
        ArgumentStruct.h)

add_executable(server
        whoServer.cpp
        BoundedBuffer.cpp
        BoundedBuffer.h
        ServerFunctions.h
        ServerFunctions.cpp
        ArgumentStruct.h
        WorkersList.cpp WorkersList.h)