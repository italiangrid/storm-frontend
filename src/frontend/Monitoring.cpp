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

#include "Monitoring.hpp"
#include "srmlogit.h"
#include <boost/thread/exceptions.hpp>

storm::Monitoring* storm::Monitoring::m_instance = NULL;
int storm::Monitoring::M_DEFAULT_SLEEP_INTERVALL = 60;


void storm::Monitoring::thread_function(Monitoring* m) {
    try {
        for (;;) {
            m->newRound();
            boost::this_thread::sleep(boost::posix_time::seconds(m->m_sleep_interval));

            if (!m->m_running) {
                break;
            }

            //print by round
            if (m->m_detailed) {
                m->printRoundDetails();
            }


            //print all history
            m->printSummary();
            if (m->m_detailed) {
                m->printDetails();
            }

        }
    } catch (boost::thread_interrupted &e) {
        srmlogit(STORM_LOG_ERROR, m->m_funcName, "Received thread_interrupted exception\n");
    }
}

