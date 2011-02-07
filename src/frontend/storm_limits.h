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


#ifndef _STORM_LIMITS_H
#define _STORM_LIMITS_H

    /* all maximum lengths defined below do not include the trailing null */

#define ST_MAXACLENTRIES 300    /* maximum number of ACL entries for a file/dir */
#define ST_MAXCLASNAMELEN 15    /* maximum length for a fileclass name */
#define ST_MAXCOMMENTLEN 255    /* maximum length for user comments in metadata */
#define ST_MAXDENFIELDS    8    /* maximum number of density values in devinfo */
#define ST_MAXDENLEN       8    /* maximum length for a alphanumeric density */
#define ST_MAXDGNLEN       6    /* maximum length for a device group name */
#define ST_MAXDPMTOKENLEN 36    /* maximum length for a DPM token */
#define ST_MAXDVNLEN      63    /* maximum length for a device name */
#define ST_MAXDVTLEN       8    /* maximum length for a device type */
#define ST_MAXFIDLEN      17    /* maximum length for a fid (DSN) */
#define ST_MAXFSEQLEN     14    /* maximum length for a fseq string */
#define ST_MAXGRPNAMELEN   2    /* maximum length for a group name */
#define ST_MAXGUIDLEN     36    /* maximum length for a guid */
#define ST_MAXHOSTNAMELEN 63    /* maximum length for a hostname */
#define ST_MAXLBLTYPLEN    3    /* maximum length for a label type */
#define ST_MAXLINELEN   1023    /* maximum length for a line in a log */
#define ST_MAXMANUFLEN    12    /* maximum length for a cartridge manufacturer */
#define ST_MAXMIGPNAMELEN 15    /* maximum length for a migration policy name */
#define ST_MAXMIGRNAMELEN 15    /* maximum length for a migrator name */
#define ST_MAXMLLEN        1    /* maximum length for a cartridge media_letter */
#define ST_MAXMODELLEN     6    /* maximum length for a cartridge model */
#define ST_MAXNAMELEN    231    /* maximum length for a pathname component */
#define ST_MAXNBDRIVES    32    /* maximum number of tape drives per server */
#define ST_MAXPATHLEN   1023    /* maximum length for a pathname */
#define ST_MAXPOLICYLEN   15    /* maximum length for a policy name */
#define ST_MAXPOOLNAMELEN 15    /* maximum length for a pool name */
#define ST_MAXPROTOLEN     7    /* maximum length for a protocol name */
#define ST_MAXPROTOLISTLEN     10*ST_MAXPROTOLEN   /* maximum length for a protocol list */
#define ST_MAXRBTNAMELEN  17    /* maximum length for a robot name */
#define ST_MAXRECFMLEN     3    /* maximum length for a record format */
#define ST_MAXREGEXPLEN   63    /* Maximum length for a regular expression */
#define ST_MAXSFNLEN    1103    /* maximum length for a replica */
#define ST_MAXSHORTHOSTLEN 10   /* maximum length for a hostname without domain */
#define ST_MAXSNLEN       24    /* maximum length for a cartridge serial nb */
#define ST_MAXSTGRIDLEN   77    /* maximum length for a stager full request id */
                /* must be >= nb digits in ST_MAXSTGREQID +
                   ST_MAXHOSTNAMELEN + 8 */
#define ST_MAXSTGREQID 999999   /* maximum value for a stager request id */
#define ST_MAXSYMLINKS     5    /* maximum number of symbolic links */
#define ST_MAXTAGLEN     255    /* maximum length for a volume tag */
#define ST_MAXTAPELIBLEN   8    /* maximum length for a tape library name */
#define ST_MAXUNMLEN       8    /* maximum length for a drive name */
#define ST_MAXUSRNAMELEN  14    /* maximum length for a login name */
#define ST_MAXVIDLEN       6    /* maximum length for a VID */
#define ST_MAXVSNLEN       6    /* maximum length for a VSN */

/* Max allowed uid/gif */
#define ST_MAXUID    0x7FFFFFFF /* Maximum uid */
#define ST_MAXGID    0x7FFFFFFF /* Maximum gid */
#endif
