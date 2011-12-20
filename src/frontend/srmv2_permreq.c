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

#include "srmv2H.h"
#include "srmlogit.h"

int ns1__srmSetPermission_impl(struct soap *soap,
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

int ns1__srmCheckPermission_impl(struct soap *soap,
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

int ns1__srmGetPermission_impl(struct soap *soap,
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

