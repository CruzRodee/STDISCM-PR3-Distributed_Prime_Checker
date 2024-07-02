#!/bin/bash

# Compile the programs
#g++ -o slave_server SlaveServer.cpp -lboost_system -lpthread
g++ -o master_server MasterServer.cpp -lboost_system -lpthread
#g++ -o client Client.cpp -lboost_system -lpthread

# Run the slave servers in different terminals
#gnome-terminal -- bash -c "./slave_server; exec bash" &

# Give the slave servers a few seconds to start
#sleep 2

# Run the master server in a new terminal
gnome-terminal -- bash -c "./master_server; exec bash" &

# Give the master server a few seconds to start
#sleep 2

# Run the client in a new terminal
#gnome-terminal -- bash -c "./client; exec bash" &
