#ifndef SRMLOGIT_H
#define SRMLOGIT_H
/* Copyright (C) 2006 by Antonio Messina <antonio.messina@ictp.it> for the ICTP project EGRID. */
/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#define STORM_LOG_NONE  0
#define STORM_LOG_ERROR 1
#define STORM_LOG_WARNING 2
#define STORM_LOG_INFO 3
#define STORM_LOG_DEBUG 4
#define STORM_LOG_DEBUG2 5
#define STORM_LOG_DEBUG3 6
#define STORM_LOG_DEBUG4 7

#ifdef __cplusplus
extern "C" {
#endif

int srmlogit_set_debuglevel(int level);
int srmlogit_init(const char* logfile);
int srmlogit(int level,const char *func, const char *msg, ...);

#ifdef __cplusplus
}
#endif

#endif
