#include <vector>
#include <queue>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

/**
* @object ThreadPool
* @brief A preinitialized pool of threads which jobs can be assigned to.
*/
class ThreadPool
{
    public:

        /**
        * Initialize a pool of the given size that will wait for jobs until the pool is destroyed.
        */
        ThreadPool(size_t numThreads);

        ~ThreadPool();
        
        /**
        * Submits the given job to the pool queue. 
        * The next free worker thread will handle the job.
        */
        void enqueue(std::function<void()> job);

        /**
        * Waits until all jobs in the queue are finished.
        */
        void waitFinishAll();

    private:

        /**
        * The loop that is run by each worker thread. Waits for new jobs, takes care of them
        * and notifies condition variables if necessary.
        */
        void _workerLoop();

        // Used to notify workers of shutdown
        bool shutDown;

        // WORKERS
        std::vector<std::thread> workers;
        std::condition_variable jobAvailableCond; // Condition variable used to wait for new jobs

        // JOB QUEUE
        std::queue<std::function<void()>> jobs;
        std::mutex queueMutex;


        // WORKER COUNTING
        std::condition_variable jobsFinishedCond;
        std::atomic<size_t> activeThreads;      
};