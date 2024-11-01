#pragma once
/*
* FILE			: UTIL_Semaphore.h
* CREATED		: 08/10/2023
* AUTHOR		: Jacob Martens (jacobmartens@gmail.com)
* DESCRIPTION	: 
* Semaphore implementation for ensuring thread-safety of resources
*/
#include <mutex>
#include <condition_variable>

class UTIL_Semaphore {
private:
	std::mutex m_mutex;
	std::condition_variable m_cv;
	int m_count;
	int m_max_count;

	const int MIN_COUNT = 0;
	const int MAX_COUNT_INFINITE = -1;

public:
	// Creates a UTIL_Semaphore with count of 0 and max_count of infinity
	UTIL_Semaphore();
	// Creates a UTIL_Semaphore with provided count and max_count of infinity
	explicit UTIL_Semaphore(int count);
	// Creates a UTIL_Semaphore with provided count and max_count
	UTIL_Semaphore(int count, int max_count);
	
	// Increments count and notifies waiting threads
	// Returns current value of m_count
	int Post();

	// Blocks thread until m_count is greater than 0, decrements count when access is granted
	// Returns current value of m_count
	int Wait();

	// Returns the current value of m_count
	int GetValue() const;
};

