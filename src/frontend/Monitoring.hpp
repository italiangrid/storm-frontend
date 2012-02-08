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

#ifndef MONITORING_HPP_
#define MONITORING_HPP_

#include <boost/thread/thread.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/bind.hpp>

#include <string>
#include <iostream>

#include "srmlogit.h"

#include "Monitor.hpp"
#include "MonitorNotEnabledException.hpp"

#include <vector>

namespace storm {

class Monitoring;

class Monitoring {
private:
    static Monitoring* instance;
    const char* _funcName;
    volatile int _sleep_interval;
    volatile bool _running;
    boost::thread _monitoring_thread;
    const char* _template_msg;

    std::vector<Monitor*> monitor_vector;
    boost::recursive_mutex _mutex;

    static void thread_function(Monitoring* m);

    Monitoring(int sleep_interval) : _sleep_interval(sleep_interval){

    	_running = false;
    	_funcName = "Monitoring";
    	_template_msg = "%s: completed=%u failed=%u errors=%u average dur.=%f\n";
    }

    void printSummary() {
    	boost::lock_guard<boost::recursive_mutex> _lock(_mutex);
		for (std::vector<Monitor *>::iterator it = this->monitor_vector.begin(); it
						!= this->monitor_vector.end(); ++it)
		{
			if(! ((Monitor*)*it)->isEmpty())
			{
				srmlogit(STORM_AUDIT, _funcName, _template_msg,
						((Monitor*)*it)->getName().c_str(), ((Monitor*)*it)->getCompleted(),
						((Monitor*)*it)->getFailed(), ((Monitor*)*it)->getErrors(),
						((Monitor*)*it)->getAverageExecTime());
			}
		}
	}

    void resetMonitors() {
    	boost::lock_guard<boost::recursive_mutex> _lock(_mutex);
    	for (std::vector<Monitor *>::iterator it = this->monitor_vector.begin(); it
						!= this->monitor_vector.end(); ++it)
		{
				((Monitor*)*it)->reset();
		}
	}

public:
    ~Monitoring() {
    	_running = false;
    	for (std::vector<Monitor *>::iterator it = this->monitor_vector.begin(); it
    							!= this->monitor_vector.end(); ++it)
		{
				delete (Monitor*)*it;
		}
        _monitoring_thread.interrupt();
        _monitoring_thread.join();
    }

    static Monitoring* getInstance() {
        if (instance == NULL) {
            instance = new Monitoring(60);
        }
        return instance;
    }

    void setTimeInterval(int timeInterval) {
		_sleep_interval = timeInterval;
	}

    void start()
    {
    	boost::lock_guard<boost::recursive_mutex> _lock(_mutex);
    	if(!_running)
    	{
			_monitoring_thread = boost::thread(boost::bind(&thread_function, this));
			_running = true;
    	}
    }

    void addMonitor(Monitor* monitor)
    {
    	boost::lock_guard<boost::recursive_mutex> _lock(_mutex);
    	this->monitor_vector.push_back(monitor);
    }

    Monitor* getMonitor(std::string name) throw (MonitorNotEnabledException)
	{
    	boost::lock_guard<boost::recursive_mutex> _lock(_mutex);
		for (std::vector<Monitor *>::iterator it = this->monitor_vector.begin(); it
						!= this->monitor_vector.end(); ++it)
		{
			if(((Monitor*)*it)->getName() == name)
			{
				return *it;
			}
		}
		std::string message("Monitor named " + name + " not registered");
		storm::MonitorNotEnabledException exc(message);
		throw exc;
	}
};
}
#endif /* MONITORING_HPP_ */
