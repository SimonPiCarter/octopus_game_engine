#include "ThreadPool.hh"


ThreadPool::ThreadPool(uint32_t num_threads) {
    //const uint32_t num_threads = std::thread::hardware_concurrency(); // Max # of threads the system supports
    threads.resize(num_threads);
    for (uint32_t i = 0; i < num_threads; i++) {
        threads.at(i) = std::thread([this] () { ThreadLoop(); });
    }
}

void ThreadPool::ThreadLoop() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            mutex_condition.wait(lock, [this] {
                return !jobs.empty() || should_terminate;
            });
            if (should_terminate) {
                return;
            }
            job = jobs.front();
		    ++working;
            jobs.pop();
        }
        job();
		--working;
    }
}

void ThreadPool::queueJob(const std::function<void()>& job) {
    std::unique_lock<std::mutex> lock(queue_mutex);
    jobs.push(job);
    mutex_condition.notify_one();
}

bool ThreadPool::busy() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    return working > 0 || !jobs.empty();
}

void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        should_terminate = true;
        mutex_condition.notify_all();
    }
    for (std::thread& active_thread : threads) {
        active_thread.join();
    }
    threads.clear();
}

void enqueue_and_wait(ThreadPool &pool_p, std::vector<std::function<void()>> const &jobs_p)
{
	int finished_l = 0;
	std::mutex terminationMutex_l;
	std::condition_variable termination_l;
	std::unique_lock<std::mutex> lock_l(terminationMutex_l);
	int nbJobs_l = jobs_p.size();

	for(int n = 0 ; n < nbJobs_l ; ++ n)
	{
		pool_p.queueJob([&finished_l, &termination_l, &terminationMutex_l, &jobs_p, n]()
		{
			jobs_p[n]();
			std::unique_lock<std::mutex> lock_l(terminationMutex_l);
			finished_l++;
			termination_l.notify_all();
		}
		);
	}

	if(finished_l < nbJobs_l)
	{
		termination_l.wait(lock_l, [&]{ return finished_l >= nbJobs_l ; });
	}
}
