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
#include <sys/time.h>
#include <time.h>
#include <stdarg.h>
#include "srm_server.h"
#include <stdio.h>
#include <string.h>

#include <boost/thread.hpp>
#include "ThreadPool.hpp"
#include <sstream>
#include "srmlogit.h"
#include "storm_util.h"
#include "request_id.hpp"

extern int jid;

boost::mutex log_mutex;
boost::mutex audit_mutex;

static FILE *log_fd = NULL;
static FILE *audit_fd = NULL;
static int loglevel = STORM_LOG_ERROR;
static bool m_auditEnabled;

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
	int result = 0;

	m_auditEnabled = auditEnabled;
    if (NULL == logfile) {

        log_fd = stderr;

    } else {

        log_fd = fopen(logfile, "a");

        if (log_fd == NULL) {
            fprintf(stderr, "Cannot open log file %s\n", logfile);
            result = 1;
        }
    }

    if (m_auditEnabled) {
        if (NULL == auditfile) {

            audit_fd = stderr;

        } else {

            audit_fd = fopen(auditfile, "a");

            if (audit_fd == NULL) {
                fprintf(stderr, "Cannot open audit file %s\n", auditfile);
                if (log_fd != NULL) {
                    fclose(log_fd);
                }
                result = 1;
            }
        }
    } else {
        audit_fd = NULL;
    }

    return result;
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


int logPrefix(char* ptrbuf, int log_level, const char* function_name) {

    struct timeval tv;
    struct tm* tm_p;
    struct tm tm;

    if (gettimeofday(&tv, NULL) == -1){
	// error handling
    }

    suseconds_t msecs = tv.tv_usec / 1000;

    tm_p = localtime_r(&tv.tv_sec,&tm);
    if (tm_p == NULL) {
	// error handling
    }

    std::string  tid;
    if(storm::ThreadPool::isInstanceAvailable())
    {
	    tid = storm::ThreadPool::getInstance()->getThreadIdLable(boost::this_thread::get_id());
    }
    if(tid.empty())
    {
	    tid = std::string("Main - ");
    }

    const char * request_id = storm::get_request_id();

    if(function_name == NULL)
    {
	    return snprintf(ptrbuf, LOGBUFSZ -1, "%02d/%02d %02d:%02d:%02d.%03d %s %s [%s]: ", tm_p->tm_mon + 1, tm_p->tm_mday,
				    tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec, msecs, tid.c_str(), getLogLevelLable(log_level).c_str(),
				    request_id == NULL ? "?" : request_id);
    }
    else
    {
	    return snprintf(ptrbuf, LOGBUFSZ -1, "%02d/%02d %02d:%02d:%02d.%03d %s %s [%s]: %s : ", tm_p->tm_mon + 1, tm_p->tm_mday,
				    tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec, msecs, tid.c_str(), getLogLevelLable(log_level).c_str(), 
				    request_id == NULL ? "?" : request_id, function_name);
    }
}

int logPrefix(char* ptrbuf, int log_level){

    return logPrefix(ptrbuf, log_level, NULL);
}

int writeLogPrefix(char* prtbuf, int logLevel, const char* functionName)
{
	struct tm *tm;
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
	struct tm tmstruc;
#endif
	time_t current_time;

(void) time(&current_time); /* Get current time */
#if (defined(_REENTRANT) || defined(_THREAD_SAFE)) && !defined(_WIN32)
	(void) localtime_r(&current_time, &tmstruc);
	tm = &tmstruc;
#else
	tm = localtime(&current_time);
#endif

	std::string  tid;
	if(storm::ThreadPool::isInstanceAvailable())
	{
		tid = storm::ThreadPool::getInstance()->getThreadIdLable(boost::this_thread::get_id());
	}
	if(tid.empty())
	{
		tid = std::string("Main - ");
	}
	if(functionName == NULL)
	{
		return snprintf(prtbuf, LOGBUFSZ -1, "%02d/%02d %02d:%02d:%02d %s %s : ", tm->tm_mon + 1, tm->tm_mday,
					tm->tm_hour, tm->tm_min, tm->tm_sec, tid.c_str(), getLogLevelLable(logLevel).c_str());
	}
	else
	{
		return snprintf(prtbuf, LOGBUFSZ -1, "%02d/%02d %02d:%02d:%02d %s %s : %s : ", tm->tm_mon + 1, tm->tm_mday,
					tm->tm_hour, tm->tm_min, tm->tm_sec, tid.c_str(), getLogLevelLable(logLevel).c_str(), functionName);
	}
}

int writeLogPrefix(char* prtbuf, int logLevel)
{
	return writeLogPrefix(prtbuf, logLevel, NULL);
}

int loggingError(const char* requestName) {
	int save_errno = errno;
	char prtbuf[LOGBUFSZ];
	signed int max_char_to_write = LOGBUFSZ - 1;
	int writtenChars = 0;
	writtenChars += logPrefix(prtbuf, STORM_LOG_ERROR);
	if(writtenChars < max_char_to_write)
	{
		if(requestName != NULL)
		{
			writtenChars += snprintf(prtbuf + writtenChars, max_char_to_write - writtenChars,
					"Unable to log message from function %s, null arguments\n",
					requestName);
		}
		else
		{
			writtenChars += snprintf(prtbuf + writtenChars, max_char_to_write - writtenChars,
					"Unable to log message, null arguments\n");
		}
	}

	if (writtenChars < max_char_to_write) {
		prtbuf[writtenChars] = '\0';
	}
	else
	{
		sprintf(prtbuf + (LOGBUFSZ - 12), " TRUNCATED\n\0");
	}
	boost::mutex::scoped_lock lock = boost::mutex::scoped_lock(log_mutex);
	size_t const written_bytes = fwrite(prtbuf, sizeof(char), strlen(prtbuf), log_fd);
	if (written_bytes < strlen(prtbuf)){
		// This will likely never happen, but we should print something...

	}
	fflush(log_fd);
	lock.unlock();

	errno = save_errno;
	return 0;
}


int srmlogit(int level, const char *func, const char *msg, ...) {
    va_list args;
    int save_errno = errno;
    char prtbuf[LOGBUFSZ];
    signed int max_char_to_write = LOGBUFSZ - 1;
    int writtenChars = 0;

    if (level > loglevel) {
        return 0;
    }

    va_start(args, msg);
    writtenChars += logPrefix(prtbuf, level , func);
    if(writtenChars < max_char_to_write)
    {
    	writtenChars += vsnprintf(prtbuf + writtenChars, max_char_to_write - writtenChars, msg, args);
    }
    va_end(args);
    if(writtenChars < max_char_to_write)
    {
    	prtbuf[writtenChars] = '\0';
    }
    else
    {
    	sprintf(prtbuf + (LOGBUFSZ - 12), " TRUNCATED\n\0");
    }

    boost::mutex::scoped_lock lock = boost::mutex::scoped_lock(log_mutex);
    size_t const written_bytes = fwrite(prtbuf, sizeof(char), strlen(prtbuf), log_fd);
    if (written_bytes < strlen(prtbuf)){

    	// TODO: This will likely never happen, but we should print something...

    }
    fflush(log_fd);
    lock.unlock();

    errno = save_errno;
    return 0;
}


int srmLogRequest(const char* requestName, const char* clientIP, const char* clientDN) {
	int save_errno = errno;
    char prtbuf[LOGBUFSZ];
    signed int max_char_to_write = LOGBUFSZ - 1;
    int writtenChars = 0;
    if(requestName == NULL || clientIP == NULL || clientDN == NULL )
	{
		loggingError(requestName);
		errno = save_errno;
		return 1;
	}

    writtenChars += logPrefix(prtbuf, STORM_LOG_INFO);
    if(writtenChars < max_char_to_write)
	{
    	writtenChars += snprintf(prtbuf + writtenChars, max_char_to_write - writtenChars,
				"Request \'%s\' from Client IP=%s Client DN=%s\n",
				requestName, clientIP, clientDN);
    }
    if(writtenChars < max_char_to_write)
    {
    	prtbuf[writtenChars] = '\0';
    }
    else
    {
    	sprintf(prtbuf + (LOGBUFSZ - 12), " TRUNCATED\n\0");
    }

    boost::mutex::scoped_lock lock = boost::mutex::scoped_lock(log_mutex);
    size_t const written_bytes = fwrite(prtbuf, sizeof(char), strlen(prtbuf), log_fd);
    if (written_bytes < strlen(prtbuf)){

        	// TODO: This will likely never happen, but we should print something...

    }
    fflush(log_fd);
    lock.unlock();

    errno = save_errno;
    return 0;
}

int srmLogRequestWithSurls(const char* requestName, const char* clientIP, const char* clientDN, const char* surls, int surlsNum)
{
	int save_errno = errno;
	char prtbuf[LOGBUFSZ];
	signed int max_char_to_write = LOGBUFSZ - 1;
	int writtenChars = 0;

	if(requestName == NULL || clientIP == NULL || clientDN == NULL || surls == NULL)
	{
		loggingError(requestName);
		errno = save_errno;
		return 1;
	}
	writtenChars += logPrefix(prtbuf, STORM_LOG_INFO);
	if(writtenChars < max_char_to_write)
	{
		writtenChars += snprintf(prtbuf + writtenChars, max_char_to_write - writtenChars,
				"Request \'%s\' from Client IP=\'%s\' Client DN=\'%s\'",
				requestName, clientIP, clientDN);
	}

	if (writtenChars < max_char_to_write)
	{
		if(surls != NULL && surlsNum > 0)
		{
			writtenChars += snprintf(prtbuf + writtenChars, max_char_to_write - writtenChars,
					" # Requested \'%u\' SURL(s): \'%s\'\n",
					surlsNum, surls);
		}
		else
		{
			prtbuf[writtenChars] = '\n';
			writtenChars++;
		}
	}

	if (writtenChars < max_char_to_write) {
		prtbuf[writtenChars] = '\0';
	}
	else
	{
		sprintf(prtbuf + (LOGBUFSZ - 12), " TRUNCATED\n\0");
	}

	boost::mutex::scoped_lock lock = boost::mutex::scoped_lock(log_mutex);
	size_t const written_bytes = fwrite(prtbuf, sizeof(char), strlen(prtbuf), log_fd);
	if (written_bytes < strlen(prtbuf)){

	    	// TODO: This will likely never happen, but we should print something...

	}
	fflush(log_fd);
	lock.unlock();

	errno = save_errno;
	return 0;
}

int srmLogRequestWithToken(const char* requestName, const char* clientIP, const char* clientDN, const char* requestToken)
{
	int save_errno = errno;
	char prtbuf[LOGBUFSZ];
	signed int max_char_to_write = LOGBUFSZ - 1;
	int writtenChars = 0;
	if(requestName == NULL || clientIP == NULL || clientDN == NULL || requestToken == NULL)
	{
		loggingError(requestName);
		errno = save_errno;
		return 1;
	}

	writtenChars += logPrefix(prtbuf, STORM_LOG_INFO);
	if(writtenChars < max_char_to_write)
	{
		writtenChars += snprintf(prtbuf + writtenChars, max_char_to_write - writtenChars,
				"Request \'%s\' from Client IP=\'%s\' Client DN=\'%s\' # Requested token \'%s\'\n",
				requestName, clientIP, clientDN, requestToken);
	}

	if (writtenChars < max_char_to_write) {
		prtbuf[writtenChars] = '\0';
	}
	else
	{
		sprintf(prtbuf + (LOGBUFSZ - 12), " TRUNCATED\n\0");
	}

	boost::mutex::scoped_lock lock = boost::mutex::scoped_lock(log_mutex);
	size_t const written_bytes = fwrite(prtbuf, sizeof(char), strlen(prtbuf), log_fd);
	if (written_bytes < strlen(prtbuf)){

	    	// TODO: This will likely never happen, but we should print something...

	}
	fflush(log_fd);
	lock.unlock();

	errno = save_errno;
	return 0;
}

int srmLogRequestWithTokenList(const char* requestName, const char* clientIP, const char* clientDN, const char* tokens, int tokensNum)
{
	int save_errno = errno;
	char prtbuf[LOGBUFSZ];
	signed int max_char_to_write = LOGBUFSZ - 1;
	int writtenChars = 0;

	if(requestName == NULL || clientIP == NULL || clientDN == NULL || tokens == NULL)
	{
		loggingError(requestName);
		errno = save_errno;
		return 1;
	}
	writtenChars += logPrefix(prtbuf, STORM_LOG_INFO);
	if(writtenChars < max_char_to_write)
	{
		writtenChars += snprintf(prtbuf + writtenChars, max_char_to_write - writtenChars,
				"Request \'%s\' from Client IP=\'%s\' Client DN=\'%s\'",
				requestName, clientIP, clientDN);
	}

	if (writtenChars < max_char_to_write)
	{
		if(tokens != NULL && tokensNum > 0)
		{
			writtenChars += snprintf(prtbuf + writtenChars, max_char_to_write - writtenChars,
					" # Requested \'%u\' token(s): \'%s\'\n",
					tokensNum, tokens);
		}
		else
		{
			prtbuf[writtenChars] = '\n';
			writtenChars++;
		}
	}

	if (writtenChars < max_char_to_write) {
		prtbuf[writtenChars] = '\0';
	}
	else
	{
		sprintf(prtbuf + (LOGBUFSZ - 12), " TRUNCATED\n\0");
	}

	boost::mutex::scoped_lock lock = boost::mutex::scoped_lock(log_mutex);
	size_t const written_bytes = fwrite(prtbuf, sizeof(char), strlen(prtbuf), log_fd);
	if (written_bytes < strlen(prtbuf)){

	    	// TODO: This will likely never happen, but we should print something...

	}
	fflush(log_fd);
	lock.unlock();

	errno = save_errno;
	return 0;
}
int srmLogRequestWithTokenAndSurls(const char* requestName, const char* clientIP, const char* clientDN, const char* requestToken, const char* surls, int surlsNum)
{
	int save_errno = errno;
	char prtbuf[LOGBUFSZ];
	signed int max_char_to_write = LOGBUFSZ - 1;
	int writtenChars = 0;

	if(requestName == NULL || clientIP == NULL || clientDN == NULL || requestToken == NULL || surls == NULL)
	{
		loggingError(requestName);
		errno = save_errno;
		return 1;
	}
	writtenChars += logPrefix(prtbuf, STORM_LOG_INFO);
	if(writtenChars < max_char_to_write)
	{
		writtenChars += snprintf(prtbuf + writtenChars, max_char_to_write - writtenChars,
				"Request \'%s\' from Client IP=\'%s\' Client DN=\'%s\' # Requested token \'%s\'",
				requestName, clientIP, clientDN, requestToken);
	}

	if (writtenChars < max_char_to_write)
	{
		if(surls != NULL && surlsNum > 0)
		{
			writtenChars += snprintf(prtbuf + writtenChars, max_char_to_write - writtenChars,
					" on \'%u\' SURL(s): \'%s\'\n",
					surlsNum, surls);
		}
		else
		{
			prtbuf[writtenChars] = '\n';
			writtenChars++;
		}
	}

	if (writtenChars < max_char_to_write) {
		prtbuf[writtenChars] = '\0';
	}
	else
	{
		sprintf(prtbuf + (LOGBUFSZ - 12), " TRUNCATED\n\0");
	}

	boost::mutex::scoped_lock lock = boost::mutex::scoped_lock(log_mutex);
	size_t const written_bytes = fwrite(prtbuf, sizeof(char), strlen(prtbuf), log_fd);
	if (written_bytes < strlen(prtbuf)){

	    	// TODO: This will likely never happen, but we should print something...

	}
	fflush(log_fd);
	lock.unlock();

	errno = save_errno;
	return 0;
}

int srmLogResponseWithToken(const char* requestName, const char* requestToken, const ns1__TStatusCode statusCode)
{
	int save_errno = errno;
	char prtbuf[LOGBUFSZ];
	signed int max_char_to_write = LOGBUFSZ - 1;
	int writtenChars = 0;
	if(requestName == NULL || requestToken == NULL)
	{
		loggingError(requestName);
		errno = save_errno;
		return 1;
	}

	writtenChars += logPrefix(prtbuf, STORM_LOG_INFO);
	if(writtenChars < max_char_to_write)
	{
		writtenChars += snprintf(prtbuf + writtenChars, max_char_to_write - writtenChars,
				"Result for request \'%s\' is \'%s\'. # Produced request token: \'%s\'\n",
				requestName, reconvertStatusCode(statusCode), requestToken);
	}

	if (writtenChars < max_char_to_write) {
		prtbuf[writtenChars] = '\0';
	}
	else
	{
		sprintf(prtbuf + (LOGBUFSZ - 12), " TRUNCATED\n\0");
	}

	boost::mutex::scoped_lock lock = boost::mutex::scoped_lock(log_mutex);
	size_t const written_bytes = fwrite(prtbuf, sizeof(char), strlen(prtbuf), log_fd);
	if (written_bytes < strlen(prtbuf)){

		// TODO: This will likely never happen, but we should print something...

	}
	fflush(log_fd);
	lock.unlock();

	errno = save_errno;
	return 0;
}

int srmLogResponse(const char* requestName, const ns1__TStatusCode statusCode)
{
	int save_errno = errno;
	char prtbuf[LOGBUFSZ];
	signed int max_char_to_write = LOGBUFSZ - 1;
	int writtenChars = 0;
	if(requestName == NULL)
	{
		loggingError(requestName);
		errno = save_errno;
		return 1;
	}

	writtenChars += logPrefix(prtbuf, STORM_LOG_INFO);
	if(writtenChars < max_char_to_write)
	{
		writtenChars += snprintf(prtbuf + writtenChars, max_char_to_write - writtenChars,
				"Result for request \'%s\' is \'%s\'\n",
				requestName, reconvertStatusCode(statusCode));
	}

	if (writtenChars < max_char_to_write) {
		prtbuf[writtenChars] = '\0';
	}
	else
	{
		sprintf(prtbuf + (LOGBUFSZ - 12), " TRUNCATED\n\0");
	}

	boost::mutex::scoped_lock lock = boost::mutex::scoped_lock(log_mutex);
	size_t const written_bytes = fwrite(prtbuf, sizeof(char), strlen(prtbuf), log_fd);
	if (written_bytes < strlen(prtbuf)){

	    	// TODO: This will likely never happen, but we should print something...

	}
	fflush(log_fd);
	lock.unlock();
	errno = save_errno;
	return 0;
}

int srmAudit(const char *msg, ...) {
    va_list args;
    char prtbuf[LOGBUFSZ];
    int save_errno = errno;
    signed int max_char_to_write = LOGBUFSZ - 1;
	int writtenChars = 0;
    struct tm *tm;
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
    struct tm tmstruc;
#endif
	if (!m_auditEnabled) {
		return 0;
	}
    time_t current_time;

    va_start(args, msg);
    (void) time(&current_time); /* Get current time */
#if (defined(_REENTRANT) || defined(_THREAD_SAFE)) && !defined(_WIN32)
    (void) localtime_r(&current_time, &tmstruc);
    tm = &tmstruc;
#else
    tm = localtime(&current_time);
#endif

    writtenChars += snprintf(prtbuf, LOGBUFSZ -1, "%02d/%02d %02d:%02d:%02d : ", tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);

    if(writtenChars < max_char_to_write)
    {
    	writtenChars += vsnprintf(prtbuf + writtenChars, max_char_to_write - writtenChars, msg, args);
    }
    va_end(args);
    if (writtenChars < max_char_to_write) {
		prtbuf[writtenChars] = '\0';
	}
	else
	{
		sprintf(prtbuf + (LOGBUFSZ - 12), " TRUNCATED\n\0");
	}

    boost::mutex::scoped_lock lock = boost::mutex::scoped_lock(audit_mutex);
    size_t const written_bytes = fwrite(prtbuf, sizeof(char), strlen(prtbuf), audit_fd);
    if (written_bytes < strlen(prtbuf)){

    	// TODO: This will likely never happen, but we should print something...

    }
    fflush(audit_fd);
    lock.unlock();

    errno = save_errno;
    return 0;
}
