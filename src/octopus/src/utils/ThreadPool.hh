#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <list>

class ThreadPool {
public:
	ThreadPool(uint32_t num_threads);
	~ThreadPool();
    void queueJob(const std::function<void()>& job);
    void stop();
    bool busy();

    uint32_t size() const { return threads.size(); }

private:
    void ThreadLoop();

    bool should_terminate = false;           // Tells threads to stop looking for jobs
    std::mutex queue_mutex;                  // Prevents data races to the job queue
    std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination
    std::list<std::thread> threads;
    std::queue<std::function<void()>> jobs;

	std::atomic_int working = 0;
};

#endif
