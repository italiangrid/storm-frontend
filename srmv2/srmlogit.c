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
#include "Cglobals.h"
#include "srm_server.h"
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "srmlogit.h"

extern int jid;
extern char logfile;

static pthread_mutex_t log_mutex= PTHREAD_MUTEX_INITIALIZER;;

static FILE *log_fd=NULL;
static int loglevel=1;

/** This function init the log file. At this moment, open the file in
    append mode and set the global (to the srmlogit.c file) variable.

    This function also initialize and lock the mutex used by the
    srmlogit() function in order to avoid concurrency problem in
    writing to the log file.

    @return int   this function return 0 in case of success, or the
                  value of the global variable errno modified by
                  fopen() in case the fopen() fail.
**/
int srmlogit_set_debuglevel(int level)
{
    loglevel = level;
    return 0;
}

int srmlogit_init() {
    /* this locking is non-necessary... */
    pthread_mutex_lock(&log_mutex);
    log_fd = fopen(logfile, "a");
    pthread_mutex_unlock(&log_mutex);
    if (log_fd == NULL) {
        int e = errno;
        char ebuf[256];
        char *err = NULL;
        err = strerror_r(e, ebuf, 256);
        fprintf(stderr, "error opening file %s, error = '%s'\n", logfile, err);
        return e;
    }
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
    Cglobals_getTid(&Tid);
    if (Tid < 0) /* main thread */
        sprintf(prtbuf, "%02d/%02d %02d:%02d:%02d %5d %s: ", tm->tm_mon + 1, tm->tm_mday,
                tm->tm_hour, tm->tm_min, tm->tm_sec, jid, func);
    else
        sprintf(prtbuf, "%02d/%02d %02d:%02d:%02d %5d,%d %s: ", tm->tm_mon + 1, tm->tm_mday,
                tm->tm_hour, tm->tm_min, tm->tm_sec, jid, Tid, func);
    prefix_msg_len = strlen(prtbuf);
    max_char_to_write = LOGBUFSZ - prefix_msg_len - 1;
    desired_buf_len = vsnprintf(prtbuf + prefix_msg_len, max_char_to_write, msg, args);
    // Simple (and not 100% correct, but it is enough) check on overflow in writing prtbuf
    if (desired_buf_len >= max_char_to_write)
        sprintf(prtbuf + (LOGBUFSZ - 12), " TRUNCATED\n\0");
    va_end(args);

    pthread_mutex_lock(&log_mutex);
    if (log_fd == NULL)
        log_fd = stderr;
    fwrite(prtbuf, sizeof(char), strlen(prtbuf), log_fd);
    fflush(log_fd);
    pthread_mutex_unlock(&log_mutex);

    errno = save_errno;
    return 0;
}
/*  srm_logreq - log a request */

/*  Split the message into lines so they don't exceed LOGBUFSZ-1 characters
 *  A backslash is appended to a line to be continued
 *  A continuation line is prefixed by '+ '
 */
//void
//srm_logreq(int ll, const char *func, char *logbuf)
//{
//    int n1, n2;
//    char *p;
//    char savechrs1[2];
//    char savechrs2[2];
//
//    n1 = LOGBUFSZ - strlen (func) - 36;
//    n2 = strlen (logbuf);
//    p = logbuf;
//    while (n2 > n1) {
//        savechrs1[0] = *(p + n1);
//        savechrs1[1] = *(p + n1 + 1);
//        *(p + n1) = '\\';
//        *(p + n1 + 1) = '\0';
//        srmlogit (ll, func, SRM98, p);
//        if (p != logbuf) {
//            *p = savechrs2[0];
//            *(p + 1) = savechrs2[1];
//        }
//        p += n1 - 2;
//        savechrs2[0] = *p;
//        savechrs2[1] = *(p + 1);
//        *p = '+';
//        *(p + 1) = ' ';
//        *(p + 2) = savechrs1[0];
//        *(p + 3) = savechrs1[1];
//        n2 -= n1;
//    }
//    srmlogit (ll, func, SRM98, p);
//    if (p != logbuf) {
//        *p = savechrs2[0];
//        *(p + 1) = savechrs2[1];
//    }
//}
