/*
 * Monitoring.hpp
 *
 *  Created on: Jun 4, 2009
 *      Author: alb
 */

#ifndef MONITORING_HPP_
#define MONITORING_HPP_

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>

#include <string>
#include <iostream>

#include "srmlogit.h"

namespace storm {

class Monitoring;

class Monitoring {
private:
    static Monitoring* instance;
    const char* _funcName;
    volatile int _sleep_interval;
    volatile bool _stop;
    boost::thread _monitoring_thread;
    const char* _template_msg;
    bool _enabled;

    static void thread_function(Monitoring* m);

    boost::mutex _ping_mutex;
    volatile int _ping_completed;
    volatile int _ping_errors;
    volatile float _ping_aet;

    Monitoring(int sleep_interval) {

        _enabled = true;
        _funcName = "Monitoring";
        _stop = false;

        _template_msg = "%s: completed=%u errors=%u rps=%f aet=%f\n";

        resetData();

        _sleep_interval = sleep_interval;

        _monitoring_thread = boost::thread(boost::bind(&thread_function, this));

    }

    void pingSummary() {

        float rps = (float) _ping_completed / (float) _sleep_interval;

        srmlogit(STORM_AUDIT, _funcName, _template_msg, "Ping", _ping_completed, _ping_errors, rps, _ping_aet);
    }

    void resetData() {

        boost::mutex::scoped_lock ping_lock(_ping_mutex);
        _ping_completed = 0;
        _ping_errors = 0;
        _ping_aet = -1;
        ping_lock.unlock();
    }

public:
    ~Monitoring() {
        _stop = true;
        _monitoring_thread.interrupt();
        _monitoring_thread.join();
    }

    static Monitoring* getInstance() {
        if (instance == NULL) {
            instance = new Monitoring(60);
        }
        return instance;
    }

    void notifyPingCompleted(long executionTimeInMills, bool success) {
        boost::mutex::scoped_lock lock(_ping_mutex);

        _ping_completed++;

        if (!success) {
            _ping_errors++;
        }

        if (executionTimeInMills != -1) {
            if (_ping_aet != -1) {
                _ping_aet = (_ping_aet + ((float) executionTimeInMills / 1000.0)) / 2.0;
            } else {
                _ping_aet = (float) executionTimeInMills / 1000.0;
            }
        }
    }

    void setEnabled(bool val) {
        _enabled = val;
    }

    void setTimeInterval(int timeInterval) {
        _sleep_interval = timeInterval;
    }
};
}
#endif /* MONITORING_HPP_ */

