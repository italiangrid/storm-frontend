#include "Monitoring.hpp"

storm::Monitoring* storm::Monitoring::instance = NULL;

void storm::thread_function(Monitoring* m) {
    try {
        for (;;) {

            sleep(m->_sleep_interval);

            if (m->_stop) {
                break;
            }

            if (m->_enabled) {
                m->pingSummary();
                m->resetData();
            }
        }
    } catch (boost::thread_interrupted e) {
    }
}

