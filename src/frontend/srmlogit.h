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

#ifndef SRMLOGIT_H
#define SRMLOGIT_H

#define STORM_LOG_NONE  0
#define STORM_LOG_ERROR 1
#define STORM_LOG_WARNING 2
#define STORM_LOG_INFO 3
#define STORM_LOG_DEBUG 4
#define STORM_LOG_DEBUG2 5

#ifdef __cplusplus
extern "C" {
#endif

int srmlogit_set_debuglevel(int level);
int srmlogit_init(const char* logfile, const char* auditfile, int auditEnabled);
int srmlogit(int level,const char *func, const char *msg, ...);
int srmAudit(const char *msg, ...) ;
int srmLogRequest(const char* requestName, const char* clientIP, const char* clientDN);
int srmLogRequestWithSurls(const char* requestName, const char* clientIP, const char* clientDN, const char* surls, int surlsNum);
int srmLogRequestWithToken(const char* requestName, const char* clientIP, const char* clientDN, const char* requestToken);
int srmLogRequestWithTokenAndSurls(const char* requestName, const char* clientIP, const char* clientDN, const char* requestToken, const char* surls, int surlsNum);

#ifdef __cplusplus
}
#endif

#endif

