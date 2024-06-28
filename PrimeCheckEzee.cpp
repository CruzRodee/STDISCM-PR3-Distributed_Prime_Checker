// Tester Code
// use as reference
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cmath>
#include <chrono>

bool check_prime(const int &n)
{
    if (n <= 1)
        return false;
    for (int i = 2; i < n; i++)
    {
        if (n % i == 0)
        {
            return false;
        }
    }
    return true;
}

void prime_checker(int threadId, int start, int end, std::vector<int> &primes, std::mutex &primes_mutex)
{
    for (int current_num = start; current_num <= end; current_num++)
    {
        if (check_prime(current_num))
        {
            std::lock_guard<std::mutex> guard(primes_mutex);
            primes.push_back(current_num);
            std::cout << "Thread " << threadId << " found prime: " << current_num << std::endl;
        }
    }
}

int main()
{
    int LIMIT, THREAD_COUNT;

    std::cout << "Enter the upper bound of integers to check for primes:" << std::endl;
    std::cin >> LIMIT;

    std::cout << "Enter the number of threads to use:" << std::endl;
    std::cin >> THREAD_COUNT;

    std::vector<int> primes;
    std::mutex primes_mutex;

    // Start - For the Time in milliseconds
    auto startTime = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    int range = LIMIT / THREAD_COUNT;

    // Iterates through the numbers and for each, new thread will  start and prime checker will be called
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        int start = i * range + 2;
        int end = (i == THREAD_COUNT - 1) ? LIMIT : (i + 1) * range + 1;
        threads.emplace_back(prime_checker, i, start, end, std::ref(primes), std::ref(primes_mutex));
    }

    // This will wait for the threads to finish
    for (auto &thread : threads)
    {
        thread.join();
    }

    // End - For the Time in milliseconds
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsedTime = endTime - startTime;

    std::cout << primes.size() << " primes were found." << std::endl;
    std::cout << "Time taken: " << elapsedTime.count() << " milliseconds." << std::endl;

    return 0;
}