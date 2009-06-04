#ifndef THREADPOOL_HPP_
#define THREADPOOL_HPP_

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <queue>
#include <iostream>

using namespace std;

namespace sq {
typedef boost::function0<void> task_func;

class SynchronizedQueue {

private:
    std::queue<task_func> _q;
    boost::condition _queue_not_empty;
    boost::mutex _monitor;

public:
    SynchronizedQueue() {
    }

    task_func pop() {

        boost::mutex::scoped_lock lk(_monitor);

        while (_q.empty()) {
            _queue_not_empty.wait(lk);
        }

        task_func func = _q.front();
        _q.pop();

        return func;
    }

    void push(task_func func) {

        boost::mutex::scoped_lock lk(_monitor);

        _q.push(func);
        _queue_not_empty.notify_one();
    }

    int get_size() {
        return _q.size();
    }
};
}

class ThreadPool {
private:

    int _nThreads;
    int _queueSize;
    int _activeThreads;
    bool _stop;
    boost::thread* _thread_pool;
    sq::SynchronizedQueue _sq;
    boost::mutex _mutex;

    static void thread_function(sq::SynchronizedQueue* sq, int* activeThreads, boost::mutex* mutex,
            bool* stop) {
        try {
            for (;;) {

                sq::task_func func = (*sq).pop();

                if (*stop) {
                    break;
                }

                boost::mutex::scoped_lock lk(*mutex);
                (*activeThreads)++;
                lk.unlock();

                func();

                lk.lock();
                (*activeThreads)--;
                lk.unlock();
            }
        } catch (boost::thread_interrupted e) {
            cout << "Bye!\n";
        }

        if (*stop) {
            cout << "Bye by stop!\n";
        }
    }

public:
    ThreadPool(int nThreads) {
        _nThreads = nThreads;
        _queueSize = nThreads * 2;
        _activeThreads = 0;
        _stop = false;

        _thread_pool = new boost::thread[_nThreads];

        int i = 0;
        try {
            for (; i < _nThreads; i++) {
                _thread_pool[i] = boost::thread(boost::bind(&thread_function, &_sq, &_activeThreads, &_mutex,
                        &_stop));
            }
        } catch (boost::thread_resource_error e) {
            _stop = true;
            for (; i >= 0; i--) {
                _thread_pool[i].interrupt();
            }
            for (int i = 0; i < _nThreads; i++) {
                _thread_pool[i].join();
            }
            throw e;
        }
    }

    ~ThreadPool() {

        _stop = true;

        for (int i = 0; i < _nThreads; i++) {
            _thread_pool[i].interrupt();
        }

        for (int i = 0; i < _nThreads; i++) {
            _thread_pool[i].join();
        }

        delete[] _thread_pool;
    }

    void schedule(sq::task_func const & func) {
        _sq.push(func);
    }

    int get_pending() {
        return _sq.get_size();
    }

    int get_active() {
        return _activeThreads;
    }
};

#endif /* THREADPOOL_HPP_ */
