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
#include "boost/date_time/posix_time/posix_time.hpp"

#include <string>
#include <iostream>
#include <math.h>

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
    volatile int _round;
    volatile bool _running;
    volatile bool detailed;
    boost::thread _monitoring_thread;
    const char* _details_template_msg;
    const char* _summary_header_template_msg;
    const char* _summary_template_msg;
    boost::posix_time::ptime start_time;

    std::vector<Monitor*> monitor_vector;
    boost::recursive_mutex _mutex;

    class Summary{
    public:
    	std::string name;
		int completed;
		int failed;
		int errors;
		float maxTime;
		float minTime;
		float totalTime;

		void reset()
		{
			this->completed = 0;
			this->failed = 0;
			this->errors = 0;
			this->maxTime = 0;
			this->minTime = 0;
			this->totalTime = 0;
		}

		int getSuccess()
		{
			return (this->completed - (this->failed + this->errors));
		}

		float getAverageTime()
		{
			if(this->completed == 0)
			{
				return 0;
			}
			return (this->totalTime/(float)this->completed);
		}

    };

    static void thread_function(Monitoring* m);

    Monitoring(int sleep_interval) : _sleep_interval(sleep_interval){
    	start_time = boost::posix_time::microsec_clock::local_time();
    	_round = 0;
    	_running = false;
    	detailed = false;
    	_funcName = "Monitoring";
    	_details_template_msg = "[%s] [OK:%u F:%u E:%u,Avg:%.3f,Std Dev:%.3f,m:%.3f M:%.3f]\n";
    	_summary_template_msg = "[#%6u lifetime=%02u:%02u:%02u] %s [OK:%u F:%u E:%u,m:%.3f M:%.3f,Avg:%.3f ] %s [OK:%u F:%u E:%u,m:%.3f M:%.3f,Avg:%.3f] Last:(%s [OK:%u F:%u E:%u,m:%.3f M:%.3f] %s [OK:%u F:%u E:%u,m:%.3f M:%.3f])\n";
    }

    void printSummary() {
		boost::lock_guard<boost::recursive_mutex> _lock(_mutex);
		Summary round_synch_summary = buildRoundSummary(Monitor::Synchronous);
		Summary round_asynch_summary = buildRoundSummary(Monitor::Asynchronous);
		this->endRound();
		Summary synch_summary = buildSummary(Monitor::Synchronous);
		Summary asynch_summary = buildSummary(Monitor::Asynchronous);
		srmAudit(this->_summary_template_msg, this->_round,
				this->computeUpTimeHours(),
				this->computeUpTimeMins(),
				this->computeUpTimeSecs(), synch_summary.name.c_str(),
				synch_summary.getSuccess(), synch_summary.failed,
				synch_summary.errors, synch_summary.minTime,
				synch_summary.maxTime, synch_summary.getAverageTime(),
				asynch_summary.name.c_str(), asynch_summary.getSuccess(),
				asynch_summary.failed, asynch_summary.errors,
				asynch_summary.minTime, asynch_summary.maxTime,
				asynch_summary.getAverageTime(),
				round_synch_summary.name.c_str(),
				round_synch_summary.getSuccess(), round_synch_summary.failed,
				round_synch_summary.errors, round_synch_summary.minTime,
				round_synch_summary.maxTime, round_asynch_summary.name.c_str(),
				round_asynch_summary.getSuccess(), round_asynch_summary.failed,
				round_asynch_summary.errors, round_asynch_summary.minTime,
				round_asynch_summary.maxTime);
    	}

    Summary buildRoundSummary(Monitor::OperationType type)
    {
    	Summary new_summary;
    	new_summary.reset();
    	new_summary.name = Monitor::nameOfOperationType(type);
    	for (std::vector<Monitor *>::iterator it = this->monitor_vector.begin(); it
    	    						!= this->monitor_vector.end(); ++it)
		{
			if(((Monitor*)*it)->isUpdated() && ((Monitor*)*it)->getType() == type)
			{
				new_summary.completed += ((Monitor*)*it)->getCompletedRound();
				new_summary.failed += ((Monitor*)*it)->getFailedRound();
				new_summary.errors += ((Monitor*)*it)->getErrorsRound();
				if(new_summary.minTime == 0 || ((Monitor*)*it)->getMinTimeRound() < new_summary.minTime)
				{
					new_summary.minTime = ((Monitor*)*it)->getMinTimeRound();
				}
				if(new_summary.maxTime == 0 || ((Monitor*)*it)->getMaxTimeRound() > new_summary.maxTime)
				{
					new_summary.maxTime = ((Monitor*)*it)->getMaxTimeRound();
				}
				new_summary.totalTime+= ((Monitor*)*it)->getTotalTimeRound();
			}
		}
    	return new_summary;
    }

    Summary buildSummary(Monitor::OperationType type)
    {
    	Summary new_summary;
    	new_summary.reset();
    	new_summary.name = Monitor::nameOfOperationType(type);
    	for (std::vector<Monitor *>::iterator it = this->monitor_vector.begin(); it
    	    						!= this->monitor_vector.end(); ++it)
		{
			if(!((Monitor*)*it)->isEmpty() && ((Monitor*)*it)->getType() == type)
			{
				new_summary.completed += ((Monitor*)*it)->getCompleted();
				new_summary.failed += ((Monitor*)*it)->getFailed();
				new_summary.errors += ((Monitor*)*it)->getErrors();
				if(new_summary.minTime == 0 || ((Monitor*)*it)->getMinTime() < new_summary.minTime)
				{
					new_summary.minTime = ((Monitor*)*it)->getMinTime();
				}
				if(new_summary.maxTime == 0 || ((Monitor*)*it)->getMaxTime() > new_summary.maxTime)
				{
					new_summary.maxTime = ((Monitor*)*it)->getMaxTime();
				}
				new_summary.totalTime+= ((Monitor*)*it)->getTotalTime();
			}
		}
    	return new_summary;
    }

    void printDetails() {
		boost::lock_guard<boost::recursive_mutex> _lock(_mutex);
		bool first = true;
		for (std::vector<Monitor *>::iterator it = this->monitor_vector.begin();
				it != this->monitor_vector.end(); ++it)
		{
			if (!((Monitor*) *it)->isEmpty())
			{
				if(first)
				{
					srmAudit("Details:\n");
					first = false;
				}
				srmAudit(this->_details_template_msg,
						((Monitor*) *it)->getFriendlyName().c_str(),
						((Monitor*) *it)->getCompleted(),
						((Monitor*) *it)->getFailed(),
						((Monitor*) *it)->getErrors(),
						((Monitor*) *it)->computeAverageExecTime(),
						((Monitor*) *it)->computeExecTimeStandardDeviation(),
						((Monitor*) *it)->getMinTime(),
						((Monitor*) *it)->getMaxTime());
			}
		}
	}

    void printRoundDetails() {
   		boost::lock_guard<boost::recursive_mutex> _lock(_mutex);
   		bool first = true;
   		for (std::vector<Monitor *>::iterator it = this->monitor_vector.begin();
   				it != this->monitor_vector.end(); ++it)
   		{
   			if (((Monitor*) *it)->isUpdated())
   			{
   				if(first)
				{
   					srmAudit("Last round details:\n");
					first = false;
				}
   				srmAudit(this->_details_template_msg,
   						((Monitor*) *it)->getFriendlyName().c_str(),
   						((Monitor*) *it)->getCompletedRound(),
   						((Monitor*) *it)->getFailedRound(),
   						((Monitor*) *it)->getErrorsRound(),
   						((Monitor*) *it)->computeAverageExecTimeRound(),
   						((Monitor*) *it)->computeExecTimeStandardDeviationRound(),
   						((Monitor*) *it)->getMinTimeRound(),
						((Monitor*) *it)->getMaxTimeRound());
   			}
   		}
   	}

    void endRound()
    {
    	for (std::vector<Monitor *>::iterator it = this->monitor_vector.begin(); it
    	   						!= this->monitor_vector.end(); ++it)
		{
			if(((Monitor*)*it)->isUpdated())
			{
				((Monitor*)*it)->digestRound();
			}
		}
    }

    int computeUpTimeHours()
    {
		boost::posix_time::time_duration upTime = (boost::posix_time::microsec_clock::local_time() - this->start_time);
    	return (int) (upTime.total_milliseconds() / 1000 / 60 / 60);
    }

    int computeUpTimeMins()
	{
		boost::posix_time::time_duration upTime = (boost::posix_time::microsec_clock::local_time() - this->start_time);
		return (int) (fmod (((float)upTime.total_milliseconds() / 1000 / 60),(float) 60));
	}

    int computeUpTimeSecs()
	{
		boost::posix_time::time_duration upTime = (boost::posix_time::microsec_clock::local_time() - this->start_time);
		return (int) (fmod ((float)upTime.total_milliseconds() / 1000,(float) 60));
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

    void setDetailed(bool detailed) {
		this->detailed = detailed;
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

    void newRound()
    {
    	_round++;
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
