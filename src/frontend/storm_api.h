/*
 * $Id$
 */

#ifndef _STORM_API_H
#define _STORM_API_H
#include "storm_constants.h"
#include "osdep.h"
#include "storm_struct.h"

int *C__storm_errno();
#define storm_errno (*C__storm_errno())

			/* Disk Pool Manager client structures */

struct storm_api_thread_info {
	char *		errbufp;
	int		errbuflen;
	int		initialized;
	int		dp_errno;
};

struct storm_copyfilereq {
	char		*from_surl;
	char		*to_surl;
	time_t		lifetime;
	char		f_type;
	char		s_token[ST_MAXDPMTOKENLEN+1];
	int		flags;
};

struct storm_copyfilestatus {
	char		*from_surl;
	char		*to_surl;
	u_signed64	filesize;
	int		status;
	char		*errstring;
	time_t		pintime;
};

struct storm_filestatus {
	char		*surl;
	int		status;
	char		*errstring;
};

struct storm_getfilereq {
	char		*from_surl;
	time_t		lifetime;
	char		f_type;
	char		s_token[ST_MAXDPMTOKENLEN+1];
	int		flags;
};

struct storm_getfilestatus {
	char		*from_surl;
	char		*turl;
	u_signed64	filesize;
	int		status;
	char		*errstring;
	time_t		pintime;
};

struct storm_putfilereq {
	char		*to_surl;
	time_t		lifetime;
	char		f_type;
	char		s_token[ST_MAXDPMTOKENLEN+1];
	u_signed64	requested_size;
};

struct storm_putfilestatus {
	char		*to_surl;
	char		*turl;
	u_signed64	filesize;
	int		status;
	char		*errstring;
	time_t		pintime;
};

struct storm_reqsummary {
	char		r_token[ST_MAXDPMTOKENLEN+1];
	char		r_type;
	int		nb_reqfiles;
	int		nb_queued;
	int		nb_finished;
	int		nb_progress;
};

struct storm_space_metadata {
	char		s_type;
	char		s_token[ST_MAXDPMTOKENLEN+1];
	char		client_dn[256];
	u_signed64	t_space;	/* Total space */
	u_signed64	g_space;	/* Guaranteed space */
	u_signed64	u_space;	/* Unused space */
	time_t		a_lifetime;	/* Lifetime assigned */
	time_t		r_lifetime;	/* Remaining lifetime */
};

struct storm_tokeninfo {
	char		r_token[ST_MAXDPMTOKENLEN+1];
	time_t		c_time;
};

			/* function prototypes */

EXTERN_C int DLL_DECL storm_abortreq _PROTO((char *));
EXTERN_C int DLL_DECL storm_addfs _PROTO((char *, char *, char *, int));
EXTERN_C int DLL_DECL storm_addpool _PROTO((struct storm_pool *));
EXTERN_C int DLL_DECL storm_copy _PROTO((int, struct storm_copyfilereq *, char *, int, time_t, char *, int *, struct storm_copyfilestatus **));
EXTERN_C int DLL_DECL storm_errmsg _PROTO((char *, char *, ...));
EXTERN_C int DLL_DECL storm_extendfilelife _PROTO((char *, char *, time_t, time_t *));
EXTERN_C int DLL_DECL storm_get _PROTO((int, struct storm_getfilereq *, int, char **, char *, time_t, char *, int *, struct storm_getfilestatus **));
EXTERN_C int DLL_DECL storm_getpoolfs _PROTO((char *, int *, struct storm_fs **));
EXTERN_C int DLL_DECL storm_getpools _PROTO((int *, struct storm_pool **));
EXTERN_C int DLL_DECL storm_getprotocols _PROTO((int *, char ***));
EXTERN_C int DLL_DECL storm_getreqid _PROTO((const char *, int *, struct storm_tokeninfo **));
EXTERN_C int DLL_DECL storm_getreqsummary _PROTO((int, char **, int *, struct storm_reqsummary **));
EXTERN_C int DLL_DECL storm_getspacemd _PROTO((int, char **, int *, struct storm_space_metadata **));
EXTERN_C int DLL_DECL storm_getspacetoken _PROTO((const char *, int *, char ***));
EXTERN_C int DLL_DECL storm_getstatus_copyreq _PROTO((char *, int, char **, char **, int *, struct storm_copyfilestatus **));
EXTERN_C int DLL_DECL storm_getstatus_getreq _PROTO((char *, int, char **, int *, struct storm_getfilestatus **));
EXTERN_C int DLL_DECL storm_getstatus_putreq _PROTO((char *, int, char **, int *, struct storm_putfilestatus **));
EXTERN_C int DLL_DECL storm_ins_reqctr _PROTO(());
EXTERN_C int DLL_DECL storm_modifyfs _PROTO((char *, char *, int));
EXTERN_C int DLL_DECL storm_modifypool _PROTO((struct storm_pool *));
EXTERN_C int DLL_DECL storm_put _PROTO((int, struct storm_putfilereq *, int, char **, char *, int, time_t, char *, int *, struct storm_putfilestatus **));
EXTERN_C int DLL_DECL storm_putdone _PROTO((char *, int, char **, int *, struct storm_filestatus **));
EXTERN_C int DLL_DECL storm_releasespace _PROTO((char *, int));
EXTERN_C int DLL_DECL storm_relfiles _PROTO((char *, int, char **, int, int *, struct storm_filestatus **));
EXTERN_C int DLL_DECL storm_replicate _PROTO((char *));
EXTERN_C int DLL_DECL storm_reservespace _PROTO((const char, const char *, u_signed64, u_signed64, time_t, char *, u_signed64 *, u_signed64 *, time_t *, char *));
EXTERN_C int DLL_DECL storm_rm _PROTO((int, char **, int *, struct storm_filestatus **));
EXTERN_C int DLL_DECL storm_rmfs _PROTO((char *, char *));
EXTERN_C int DLL_DECL storm_rmpool _PROTO((char *));
EXTERN_C int DLL_DECL storm_seterrbuf _PROTO((char *, int));
EXTERN_C int DLL_DECL storm_updatespace _PROTO((char *, u_signed64, u_signed64, time_t, u_signed64 *, u_signed64 *, time_t *));
#endif
