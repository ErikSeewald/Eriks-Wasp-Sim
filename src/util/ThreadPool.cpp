#include "ThreadPool.h"
#include <atomic>

/**
* Initialize a pool of the given size that will wait for jobs until the pool is destroyed.
*/
ThreadPool::ThreadPool(size_t numThreads)
    : shutDown(false)
    , activeThreads(0)
{
    for (size_t i = 0; i < numThreads; ++i)
    {
        workers.emplace_back([this] {_workerLoop(); });
    }
}

ThreadPool::~ThreadPool() 
{
    // LOCKED SIGNAL THREADS SHUTDOWN
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        shutDown = true;
    }

    // Wake all theads so they can exit and then join them
    jobAvailableCond.notify_all();
    for (auto& t : workers) 
    {
        if (t.joinable()) {t.join();}
    }
}

/**
* Submits the given job to the pool queue.
* The next free worker thread will handle the job.
*/
void ThreadPool::enqueue(std::function<void()> job)
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        jobs.push(std::move(job));
    }
    jobAvailableCond.notify_one();
}


/**
* Waits until all jobs in the queue are finished.
*/
void ThreadPool::waitFinishAll()
{
    std::unique_lock<std::mutex> lock(queueMutex);

    jobsFinishedCond.wait(lock, [this] 
    {
        return jobs.empty() && (activeThreads == 0);
    });
}

/**
* The loop that is run by each worker thread. Waits for new jobs, takes care of them
* and notifies condition variables if necessary.
*/
void ThreadPool::_workerLoop()
{
    std::function<void()> job;
    while (true)
    {
        // LOCKED ACQUIRE JOB LOGIC
        {
            std::unique_lock<std::mutex> lock(queueMutex);

            // Wait until new job or shutdown
            jobAvailableCond.wait(lock, [this]
            {
                return !jobs.empty() || shutDown;
            }); // checked once, then the thread is blocked until the cond variable is signaled by enqueue

            if (shutDown && jobs.empty()) { return; }

            // Get next job
            job = std::move(jobs.front());
            jobs.pop();
            ++activeThreads;
        }

        // RUN JOB
        job();

        // LOCKED POST JOB LOGIC
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            --activeThreads;

            if (jobs.empty() && activeThreads == 0) { jobsFinishedCond.notify_all(); }
        }
    }
}