/*
 * $Id$
 */

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

