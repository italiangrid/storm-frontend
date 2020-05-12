/* Copyright (c) Istituto Nazionale di Fisica Nucleare (INFN). 2006-2010.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef THREADPOOL_HPP_
#define THREADPOOL_HPP_

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <queue>
#include <iostream>
#include <string>

#include <utility>
#include <map>

#include <sstream>

#include "FrontendConfiguration.hpp"

namespace storm{

typedef boost::function0<void> task_func;

class SynchronizedQueue {

   private:
      std::queue<task_func> _q;
      size_t _max_size;
      boost::condition _queue_not_empty;
      boost::condition _queue_not_full;
      boost::mutex monitor;

   public:
      SynchronizedQueue(int size)
      : _max_size(size)
      {
        assert(size > 0);
      }

      ~SynchronizedQueue() {
      }

      task_func pop() {

         boost::mutex::scoped_lock lk(monitor);
         while (_q.empty()) {
            _queue_not_empty.wait(lk);
         }
         task_func func = _q.front();
         _q.pop();
         _queue_not_full.notify_one();
         return func;
      }

      void push(task_func func) {
         boost::mutex::scoped_lock lk(monitor);
         while (_q.size() == _max_size) {
           _queue_not_full.wait(lk);
         }
         _q.push(func);
         _queue_not_empty.notify_one();
      }

      int get_size() {
         return _q.size();
      }
};

class ThreadPool {
   private:

      static ThreadPool* instance;

      int _nThreads;
      int _queueSize;
      int _activeThreads;
      bool _stop;
      boost::thread* _thread_pool;
      SynchronizedQueue _sq;
      boost::mutex _mutex;

      std::map<boost::thread::id, int> idMap;

      ThreadPool(int nThreads, int max_pending)
      :  _nThreads(nThreads),
         _queueSize(max_pending),
         _activeThreads(0),
         _stop(false),
         _thread_pool(new boost::thread[_nThreads]),
         _sq(max_pending)
      {
         int i = 0;

         try {
            for (; i < _nThreads; i++) {
               _thread_pool[i] = boost::thread(boost::bind(&thread_function, &_sq, &_activeThreads, &_mutex,
                        &_stop));
               idMap.insert(std::make_pair(_thread_pool[i].get_id(), i));
            }
         } catch (boost::thread_resource_error& e) {
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

      static void thread_function(SynchronizedQueue* sq, int* activeThreads, boost::mutex* mutex,
            bool* stop) {

         try {
            for (;;) {
               task_func func = (*sq).pop();
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
         } catch (boost::thread_interrupted& e) {
         }

      }

   public:

      ~ThreadPool() {

         _stop = true;

         for (int i = 0; i < _nThreads; i++) {
            _thread_pool[i].interrupt();
         }

         for (int i = 0; i < _nThreads; i++) {
            _thread_pool[i].join();
         }

         delete[] _thread_pool;

         instance = NULL;
      }

      static ThreadPool* getInstance(){
         if(instance == NULL)
         {
            buildInstance(DEFAULT_THREADS_NUMBER, DEFAULT_THREADPOOL_MAX_PENDING);
         }
         return instance;
      }

      static void buildInstance(int size, int max_pending)
      {
         instance = new ThreadPool(size, max_pending);
      }

      static bool isInstanceAvailable()
      {
         return instance != NULL;
      }

      void schedule(task_func const & func) {
         _sq.push(func);
      }

      int get_pending() {
         return _sq.get_size();
      }

      int get_active() {
         return _activeThreads;
      }

      int size() const {
         return _nThreads;
      }

      /**
       * returns an empty string if no id found
       **/
      std::string getThreadIdLable(boost::thread::id id)
      {
         std::string lable;
         if(this->idMap.find(id) != this->idMap.end())
         {
            int idLable = this->idMap.find(id)->second;
            lable = "Thread " + boost::lexical_cast<std::string>(idLable) + " - ";
         }

         return lable;
      }

      int getThreadNumber(boost::thread::id id) const
      {
         std::map<boost::thread::id, int>::const_iterator it = idMap.find(id);
         return it != idMap.end() ? it->second : -1;
      }
};
}
#endif /* THREADPOOL_HPP_ */
