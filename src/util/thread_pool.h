#ifndef thread_pool_h
#define thread_pool_h

#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <functional>
#if defined _WIN64
#include <windows.h>
#endif

enum class RWThreadPriority {
    low,
    mid,
    high
};

#if defined __APPLE__
qos_class_t qos_class(RWThreadPriority prio) {
    switch (prio) {
    case RWThreadPriority::low: return QOS_CLASS_BACKGROUND;
    case RWThreadPriority::mid: return QOS_CLASS_DEFAULT;
    case RWThreadPriority::high: return QOS_CLASS_USER_INITIATED;
    }
}
#elif defined _WIN64
int win_thread_priority(RWThreadPriority prio) {
    switch (prio) {
    case RWThreadPriority::low: return THREAD_PRIORITY_LOWEST;
    case RWThreadPriority::mid: return THREAD_PRIORITY_NORMAL;
    case RWThreadPriority::high: return THREAD_PRIORITY_HIGHEST;
    }
}
#endif

class ThreadPool {
public:
    ThreadPool(size_t num_threads, RWThreadPriority thread_priority) {
        this->num_threads = num_threads;
        this->thread_priority = thread_priority;
        this->_stop = false;
        this->on_empty_callback = []{};
        this->setup();
    }
    
    void setOnEmptyCallback(std::function<void()> on_empty_callback) {
        this->on_empty_callback = on_empty_callback;
    }
    
    ~ThreadPool() {
        stop();
    }
    
    void enqueue(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(tasks_mutex);
            tasks.emplace(std::move(task));
        }
        condition.notify_one();
    }
    
    void stop() {
        {
            std::unique_lock<std::mutex> lock(tasks_mutex);
            _stop = true;
        }
        condition.notify_all();
    }
    
    void join() {
        for (auto& thread: threads) {
            if (thread.joinable())
                thread.join();
        }
    }
    
private:
    size_t num_threads;
    RWThreadPriority thread_priority;
    std::function<void()> on_empty_callback;
    std::queue<std::function<void()>> tasks;
    std::vector<std::thread> threads;
    std::condition_variable condition;
    std::mutex tasks_mutex;
    bool _stop;
    
    void setup() {
        for(int i=0; i<num_threads; i++) {
            std::thread thread = std::thread( [this] {
                
                #if defined __APPLE__
                pthread_set_qos_class_self_np(QOS_CLASS_USER_INITIATED, 0);
                #endif

                while (true) {
                    std::function<void()> task;
                    bool is_empty = false;
                    {
                        std::unique_lock<std::mutex> lock(tasks_mutex);
                        condition.wait(lock, [this] {
                            return !this->tasks.empty() || this->_stop;
                        });
                        
                        if (this->_stop) {
                            return;
                        }
                        
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                        is_empty = this->tasks.empty();
                        
                        // don't call here anything that ends up calling the mutex, it will deadlock
                    }
                    
                    task();
                    
                    // don't use mutex again here, it will deadlock
                }
            });

            #if defined _WIN64
            int prio = win_thread_priority(this->thread_priority);
            HANDLE thread_handle = (HANDLE)thread.native_handle();
            SetThreadPriority(thread_handle, THREAD_PRIORITY_HIGHEST);
            #endif
            
            threads.emplace_back(std::move(thread));
        }
    }
    
public:
    static void test() {
        ThreadPool pool(4, RWThreadPriority::high);
        const int num_workers = 4;
        std::latch countdown(num_workers);
        for (int i = 0; i <= num_workers; ++i) {
            pool.enqueue([i, &countdown] {
                std::stringstream str_buffer;
                str_buffer << std::this_thread::get_id() << " " << i << std::endl;
                std::cout << str_buffer.str();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                countdown.count_down();
            });
        }
        countdown.wait();
        pool.stop();
        pool.join();
    }
};

#endif /* thread_pool_h */
