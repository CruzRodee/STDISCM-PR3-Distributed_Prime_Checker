@echo off
setlocal

:: Compile the programs
g++ -o slave_server.exe SlaveServer.cpp -lboost_system -lpthread
g++ -o master_server.exe MasterServer.cpp -lboost_system -lpthread
g++ -o client.exe Client.cpp -lboost_system -lpthread

:: Run the slave servers in different terminals
start cmd /k "slave_server.exe"

:: Give the slave servers a few seconds to start
timeout /t 2 /nobreak

:: Run the master server in a new terminal
start cmd /k "master_server.exe"

:: Give the master server a few seconds to start
timeout /t 2 /nobreak

:: Run the client in a new terminal
start cmd /k "client.exe"

endlocal
