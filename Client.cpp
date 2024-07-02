// Client.cpp

// Libraries
#include <iostream>
#include <boost/asio.hpp> // Boost.Asio Library for networking
#include <string>

using namespace boost::asio;
using ip::tcp;

// For sending tasks to server
void send_task_to_server(int start, int end, int threads)
{
    // io_service object for handling async operations
    io_service io_service;
    // resolver for getting server address and port into an endpoint
    tcp::resolver resolver(io_service);
    tcp::resolver::query query("127.0.0.1", "12345"); // Querying for the Master server address and port

    // Resolve query to get the endpoint
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    // Endpoint checking (Irrelevant, can remove)
    // tcp::endpoint endpoint = *endpoint_iterator;
    // std::cout << "IP Address: " << endpoint.address().to_string() << std::endl;
    // std::cout << "Port: " << endpoint.port() << std::endl;

    // Create and connect the socket to the endpoint of the master server
    std::cout << "Attempting to connect to Server." << std::endl;
    tcp::socket socket(io_service);
    boost::system::error_code ec; // Error checking
    connect(socket, endpoint_iterator, ec);

    // Check if Client managed to connect
    if (ec) // Failed Connection
    {
        std::cerr << "Failed to connect to server: " << ec.message() << std::endl;
        std::cerr << "Was the server turned on?" << std::endl;
        return;
    }
    else // Successful Connection
    {
        std::cout << "Successfully connected to server." << std::endl;
        // Create a message string using the start and end points separated by a space then send to server using socket
        std::string message = std::to_string(start) + " " + std::to_string(end) + " " + std::to_string(threads) + "\n";
        std::cout << "User Input: " << message;
        write(socket, buffer(message));

        // Read response from the server
        boost::asio::streambuf response;                                        // Prepare to read response
        read_until(socket, response, "\n");                                     // Read all until newline character
        std::istream response_stream(&response);                                // input stream from the buffer to read response
        std::string response_message;                                           // response message holder
        std::getline(response_stream, response_message);                        // read a line from input stream into response message
        std::cout << "Response from server: " << response_message << std::endl; // once done, output the response message to console.
    }
}

int main()
{
    int start, end, threads; // declare start, end, and thread no. vars
    std::cout << "Enter start and end points (ex. 1 100): ";
    std::cin >> start >> end;
    std::cout << "Enter how many threads to use: ";
    std::cin >> threads;

    // Start - For the Time in milliseconds
    auto startTime = std::chrono::high_resolution_clock::now();
    send_task_to_server(start, end, threads);

    // End - For the Time in milliseconds
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsedTime = endTime - startTime;

    std::cout << "Time taken: " << elapsedTime.count() << " milliseconds." << std::endl;

    return 0;
}
