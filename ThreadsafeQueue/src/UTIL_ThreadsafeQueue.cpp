#include "UTIL_ThreadsafeQueue.h"

template<typename T>
UTIL_ThreadsafeQueue<T>::UTIL_ThreadsafeQueue(){
    this->m_max_elements = MAX_ELEMENTS_INFINITE;
    this->m_access_sem = std::make_unique<UTIL_Semaphore>(1, 1);
    this->m_count_sem = std::make_unique<UTIL_Semaphore>(0);
}

template<typename T>
UTIL_ThreadsafeQueue<T>::UTIL_ThreadsafeQueue(int max_elements){
    // Validate max elements
    if(max_elements < MIN_MAX_ELEMENTS)
        max_elements =  MAX_ELEMENTS_INFINITE;
    
    this->m_max_elements = max_elements;
    this->m_access_sem = std::make_unique<UTIL_Semaphore>(1, 1);
    this->m_count_sem = std::make_unique<UTIL_Semaphore>(0, max_elements);
}

template<typename T>
int UTIL_ThreadsafeQueue<T>::Enqueue(T data){
    int retCode = 0;

    // If max element was specified, ensure new element wouldn't exceed max
    if(this->m_max_elements != MAX_ELEMENTS_INFINITE && 
    this->m_count_sem->GetValue() >= this->m_max_elements){
    
        retCode = -1;
    }
    else{
        try{
            this->m_access_sem->Wait();

            this->m_queue.push(data);

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

// Currently required queue types
template class UTIL_ThreadsafeQueue<std::string>;
