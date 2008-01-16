/*
 * $Id$
 */

#include <errno.h>
#include <sys/types.h>
#include "storm_functions.h"
#include "srm_server.h"
#include "storm_constants.h"
#include "serrno.h"
#include "srmv2Stub.h"

storm_getonereqsummary (thip, r_token, r_type, nbreqfiles, nb_queued, nb_progress)
struct srm_srv_thread_info *thip;
char *r_token;
char *r_type;
int *nbreqfiles;
int *nb_queued;
int *nb_progress;
{
	int bol;
	int c;
	struct storm_copy_filereq cpr_entry;
	DBLISTPTR dblistptr;
	struct storm_req storm_req;
	struct storm_get_filereq gfr_entry;
	struct storm_put_filereq pfr_entry;

	if (storm_get_pending_req_by_token (&thip->dbfd, r_token, &storm_req, 0, NULL) < 0 &&
	    storm_get_req_by_token (&thip->dbfd, r_token, &storm_req, 0, NULL) < 0)
		return (-1);
	bol = 1;
	*r_type = storm_req.r_type;
	*nbreqfiles = storm_req.nbreqfiles;
	*nb_queued = 0;
	*nb_progress = 0;
	switch (storm_req.r_type) {
	case 'C':
		while ((c = storm_list_cpr_entry (&thip->dbfd, bol, r_token,
		    &cpr_entry, 0, NULL, 0, &dblistptr)) == 0) {
			bol = 0;
			if (cpr_entry.status == SRM_USCOREREQUEST_USCOREQUEUED)
				(*nb_queued)++;
			else if (cpr_entry.status == SRM_USCOREREQUEST_USCOREINPROGRESS)
				(*nb_progress)++;
		}
		(void) storm_list_cpr_entry (&thip->dbfd, bol, r_token,
		    &cpr_entry, 0, NULL, 1, &dblistptr);
		break;
	case 'G':
		while ((c = storm_list_gfr_entry (&thip->dbfd, bol, r_token,
		    &gfr_entry, 0, NULL, 0, &dblistptr)) == 0) {
			bol = 0;
			if (gfr_entry.status == SRM_USCOREREQUEST_USCOREQUEUED)
				(*nb_queued)++;
			else if (gfr_entry.status == SRM_USCOREREQUEST_USCOREINPROGRESS)
				(*nb_progress)++;
		}
		(void) storm_list_gfr_entry (&thip->dbfd, bol, r_token,
		    &gfr_entry, 0, NULL, 1, &dblistptr);
		break;
	case 'P':
		while ((c = storm_list_pfr_entry (&thip->dbfd, bol, r_token,
		    &pfr_entry, 0, NULL, 0, &dblistptr)) == 0) {
			bol = 0;
			if (pfr_entry.status == SRM_USCOREREQUEST_USCOREQUEUED)
				(*nb_queued)++;
			else if (pfr_entry.status == SRM_USCOREREQUEST_USCOREINPROGRESS)
				(*nb_progress)++;
		}
		(void) storm_list_pfr_entry (&thip->dbfd, bol, r_token,
		    &pfr_entry, 0, NULL, 1, &dblistptr);
		break;
	}
	return (0);
}

storm_relonefile (thip, r_token, r_type, surl, status)
struct srm_srv_thread_info *thip;
char *r_token;
char r_type;
char *surl;
int *status;
{
	struct storm_copy_filereq cpr_entry;
	struct storm_get_filereq gfr_entry;
	char *pfn;
	struct storm_put_filereq pfr_entry;
	storm_dbrec_addr rec_addr;
	time_t t1, t2;

	switch (r_type) {
	case 'C':
		if (storm_get_cpr_by_surl (&thip->dbfd, r_token, surl, &cpr_entry,
		    1, &rec_addr) < 0) {
			*status = serrno2statuscode (serrno);
			return (-1);
		}
		cpr_entry.lifetime = 0;
		if (storm_update_cpr_entry (&thip->dbfd, &rec_addr, &cpr_entry) < 0) {
			*status = serrno2statuscode(serrno);
			return (-1);
		}
		break;
	case 'G':
		if (storm_get_gfr_by_surl (&thip->dbfd, r_token, surl, &gfr_entry,
		    1, &rec_addr) < 0) {
			*status = serrno2statuscode(serrno);
			return (-1);
		}
		gfr_entry.lifetime = 0;
		if (storm_update_gfr_entry (&thip->dbfd, &rec_addr, &gfr_entry) < 0) {
			*status = serrno2statuscode(serrno);
			return (-1);
		}
		pfn = gfr_entry.pfn;
		break;
	case 'P':
		if (storm_get_pfr_by_surl (&thip->dbfd, r_token, surl, &pfr_entry,
		    1, &rec_addr) < 0) {
			*status = serrno2statuscode(serrno);
			return (-1);
		}
		pfr_entry.lifetime = 0;
		if (storm_update_pfr_entry (&thip->dbfd, &rec_addr, &pfr_entry) < 0) {
			*status = serrno2statuscode(serrno);
			return (-1);
		}
		pfn = pfr_entry.pfn;
		break;
	}
	if (r_type == 'C')
		return (0);
	if (storm_get_max_get_lifetime (&thip->dbfd, pfn, &t1) < 0)
		t1 = 0;
	if (storm_get_max_get_lifetime (&thip->dbfd, pfn, &t2) < 0)
		t2 = 0;
/******* FLAVIA TO DO *******/
/*
	if (Cns_setptime (pfn, t1 > t2 ? t1 : t2) < 0) {
		*status = serrno2statuscode(serrno);
		return (-1);
	}
*/
/******* FLAVIA TO DO *******/
	return (0);
}
