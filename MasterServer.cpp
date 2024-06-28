// Server.cpp

#include <iostream>
#include <boost/asio.hpp> // Boost.Asio Library for networking
#include <thread>
#include <vector>
#include <mutex>
#include <cmath>

using namespace boost::asio;
using ip::tcp;

// Function to check if a number is a prime
bool check_prime(const int &n)
{
    if (n <= 1)
        return false;
    for (int i = 2; i <= std::sqrt(n); i++) // Improved prime check
    {
        if (n % i == 0)
        {
            return false;
        }
    }
    return true;
}

// Function to count primes in a range using multiple threads
void prime_checker(int start, int end, int &prime_count, int num_threads)
{
    std::mutex count_mutex;
    prime_count = 0;

    auto thread_func = [&](int s, int e)
    {
        int local_count = 0;
        for (int current_num = s; current_num <= e; ++current_num)
        {
            if (check_prime(current_num))
            {
                local_count++;
            }
        }
        std::lock_guard<std::mutex> guard(count_mutex);
        prime_count += local_count;
    };

    int range = (end - start + 1) / num_threads;
    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; ++i)
    {
        int s = start + i * range;
        int e = (i == num_threads - 1) ? end : s + range - 1;
        threads.emplace_back(thread_func, s, e);
    }

    for (auto &t : threads)
    {
        t.join();
    }
    std::cout << "Task completed. Primes found: " << prime_count << std::endl;
}

// Function for handling the client connections
void handle_client(tcp::socket socket, std::vector<tcp::socket> &slaves, bool master_only)
{
    std::cout << "Handling Client" << std::endl;
    boost::asio::streambuf buf;
    read_until(socket, buf, "\n");
    std::istream is(&buf);
    int start, end, threads;
    is >> start >> end >> threads;
    std::cout << "Client input: Start = " << start << " End = " << end << " Thread No. = " << threads << std::endl;

    int total_primes = 0;

    // Master-only mode: Master handles the entire range
    if (master_only)
    {
        prime_checker(start, end, total_primes, threads);
    }
    else // Using slave servers
    {
        int num_slaves = slaves.size();                   // Number of slave servers
        int range = (end - start + 1) / (num_slaves + 1); // +1 for the master

        // Master server also handles a part of the range
        int master_start = start;
        int master_end = master_start + range - 1;
        int master_primes = 0;
        prime_checker(master_start, master_end, master_primes, threads);
        total_primes += master_primes;

        // Distribute remaining tasks to the slave servers
        for (int i = 0; i < num_slaves; ++i)
        {
            int s = start + (i + 1) * range;
            int e = (i == num_slaves - 1) ? end : s + range - 1;

            std::string message = std::to_string(s) + " " + std::to_string(e) + " " + std::to_string(threads) + "\n";
            write(slaves[i], buffer(message));
        }

        // Collect results from the slave servers
        for (int i = 0; i < num_slaves; ++i)
        {
            boost::asio::streambuf result_buf;
            read_until(slaves[i], result_buf, "\n");
            std::istream result_stream(&result_buf);
            int primes;
            result_stream >> primes;
            total_primes += primes;
        }
    }

    std::string response = "Total primes: " + std::to_string(total_primes) + "\n";
    std::cout << "Total Primes found for Client: " << total_primes << std::endl;
    write(socket, buffer(response));
}

int main()
{
    bool master_only = false; // Set this to true to run in master-only mode

    io_service io_service; // create io_service object for asynch operations

    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 12345)); // acceptor for listening to connections on port 12345
    std::vector<tcp::socket> slaves;                                     // vector that holds slave sockets, useful if adding more slaves

    if (!master_only)
    {
        // copy paste this for adding more slaves
        // Code for connecting to a slave, at the moment only one slave
        // Slave 1: Localhost Slave
        tcp::socket slave_socket(io_service);                                // socket for slave
        tcp::resolver resolver(io_service);                                  // resolver for translating slave address+port to endpoint
        tcp::resolver::query query("127.0.0.1", "12346");                    // create a query with the slave address+port, change depending on the machine's actual server+port
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query); // resolve query to get endpoint iterator
        boost::system::error_code ec;                                        // holder for error code (for checking)
        connect(slave_socket, endpoint_iterator, ec);                        // attempt to connect
        if (ec)                                                              // error check
        {
            std::cerr << "Failed to connect to slave: " << ec.message() << std::endl;
            return 1;
        }
        slaves.push_back(std::move(slave_socket)); // add the slave socket to vector
    }

    while (true)
    {
        std::cout << "Server initiated. Ready to Accept Client." << std::endl;
        tcp::socket socket(io_service);
        acceptor.accept(socket);
        std::cout << "Client Connected." << std::endl;
        std::thread(handle_client, std::move(socket), std::ref(slaves), master_only).detach();
    }

    return 0;
}
