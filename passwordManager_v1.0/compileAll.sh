#!/bin/bash

g++ client.cpp -w -lSDL_bgi -lSDL2main -lSDL2 -o client
g++ -pthread -fpermissive -o server server.cpp

#./server <nr_threaduri>
#./client 0.0.0.0 2909

exit 0