/*
* FILE			: UTIL_ThreadsafeQueue.cpp
* CREATED		: 08/10/2023
* AUTHOR		: Jacob Martens (jacobmartens@gmail.com)
* DESCRIPTION	:
* Source file for the templatizable threadsafe queue
*/
#include "UTIL_ThreadsafeQueue.h"

/*
* Constructor
* Creates a Threadsafe Queue of type T with no element limit
*/
template<typename T>
UTIL_ThreadsafeQueue<T>::UTIL_ThreadsafeQueue(){
    this->m_max_elements = MAX_ELEMENTS_INFINITE;
    this->m_access_sem = std::make_unique<UTIL_Semaphore>(1, 1);
    this->m_count_sem = std::make_unique<UTIL_Semaphore>(0);
}

/*
* Parameterized Constructor
* Creates a Threadsafe Queue of type T with a size limit of max_elements
*/
template<typename T>
UTIL_ThreadsafeQueue<T>::UTIL_ThreadsafeQueue(int max_elements){
    // Validate max elements
    if(max_elements < MIN_MAX_ELEMENTS)
        max_elements =  MAX_ELEMENTS_INFINITE;
    
    this->m_max_elements = max_elements;
    this->m_access_sem = std::make_unique<UTIL_Semaphore>(1, 1);
    this->m_count_sem = std::make_unique<UTIL_Semaphore>(0, max_elements);
}

/*
* Enqueue
* Adds data of type T to the queue
*/
template<typename T>
int UTIL_ThreadsafeQueue<T>::Enqueue(const T& data){
    int retCode = 0;

    // If max element was specified, ensure new element wouldn't exceed max
    if(this->m_max_elements != MAX_ELEMENTS_INFINITE && 
    this->m_count_sem->GetValue() >= this->m_max_elements){
    
        retCode = -1;
    }
    else{
        try{
            // Wait for access
            this->m_access_sem->Wait();

            // Enqueue Data
            this->m_queue.push(data);

            // Relinquish access, notify anyone waiting
            this->m_access_sem->Post();
            this->m_count_sem->Post();
        }
        catch(const std::system_error& ex){
            printf("ThreadsafeQueue : Semaphore threw %s\n", ex.what());
            retCode = -1;
        }
    }

    return retCode;
}

/*
* Dequeue
* Pop data off the queue, returns data
* If block == 1, this call will block
*/
template<typename T>
T UTIL_ThreadsafeQueue<T>::Dequeue(int block){
    T new_data;

    if(this->m_count_sem->GetValue() > 0 || block == BLOCKING_CALL){
        this->m_count_sem->Wait();
        this->m_access_sem->Wait();

        new_data = this->m_queue.front();
        this->m_queue.pop();

        this->m_access_sem->Post();
    }

    return new_data;
}

// Forward Declarations
// Currently required queue types
template class UTIL_ThreadsafeQueue<std::string>;
