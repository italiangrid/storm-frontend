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
#include <boost/date_time/posix_time/posix_time.hpp>

#include <string>
#include <iostream>
#include <set>
#include <math.h>

#include "srmlogit.h"

#include "Monitor.hpp"
#include "MonitorStub.hpp"
#include "MonitorNotEnabledException.hpp"

#include <vector>

#include "FrontendConfiguration.hpp"
#include "ThreadPool.hpp"

namespace storm {

class Monitoring;

class Monitoring {
private:
    static Monitoring* m_instance;
    static int M_DEFAULT_SLEEP_INTERVALL;
    const char* m_funcName;
    volatile int m_sleep_interval;
    volatile int m_round;
    volatile bool m_running;
    volatile bool m_detailed;
    boost::thread m_monitoring_thread;
    const char* m_details_template_msg;
    const char* m_summary_header_template_msg;
    const char* m_summary_template_msg;
    boost::posix_time::ptime m_start_time;

    std::vector<Monitor*> m_monitor_vector;
    boost::recursive_mutex m_mutex;

    Monitor* m_defaultMonitor;

    class Summary{
    public:
    	std::string m_name;
		int m_completed;
		int m_failed;
		int m_errors;
		float m_maxTime;
		float m_minTime;
		float m_totalTime;

		void reset()
		{
			m_completed = 0;
			m_failed = 0;
			m_errors = 0;
			m_maxTime = 0;
			m_minTime = 0;
			m_totalTime = 0;
		}

		int getSuccess()
		{
			return (m_completed - (m_failed + m_errors));
		}

		float getAverageTime()
		{
			if(m_completed == 0)
			{
				return 0;
			}
			return (m_totalTime/(float)m_completed);
		}

    };

    static void thread_function(Monitoring* m);

    Monitoring(int sleep_interval) : m_sleep_interval(sleep_interval){
    	m_start_time = boost::posix_time::microsec_clock::local_time();
    	m_round = 0;
    	m_running = false;
    	m_detailed = false;
    	m_funcName = "Monitoring";
    	m_details_template_msg = "[%s] [OK:%u,F:%u,E:%u,Avg:%.3f,Std Dev:%.3f,m:%.3f,M:%.3f]\n";
    	m_summary_template_msg = "[#%6u lifetime=%02u:%02u:%02u] %s [OK:%u,F:%u,E:%u,m:%.3f,M:%.3f,Avg:%.3f] %s [OK:%u,F:%u,E:%u,m:%.3f,M:%.3f,Avg:%.3f] Last:(%s [OK:%u,F:%u,E:%u,m:%.3f,M:%.3f] %s [OK:%u,F:%u,E:%u,m:%.3f,M:%.3f]) Tasks(max_active:%u,active:%u,max_pending:%u,pending:%u)\n";
    	m_defaultMonitor = new MonitorStub();
    }

    void printSummary() {
		boost::lock_guard<boost::recursive_mutex> lock(m_mutex);
		Summary round_synch_summary = buildRoundSummary(Monitor::Synchronous);
		Summary round_asynch_summary = buildRoundSummary(Monitor::Asynchronous);
		this->endRound();
		Summary synch_summary = buildSummary(Monitor::Synchronous);
		Summary asynch_summary = buildSummary(Monitor::Asynchronous);

		srmAudit(m_summary_template_msg, m_round,
				this->computeUpTimeHours(),
				this->computeUpTimeMins(),
				this->computeUpTimeSecs(), synch_summary.m_name.c_str(),
				synch_summary.getSuccess(), synch_summary.m_failed,
				synch_summary.m_errors, synch_summary.m_minTime,
				synch_summary.m_maxTime, synch_summary.getAverageTime(),
				asynch_summary.m_name.c_str(), asynch_summary.getSuccess(),
				asynch_summary.m_failed, asynch_summary.m_errors,
				asynch_summary.m_minTime, asynch_summary.m_maxTime,
				asynch_summary.getAverageTime(),
				round_synch_summary.m_name.c_str(),
				round_synch_summary.getSuccess(), round_synch_summary.m_failed,
				round_synch_summary.m_errors, round_synch_summary.m_minTime,
				round_synch_summary.m_maxTime, round_asynch_summary.m_name.c_str(),
				round_asynch_summary.getSuccess(), round_asynch_summary.m_failed,
				round_asynch_summary.m_errors, round_asynch_summary.m_minTime,
				round_asynch_summary.m_maxTime,
				storm::ThreadPool::getInstance()->size(),
				storm::ThreadPool::getInstance()->get_active(),
				FrontendConfiguration::getInstance()->getThreadpoolMaxPending(),
				storm::ThreadPool::getInstance()->get_pending());
    	}

    Summary buildRoundSummary(Monitor::OperationType type)
    {
    	Summary new_summary;
    	new_summary.reset();
    	new_summary.m_name = Monitor::nameOfOperationType(type);
    	std::vector<Monitor*>::const_iterator const vectorEnd = m_monitor_vector.end();
    	for (std::vector<Monitor*>::iterator it = m_monitor_vector.begin();
    			it != vectorEnd; ++it)
		{
			if(((Monitor*)*it)->isUpdated() && ((Monitor*)*it)->getType() == type)
			{
				new_summary.m_completed += ((Monitor*)*it)->getCompletedRound();
				new_summary.m_failed += ((Monitor*)*it)->getFailedRound();
				new_summary.m_errors += ((Monitor*)*it)->getErrorsRound();
				if(new_summary.m_minTime == 0 || ((Monitor*)*it)->getMinTimeRound() < new_summary.m_minTime)
				{
					new_summary.m_minTime = ((Monitor*)*it)->getMinTimeRound();
				}
				if(new_summary.m_maxTime == 0 || ((Monitor*)*it)->getMaxTimeRound() > new_summary.m_maxTime)
				{
					new_summary.m_maxTime = ((Monitor*)*it)->getMaxTimeRound();
				}
				new_summary.m_totalTime+= ((Monitor*)*it)->getTotalTimeRound();
			}
		}
    	return new_summary;
    }

    Summary buildSummary(Monitor::OperationType type)
    {
    	Summary new_summary;
    	new_summary.reset();
    	new_summary.m_name = Monitor::nameOfOperationType(type);
    	std::vector<Monitor*>::const_iterator const vectorEnd = m_monitor_vector.end();
    	for (std::vector<Monitor *>::iterator it = m_monitor_vector.begin();
    			it != vectorEnd; ++it)
		{
			if(!((Monitor*)*it)->isEmpty() && ((Monitor*)*it)->getType() == type)
			{
				new_summary.m_completed += ((Monitor*)*it)->getCompleted();
				new_summary.m_failed += ((Monitor*)*it)->getFailed();
				new_summary.m_errors += ((Monitor*)*it)->getErrors();
				if(new_summary.m_minTime == 0 || ((Monitor*)*it)->getMinTime() < new_summary.m_minTime)
				{
					new_summary.m_minTime = ((Monitor*)*it)->getMinTime();
				}
				if(new_summary.m_maxTime == 0 || ((Monitor*)*it)->getMaxTime() > new_summary.m_maxTime)
				{
					new_summary.m_maxTime = ((Monitor*)*it)->getMaxTime();
				}
				new_summary.m_totalTime+= ((Monitor*)*it)->getTotalTime();
			}
		}
    	return new_summary;
    }

    void printDetails() {
		boost::lock_guard<boost::recursive_mutex> lock(m_mutex);
		bool first = true;
		std::vector<Monitor*>::const_iterator const vectorEnd = m_monitor_vector.end();
		for (std::vector<Monitor *>::iterator it = m_monitor_vector.begin();
				it != vectorEnd; ++it)
		{
			if (!((Monitor*) *it)->isEmpty())
			{
				if(first)
				{
					srmAudit("Details:\n");
					first = false;
				}
				srmAudit(m_details_template_msg,
						((Monitor*) *it)->getFriendlyName().c_str(),
						((Monitor*) *it)->getSuccessfull(),
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
   		boost::lock_guard<boost::recursive_mutex> lock(m_mutex);
   		bool first = true;
   		std::vector<Monitor*>::const_iterator const vectorEnd = m_monitor_vector.end();
   		for (std::vector<Monitor *>::iterator it = m_monitor_vector.begin();
   				it != vectorEnd; ++it)
   		{
   			if (((Monitor*) *it)->isUpdated())
   			{
   				if(first)
				{
   					srmAudit("Last round details:\n");
					first = false;
				}
   				srmAudit(m_details_template_msg,
   						((Monitor*) *it)->getFriendlyName().c_str(),
   						((Monitor*) *it)->getSuccessfullRound(),
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
    	std::vector<Monitor*>::const_iterator const vectorEnd = m_monitor_vector.end();
    	for (std::vector<Monitor *>::iterator it = m_monitor_vector.begin();
    			it != vectorEnd; ++it)
		{
			if(((Monitor*)*it)->isUpdated())
			{
				((Monitor*)*it)->digestRound();
			}
		}
    }

    int computeUpTimeHours()
    {
		boost::posix_time::time_duration upTime = (boost::posix_time::microsec_clock::local_time() - m_start_time);
    	return (int) (upTime.total_milliseconds() / 1000 / 60 / 60);
    }

    int computeUpTimeMins()
	{
		boost::posix_time::time_duration upTime = (boost::posix_time::microsec_clock::local_time() - m_start_time);
		return (int) (fmod (((float)upTime.total_milliseconds() / 1000 / 60),(float) 60));
	}

    int computeUpTimeSecs()
	{
		boost::posix_time::time_duration upTime = (boost::posix_time::microsec_clock::local_time() - m_start_time);
		return (int) (fmod ((float)upTime.total_milliseconds() / 1000,(float) 60));
	}

public:
    ~Monitoring() {
    	m_running = false;
    	std::vector<Monitor*>::const_iterator const vectorEnd = m_monitor_vector.end();
    	for (std::vector<Monitor*>::iterator it = m_monitor_vector.begin();
    			it != vectorEnd; ++it)
		{
				delete (Monitor*)*it;
		}
    	delete m_defaultMonitor;
    	m_monitoring_thread.interrupt();
    	m_monitoring_thread.join();
    }

    static Monitoring* getInstance() {
        if (m_instance == NULL) {
        	m_instance = new Monitoring(M_DEFAULT_SLEEP_INTERVALL);
        }
        return m_instance;
    }

    void setTimeInterval(int timeInterval) {
    	m_sleep_interval = timeInterval;
	}

    void setDetailed(bool detailed) {
		m_detailed = detailed;
	}

    void start()
    {
    	boost::lock_guard<boost::recursive_mutex> lock(m_mutex);
    	if(!m_running)
    	{
    		m_monitoring_thread = boost::thread(boost::bind(&thread_function, this));
    		m_running = true;
    	}
    }

    void newRound()
    {
    	m_round++;
    }

    void addMonitor(Monitor* monitor)
    {
    	boost::lock_guard<boost::recursive_mutex> lock(m_mutex);
    	m_monitor_vector.push_back(monitor);
    }

    Monitor* getMonitor(std::string name)
	{
    	boost::lock_guard<boost::recursive_mutex> lock(m_mutex);
    	std::vector<Monitor*>::const_iterator const vectorEnd = m_monitor_vector.end();
		for (std::vector<Monitor *>::iterator it = m_monitor_vector.begin();
				it != vectorEnd; ++it)
		{
			if(((Monitor*)*it)->getName() == name)
			{
				return *it;
			}
		}
		if(m_defaultMonitor == NULL)
		{
			std::string message("Monitor named " + name + " not registered");
			storm::MonitorNotEnabledException exc(message);
			throw exc;
		}
		else
		{
			return m_defaultMonitor;
		}
	}

    std::set<std::string> getMonitorNames()
   	{
       	boost::lock_guard<boost::recursive_mutex> lock(m_mutex);
       	std::set<std::string> names;
       	std::vector<Monitor*>::const_iterator const vectorEnd = m_monitor_vector.end();
   		for (std::vector<Monitor *>::iterator it = m_monitor_vector.begin();
   				it != vectorEnd; ++it)
   		{
   			names.insert(((Monitor*)*it)->getName());
   		}
   		return names;
   	}
};
}
#endif /* MONITORING_HPP_ */
