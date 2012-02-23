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

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <stdarg.h>
#include "srm_server.h"
#include <stdio.h>
#include <string.h>
//#include <boost/thread/mutex.hpp>

#include "boost/thread.hpp"
#include <sstream>
#include "srmlogit.h"

extern int jid;

//static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
boost::mutex log_mutex;
boost::mutex audit_mutex;

static FILE *log_fd = NULL;
static FILE *audit_fd = NULL;
static int loglevel = STORM_LOG_ERROR;
static bool _auditEnabled;

/**
 * This function init the log file. At this moment, open the file in
 * append mode and set the global (to the srmlogit.c file) variable.
 *
 * This function is not thread-safe.
 *
 * @return int   this function return 0 in case of success, or the value
 *               of the global variable errno modified by fopen() in case
 *               the fopen() fail.
 *
 **/
int srmlogit_init(const char* logfile, const char* auditfile, int auditEnabled) {

    _auditEnabled = auditEnabled;

    if (NULL == logfile) {

        log_fd = stderr;
        return 0;

    } else {

        log_fd = fopen(logfile, "a");

        if (log_fd == NULL) {
            fprintf(stderr, "Cannot open log file %s\n", logfile);
            return 1;
        }
    }

    if (_auditEnabled) {
        if (NULL == auditfile) {

            audit_fd = stderr;
            return 0;

        } else {

            audit_fd = fopen(auditfile, "a");

            if (audit_fd == NULL) {
                fprintf(stderr, "Cannot open audit file %s\n", auditfile);
                if (log_fd != NULL) {
                    fclose(log_fd);
                }
                return 1;
            }
        }
    } else {
        audit_fd = NULL;
    }

    return 0;
}

int srmlogit_set_debuglevel(int level) {
    loglevel = level;
    return 0;
}

std::string getLogLevelLable(int logLevel)
{
	std::string lable;
	switch (logLevel)
	{
		case STORM_LOG_NONE:
			lable = std::string("NONE");
			break;
		case STORM_LOG_ERROR:
			lable = std::string("ERROR");
			break;
		case STORM_LOG_WARNING:
			lable = std::string("WARN");
			break;
		case STORM_LOG_INFO:
			lable = std::string("INFO");
			break;
		case STORM_LOG_DEBUG:
			lable = std::string("DEBUG");
			break;
		case STORM_LOG_DEBUG2:
			lable = std::string("DEBUG2");
			break;
		default:
			lable = std::string("UNKNOWN");
			break;
	}
	return lable;
}

int srmlogit(int level, const char *func, const char *msg, ...) {
    va_list args;
    char prtbuf[LOGBUFSZ];
    int save_errno;
    int Tid = 0;
    int prefix_msg_len;
    signed int max_char_to_write;
    int desired_buf_len = 0;
    struct tm *tm;
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
    struct tm tmstruc;
#endif
    time_t current_time;

    if (level > loglevel) {
        return 0;
    }

    save_errno = errno;
    va_start(args, msg);
    (void) time(&current_time); /* Get current time */
#if (defined(_REENTRANT) || defined(_THREAD_SAFE)) && !defined(_WIN32)
    (void) localtime_r(&current_time, &tmstruc);
    tm = &tmstruc;
#else
    tm = localtime(&current_time);
#endif

    std::ostringstream oss;


    oss << boost::this_thread::get_id();

    std::string tid = oss.str();
    prefix_msg_len = snprintf(prtbuf, LOGBUFSZ -1, "%02d/%02d %02d:%02d:%02d %s %s: %s: ", tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec, tid.c_str(), func , getLogLevelLable(level).c_str());

    max_char_to_write = LOGBUFSZ - prefix_msg_len - 1;
    if(max_char_to_write > 0)
    {
    	desired_buf_len = vsnprintf(prtbuf + prefix_msg_len, max_char_to_write, msg, args);
    }
    else
    {
    	sprintf(prtbuf + (LOGBUFSZ - 12), " TRUNCATED\n\0");
    }

    prtbuf[LOGBUFSZ-1] = '\0';
    va_end(args);

    // Simple (and not 100% correct, but it is enough) check on overflow in writing prtbuf
    if (desired_buf_len >= max_char_to_write) {
        sprintf(prtbuf + (LOGBUFSZ - 12), " TRUNCATED\n\0");
    }

    boost::mutex::scoped_lock lock;
    FILE *fd;

    if (level == STORM_AUDIT) {
        if (!_auditEnabled) {
            return 0;
        }
        fd = audit_fd;
        lock = boost::mutex::scoped_lock(audit_mutex);
    } else {
        fd = log_fd;
        lock = boost::mutex::scoped_lock(log_mutex);
    }

    fwrite(prtbuf, sizeof(char), strlen(prtbuf), fd);
    fflush(fd);

    lock.unlock();

    errno = save_errno;
    return 0;
}

int srmLogRequest(const char* requestName, const char* clientIP, const char* clientDN) {
    char prtbuf[LOGBUFSZ];
    int save_errno;
    int Tid = 0;
    int prefix_msg_len;
    signed int max_char_to_write;
    int desired_buf_len = 0;
    struct tm *tm;
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
    struct tm tmstruc;
#endif
    time_t current_time;

    if (STORM_LOG_INFO > loglevel) {
            return 0;
	}

    save_errno = errno;
    (void) time(&current_time); /* Get current time */
#if (defined(_REENTRANT) || defined(_THREAD_SAFE)) && !defined(_WIN32)
    (void) localtime_r(&current_time, &tmstruc);
    tm = &tmstruc;
#else
    tm = localtime(&current_time);
#endif

    std::ostringstream oss;
    oss << boost::this_thread::get_id();
    std::string tid = oss.str();

    prefix_msg_len = snprintf(prtbuf, LOGBUFSZ -1, "%02d/%02d %02d:%02d:%02d %s INFO : ", tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec, tid.c_str());

    max_char_to_write = LOGBUFSZ - prefix_msg_len - 1;
    if(max_char_to_write > 0)
    {
    	desired_buf_len = snprintf(prtbuf + prefix_msg_len, max_char_to_write,
				"Request \'%s\' from Client IP=%s Client DN=%s\n",
				requestName, clientIP, clientDN);
    }
    else
    {
    	sprintf(prtbuf + (LOGBUFSZ - 12), " TRUNCATED\n\0");
    }

    prtbuf[LOGBUFSZ-1] = '\0';

    // Simple (and not 100% correct, but it is enough) check on overflow in writing prtbuf
    if (desired_buf_len >= max_char_to_write) {
        sprintf(prtbuf + (LOGBUFSZ - 12), " TRUNCATED\n\0");
    }

    boost::mutex::scoped_lock lock;
    FILE *fd = log_fd;
	lock = boost::mutex::scoped_lock(log_mutex);

    fwrite(prtbuf, sizeof(char), strlen(prtbuf), fd);
    fflush(fd);

    lock.unlock();

    errno = save_errno;
    return 0;
}

int srmAudit(const char *msg, ...) {
    va_list args;
    char prtbuf[LOGBUFSZ];
    int save_errno;
    int prefix_msg_len;
    signed int max_char_to_write;
    int desired_buf_len = 0;
    struct tm *tm;
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
    struct tm tmstruc;
#endif
	if (!_auditEnabled) {
		return 0;
	}
    time_t current_time;

    save_errno = errno;
    va_start(args, msg);
    (void) time(&current_time); /* Get current time */
#if (defined(_REENTRANT) || defined(_THREAD_SAFE)) && !defined(_WIN32)
    (void) localtime_r(&current_time, &tmstruc);
    tm = &tmstruc;
#else
    tm = localtime(&current_time);
#endif

    prefix_msg_len = snprintf(prtbuf, LOGBUFSZ -1, "%02d/%02d %02d:%02d:%02d : ", tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);

    max_char_to_write = LOGBUFSZ - prefix_msg_len - 1;
    if(max_char_to_write > 0)
    {
    	desired_buf_len = vsnprintf(prtbuf + prefix_msg_len, max_char_to_write, msg, args);
    }
    else
    {
    	sprintf(prtbuf + (LOGBUFSZ - 12), " TRUNCATED\n\0");
    }

    prtbuf[LOGBUFSZ-1] = '\0';
    va_end(args);

    // Simple (and not 100% correct, but it is enough) check on overflow in writing prtbuf
    if (desired_buf_len >= max_char_to_write) {
        sprintf(prtbuf + (LOGBUFSZ - 12), " TRUNCATED\n\0");
    }

    boost::mutex::scoped_lock lock = boost::mutex::scoped_lock(audit_mutex);

    fwrite(prtbuf, sizeof(char), strlen(prtbuf), audit_fd);
    fflush(audit_fd);

    lock.unlock();

    errno = save_errno;
    return 0;
}
