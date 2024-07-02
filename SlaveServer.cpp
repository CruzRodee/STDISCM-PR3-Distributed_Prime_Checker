// SlaveServer.cpp

#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <vector>
#include <mutex>
#include <cmath>

using namespace boost::asio;
using ip::tcp;

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
}

void handle_task(tcp::socket socket)
{
    try
    {
        boost::asio::streambuf buf;
        read_until(socket, buf, "\n");
        std::istream is(&buf);
        int start, end, threads;
        is >> start >> end >> threads;
        std::cout << "Received task: Start = " << start << " End = " << end << " Threads = " << threads << std::endl;

        int prime_count = 0;
        prime_checker(start, end, prime_count, threads);

        std::string result = std::to_string(prime_count) + "\n";
        write(socket, buffer(result));
        std::cout << "Task completed. Primes found: " << prime_count << std::endl;

        // Properly close the socket after the task is done
        socket.shutdown(tcp::socket::shutdown_both);
        socket.close();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception in handle_task: " << e.what() << std::endl;
    }
}

int main()
{
    io_service io_service;

    tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 12345)); // Listen on port 12345

    while (true)
    {
        std::cout << "Slave server is ready to accept tasks." << std::endl;
        tcp::socket socket(io_service);
        acceptor.accept(socket);
        std::cout << "Connected to master server." << std::endl;
        handle_task(std::move(socket));
    }

    return 0;
}
