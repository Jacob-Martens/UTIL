#pragma once
/*
* FILE			: UTIL_ThreadsafeQueue.h
* CREATED		: 08/10/2023
* AUTHOR		: Jacob Martens (jacobmartens@gmail.com)
* DESCRIPTION	: 
* Template Threadsafe Queue. Useful for passing data from a 
* callback to a main processing thread 
*/
#include <queue>
#include <memory>
#include <exception>

#include "UTIL_Semaphore.h"

template <typename T>
class UTIL_ThreadsafeQueue{
private:
    std::queue<T> m_queue;      // Queue containing data
    std::unique_ptr<UTIL_Semaphore> m_access_sem;   // Semaphore for controlling access
    std::unique_ptr<UTIL_Semaphore> m_count_sem;    // Semaphore for blocking on empty queue
    int m_max_elements;

    const int MIN_MAX_ELEMENTS = 0;
    const int MAX_ELEMENTS_INFINITE = -1;
    const int BLOCKING_CALL = 1;

public:
    // Creates a Threadsafe Queue with no element limit
    UTIL_ThreadsafeQueue();
    // Creates a Threadsafe Queue with a size limit of max_elements
    UTIL_ThreadsafeQueue(int max_elements);

    // Adds data to the queue
    // Returns 0 on success, -1 on fail
    int Enqueue(T data);

    // Pops data from the queue
    //      if block == 1, this function will block until data is in the queue
    // Returns the popped data
    T Dequeue(int block);
};