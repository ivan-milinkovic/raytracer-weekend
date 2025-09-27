#ifndef thread_pool_h
#define thread_pool_h

#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <functional>

class ThreadPool {
public:
    ThreadPool(size_t num_threads, qos_class_t qos_class) {
        this->num_threads = num_threads;
        this->qos_class = qos_class;
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
    qos_class_t qos_class;
    std::function<void()> on_empty_callback;
    std::queue<std::function<void()>> tasks;
    std::vector<std::thread> threads;
    std::condition_variable condition;
    std::mutex tasks_mutex;
    bool _stop;
    
    void setup() {
        for(int i=0; i<num_threads; i++) {
            std::thread thread = std::thread( [this] {
                pthread_set_qos_class_self_np(QOS_CLASS_USER_INITIATED, 0);
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
                    
                    if (is_empty) {
                        this->on_empty_callback();
                    }
                    
                    // don't use mutex again here, it will deadlock
                }
            });
            
            threads.emplace_back(std::move(thread));
        }
    }
};

#endif /* thread_pool_h */
