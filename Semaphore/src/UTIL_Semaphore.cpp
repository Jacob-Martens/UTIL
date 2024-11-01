#include "UTIL_Semaphore.h"

UTIL_Semaphore::UTIL_Semaphore(){
	this->m_count = MIN_COUNT;
	this->m_max_count = MAX_COUNT_INFINITE;
}

UTIL_Semaphore::UTIL_Semaphore(int count){
	// Validate provided count
	if (count < MIN_COUNT)
		count = MIN_COUNT;
	
	this->m_count = count;
	this->m_max_count = MAX_COUNT_INFINITE;
}

UTIL_Semaphore::UTIL_Semaphore(int count, int max_count){
	// Validate provided count
	if (count < MIN_COUNT)
		count = MIN_COUNT;

	// Validate provided max count
	if (max_count < MIN_COUNT)
		max_count = MAX_COUNT_INFINITE;
	if (max_count < count)
		max_count = count;


	this->m_count = count;
	this->m_max_count = max_count;
}

int UTIL_Semaphore::Post(){
	std::unique_lock<std::mutex> lock(this->m_mutex);
	if (this->m_max_count == MAX_COUNT_INFINITE || this->m_count < this->m_max_count) {
		this->m_count++;
		this->m_cv.notify_one();
	}
	return this->m_count;
}

int UTIL_Semaphore::Wait(){
	std::unique_lock<std::mutex> lock(this->m_mutex);
	while (this->m_count == 0)
		this->m_cv.wait(lock);
	this->m_count--;
	return this->m_count;
}

int UTIL_Semaphore::GetValue() const{
	return this->m_count;
}
