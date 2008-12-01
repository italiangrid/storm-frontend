/*
 * $Id$
 */

#include <sys/types.h>
#include <Cgrp.h>
#include <Cpwd.h>
#include "Cnetdb.h"
#include "storm_limits.h"
#include "storm_util.h"
#include "storm_functions.h"
#include "serrno.h"
#include "srmv2H.h"
#include "srmlogit.h"

int ns1__srmSetPermission(struct soap *soap,
                          struct ns1__srmSetPermissionRequest *req,
                          struct ns1__srmSetPermissionResponse_ *rep)
{
    struct ns1__srmSetPermissionResponse *repp;
    
    /************************ Allocate response structure *******************************/
    if ((repp = soap_malloc(soap, sizeof(struct ns1__srmSetPermissionResponse))) == NULL)
        return (SOAP_EOM);
    
    if ((repp->returnStatus = soap_malloc(soap, sizeof(struct ns1__TReturnStatus))) == NULL)
        return (SOAP_EOM);
    repp->returnStatus->explanation = "Not supported";
    repp->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;

    /* Assign the repp response structure to the output parameter rep */
    rep->srmSetPermissionResponse = repp;

    return (SOAP_OK);
}

int ns1__srmCheckPermission(struct soap *soap,
                            struct ns1__srmCheckPermissionRequest *req,
                            struct ns1__srmCheckPermissionResponse_ *rep)
{
    struct ns1__srmCheckPermissionResponse *repp;
    
    /************************ Allocate response structure *******************************/
    if ((repp = soap_malloc(soap, sizeof(struct ns1__srmCheckPermissionResponse))) == NULL)
        return (SOAP_EOM);
    repp->arrayOfPermissions = NULL;
    
    if ((repp->returnStatus = soap_malloc(soap, sizeof(struct ns1__TReturnStatus))) == NULL)
        return (SOAP_EOM);
    repp->returnStatus->explanation = "Not supported";
    repp->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;

    /* Assign the repp response structure to the output parameter rep */
    rep->srmCheckPermissionResponse = repp;

    return (SOAP_OK);
}

int ns1__srmGetPermission(struct soap *soap,
                          struct ns1__srmGetPermissionRequest *req,
                          struct ns1__srmGetPermissionResponse_ *rep)
{
    struct ns1__srmGetPermissionResponse *repp;
    
    /************************ Allocate response structure *******************************/
    if ((repp = soap_malloc(soap, sizeof(struct ns1__srmGetPermissionResponse))) == NULL)
        return (SOAP_EOM);
    repp->arrayOfPermissionReturns = NULL;
    
    if ((repp->returnStatus = soap_malloc(soap, sizeof(struct ns1__TReturnStatus))) == NULL)
        return (SOAP_EOM);
    repp->returnStatus->explanation = "Not supported";
    repp->returnStatus->statusCode = SRM_USCORENOT_USCORESUPPORTED;

    /* Assign the repp response structure to the output parameter rep */
    rep->srmGetPermissionResponse = repp;

    return (SOAP_OK);
}

//#if defined(GSI_PLUGINS)
//static int cvt_entries(struct ns1__srmSetPermissionRequest *req, struct storm_acl *acl)
//{
//    int i;
//    int nentries = 0;
//
//    if (req->ownerPermission) {
//        acl->a_type = STORM_ACL_USER_OBJ;
//        acl->a_id = 0;
//        acl->a_perm = req->ownerPermission->mode;
//        acl++;
//        nentries++;
//    }
//    if (req->userPermission) {
//        for (i = 0; i < req->userPermission->__sizeuserPermissionArray; i++) {
//            acl->a_type = STORM_ACL_USER;
//            if ((acl->a_id = cvt_user (req->userPermission->userPermissionArray[i]->userID->value)) < 0)
//                return (-1);
//            acl->a_perm = req->userPermission->userPermissionArray[i]->mode;
//            acl++;
//            nentries++;
//        }
//    }
//    if (req->groupPermission) {
//        for (i = 0; i < req->groupPermission->__sizegroupPermissionArray; i++) {
//            acl->a_type = STORM_ACL_GROUP;
//            if ((acl->a_id = cvt_group (req->groupPermission->groupPermissionArray[i]->groupID->value)) < 0)
//                return (-1);
//            acl->a_perm = req->groupPermission->groupPermissionArray[i]->mode;
//            acl++;
//            nentries++;
//        }
//    }
//    if (req->otherPermission) {
//        acl->a_type = STORM_ACL_OTHER;
//        acl->a_id = 0;
//        acl->a_perm = req->otherPermission->mode;
//        acl++;
//        nentries++;
//    }
//    return (nentries);
//    return(0);
//}
//
//cvt_group (char *p)
//{
//    struct group *gr;
//
//    if ((gr = Cgetgrnam (p)) == NULL) {
//        return (-1);
//    }
//    return (gr->gr_gid);
//}
//
//cvt_user (char *p)
//{
//    struct passwd *pwd;
//    char *q;
//
//    if (strchr (p, ':')) {  /* assume that it is a DN */
//        if (globus_gss_assist_gridmap (p, &q))
//            return (-1);
//        pwd = Cgetpwnam (q);
//        free (q);
//        if (pwd == NULL)
//            return (-1);
//    } else {
//        if ((pwd = Cgetpwnam (p)) == NULL)
//            return (-1);
//    }
//    return (pwd->pw_uid);
//}
//#endif /* #if defined(GSI_PLUGINS) */
