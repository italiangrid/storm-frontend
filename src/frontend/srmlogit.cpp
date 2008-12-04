/*
 * Copyright (C) 2004 by CERN
 * All rights reserved
 */

#ifndef lint
static char sccsid[] = "@(#)$RCSfile$ $Revision$ $Date$ CERN Jean-Philippe Baud";
#endif /* not lint */

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <stdarg.h>
#include "srm_server.h"
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include <boost/thread.hpp>
#include <sstream>
#include "srmlogit.h"

extern int jid;

static pthread_mutex_t log_mutex= PTHREAD_MUTEX_INITIALIZER;;

static FILE *log_fd = NULL;
static int loglevel = STORM_LOG_ERROR;

/** This function init the log file. At this moment, open the file in
    append mode and set the global (to the srmlogit.c file) variable.

    This function also initialize and lock the mutex used by the
    srmlogit() function in order to avoid concurrency problem in
    writing to the log file.

    @return int   this function return 0 in case of success, or the
                  value of the global variable errno modified by
                  fopen() in case the fopen() fail.
**/
int srmlogit_init(const char* logfile) {

    if (NULL == logfile) {
        log_fd = stderr;
        return 0;
    }

    // this locking is actually not needed...
    pthread_mutex_lock(&log_mutex);

    log_fd = fopen(logfile, "a");

    pthread_mutex_unlock(&log_mutex);

    if (log_fd == NULL) {
        if (NULL == logfile)
            fprintf(stderr, "Cannot open log file\n");
        else
            fprintf(stderr, "Cannot open log file %s\n", logfile);
        return 1;
    }
    return 0;
}

int srmlogit_set_debuglevel(int level)
{
    loglevel = level;
    return 0;
}

int srmlogit(int level, const char *func, const char *msg, ...) {
    va_list args;
    char prtbuf[LOGBUFSZ];
    int save_errno;
    int Tid = 0;
    int prefix_msg_len;
    int max_char_to_write;
    int desired_buf_len;
    struct tm *tm;
#if defined(_REENTRANT) || defined(_THREAD_SAFE)
    struct tm tmstruc;
#endif
    time_t current_time;

    if (level > loglevel)
        return 0;

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
    const char* tid = (oss.str()).c_str();

    prefix_msg_len = sprintf(prtbuf, "%02d/%02d %02d:%02d:%02d %s %s: ", tm->tm_mon + 1,
            tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, tid, func);

    max_char_to_write = LOGBUFSZ - prefix_msg_len - 1;
    desired_buf_len = vsnprintf(prtbuf + prefix_msg_len, max_char_to_write, msg, args);
    va_end(args);

    // Simple (and not 100% correct, but it is enough) check on overflow in writing prtbuf
    if (desired_buf_len >= max_char_to_write) {
        sprintf(prtbuf + (LOGBUFSZ - 12), " TRUNCATED\n\0");
    }

    pthread_mutex_lock(&log_mutex);

    fwrite(prtbuf, sizeof(char), strlen(prtbuf), log_fd);
    fflush(log_fd);

    pthread_mutex_unlock(&log_mutex);

    errno = save_errno;
    return 0;
}
