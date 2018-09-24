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

/**
 * \file xml_dencode.c
 * 
 * This file contains the callerNametions implementing, for each output type of srmv2.2, the functions
 * that dencode a specified type from the xml structure to the corresponding WSDL type.
 */
 
#include "xmlrpc_decode.hpp"
#include <stdio.h>
#include <cgsi_plugin.h>


/***************************************************************************************************************/
/***************************************************************************************************************/
/***************************************************************************************************************/

/**
 * The decode_lifetimeValue() decodes parameter representing life time from the xml structure received by the BE.
 * Actually life time values are int values in the wsdl of SRM v2.2. Probably this is a mistake therefore
 * we use ULONG64 of the BE side (like was for SRM v2.1).
 * @param callerName The name of the caller callerNametion (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc callerNametions.
 * @param soap The gSOAP structure
 * @param lifetimeVal The returned arrayOfFileStatuses (memory is allocated with soap_malloc()).
 * @param fieldName The name of the xml field to search for.
 * @param xmlStruct The xml structure to parse.
 */
int decode_lifetimeValue(const char *callerName,
                         xmlrpc_env *env_addr,
                         struct soap *soap,
                         int **lifetimeVal,
                         char *fieldName,
                         xmlrpc_value *xmlStruct)
{
    int error;
    ULONG64 *longValue;
    
    error = decode_ULONG64(callerName, env_addr, soap, &longValue, fieldName, xmlStruct);
    if (longValue == NULL) *lifetimeVal = NULL;
    else {
        *lifetimeVal = static_cast<int*>(soap_malloc(soap, sizeof(int)));
        **lifetimeVal = (int) *longValue;
    }
    
    return(error);
}

/**
 * The decode_ArrayOfTSURLReturnStatus() decodes an ArrayOfTSURLReturnStatus from the xml structure
 * received by the BE.
 * @param callerName The name of the caller callerNametion (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc callerNametions.
 * @param soap The gSOAP structure
 * @param arrayOfFileStatuses The returned arrayOfFileStatuses (memory is allocated with soap_malloc()).
 * @param fieldName The name of the xml field to search for.
 * @param xmlStruct The xml structure to parse.
 */
int decode_ArrayOfTSURLReturnStatus(const char *callerName,
                                    xmlrpc_env *env_addr,
                                    struct soap *soap,
                                    struct ns1__ArrayOfTSURLReturnStatus **arrayOfFileStatuses,
                                    char *fieldName,
                                    xmlrpc_value *xmlStruct)
{
    struct ns1__TSURLReturnStatus **statusArray;
    int i, arraySize, error;
    xmlrpc_value *xml_arrayOfFileStatuses, *xml_arrayItem;
    
    *arrayOfFileStatuses = NULL;
    
    xmlrpc_struct_find_value(env_addr, xmlStruct, fieldName, &xml_arrayOfFileStatuses);
    if ((env_addr->fault_occurred) || (NULL == xml_arrayOfFileStatuses)) {
        srmlogit(STORM_LOG_WARNING, callerName, "%s not specified by BE.\n", fieldName);
        return(DECODE_ERR_NOT_FOUND);
    }
    
    arraySize = -1;
    arraySize = xmlrpc_array_size(env_addr, xml_arrayOfFileStatuses);
    if ( (env_addr->fault_occurred) || (arraySize<1) ) {
        srmlogit(STORM_LOG_WARNING, callerName, "xml_arrayOfFileStatuses is empty\n");
        return(DECODE_ERR_NOT_FOUND);
    }
    
    /************************ Allocate memory for the response structure ******************************/
    *arrayOfFileStatuses = static_cast<ns1__ArrayOfTSURLReturnStatus*>(soap_malloc(soap, sizeof(struct ns1__ArrayOfTSURLReturnStatus)));
    if (NULL == *arrayOfFileStatuses) {
        srmlogit(STORM_LOG_WARNING, callerName, "Memory allocation error for arrayOfFileStatuses\n");
        xmlrpc_DECREF(xml_arrayOfFileStatuses);
        return(DECODE_ERR_SOAP_MEMORY_ERROR);
    }
    
    statusArray = static_cast<ns1__TSURLReturnStatus**>(soap_malloc(soap, arraySize * sizeof(struct ns1__TSURLReturnStatus *)));
    if (NULL == statusArray) {
        srmlogit(STORM_LOG_WARNING, callerName, "Memory allocation error for arrayOfFileStatuses->statusArray\n");
        *arrayOfFileStatuses = NULL;
        xmlrpc_DECREF(xml_arrayOfFileStatuses);
        return(DECODE_ERR_SOAP_MEMORY_ERROR);
    }
    
    for (i=0; i<arraySize; i++) {
        statusArray[i] = static_cast<ns1__TSURLReturnStatus*>(soap_malloc(soap, sizeof(struct ns1__TSURLReturnStatus)));
        if (NULL == statusArray[i]) {
            srmlogit(STORM_LOG_WARNING, callerName, "Memory allocation error for arrayOfFileStatuses->statusArray[%d]\n", i);
            *arrayOfFileStatuses = NULL;
            xmlrpc_DECREF(xml_arrayOfFileStatuses);
            return(DECODE_ERR_SOAP_MEMORY_ERROR);
        }
    }
    
    (*arrayOfFileStatuses)->__sizestatusArray = arraySize;
    (*arrayOfFileStatuses)->statusArray = statusArray;
    
    /********************************************************************************************************/
    /*************************** Decode each element of arrayOfFileStatuses *********************************/
    for (i=0; i<arraySize; i++) {
        xmlrpc_array_read_item(env_addr, xml_arrayOfFileStatuses, i, &xml_arrayItem);
        if (env_addr->fault_occurred) {
            srmlogit(STORM_LOG_ERROR, callerName, "Error reading item (arrayOfFileStatuses->statusArray[%d])\n", i);
            statusArray[i] = NULL;
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
            xmlrpc_DECREF(xml_arrayItem);
            continue;   // Go to the next element
        }
        
        /** MANDATORY ******* (1) Decode surl (in char *) **************************************************/
        error = decode_string(callerName, env_addr, soap, &(statusArray[i]->surl), "surl", xml_arrayItem);
        if (error != 0) {
            srmlogit(STORM_LOG_ERROR, callerName, "Error reading (arrayOfFileStatuses->statusArray[%d]->surl)\n", i);
            statusArray[i] = NULL;
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
            xmlrpc_DECREF(xml_arrayItem);
            continue;   // Go to the next element
        }
        /** MANDATORY ******* (2) Decode status (in struct ns1__TReturnStatus *) ***************************/
        error = decode_TReturnStatus(callerName, env_addr, soap, &(statusArray[i]->status), SRM_PARAM_status, xml_arrayItem);
        if (error != 0) {
            srmlogit(STORM_LOG_ERROR, callerName, "Error reading (arrayOfFileStatuses->statusArray[%d]->status)\n", i);
            statusArray[i] = NULL;
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
            xmlrpc_DECREF(xml_arrayItem);
            continue;   // Go to the next element
        }
        
        xmlrpc_DECREF(xml_arrayItem);
        
    } // for (i=0; i<arraySize; i++)
    
    xmlrpc_DECREF(xml_arrayOfFileStatuses);
    
    return(0);
}

/**
 * The decode_ArrayOfTExtraInfo() decodes an ArrayOfTExtraInfo from the xml structure
 * received by the BE.
 * @param callerName The name of the caller callerNametion (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc callerNametions.
 * @param soap The gSOAP structure
 * @param arrayOfTExtraInfo The returned arrayOfTExtraInfo (memory is allocated with soap_malloc()).
 * @param fieldName The name of the xml field to search for.
 * @param xmlStruct The xml structure to parse.
 */
int decode_ArrayOfTExtraInfo(const char *callerName,
                             xmlrpc_env *env_addr,
                             struct soap *soap,
                             struct ns1__ArrayOfTExtraInfo **arrayOfTExtraInfo,
                             char *fieldName,
                             xmlrpc_value *xmlStruct)
{
    struct ns1__TExtraInfo **extraInfoArray;
    int i, arraySize, error;
    xmlrpc_value *xml_arrayOfTExtraInfo, *xml_arrayItem;
    
    *arrayOfTExtraInfo = NULL;
    
    xmlrpc_struct_find_value(env_addr, xmlStruct, fieldName, &xml_arrayOfTExtraInfo);
    if ((env_addr->fault_occurred) || (NULL == xml_arrayOfTExtraInfo)) {
        srmlogit(STORM_LOG_WARNING, callerName, "%s not specified by BE.\n", fieldName);
        return(DECODE_ERR_NOT_FOUND);
    }
    
    arraySize = -1;
    arraySize = xmlrpc_array_size(env_addr, xml_arrayOfTExtraInfo);
    if ( (env_addr->fault_occurred) || (arraySize<1) ) {
        srmlogit(STORM_LOG_WARNING, callerName, "xml_arrayOfTExtraInfo is empty\n");
        return(DECODE_ERR_NOT_FOUND);
    }
    
    /************************ Allocate memory for the response structure ******************************/
    *arrayOfTExtraInfo = static_cast<ns1__ArrayOfTExtraInfo*>(soap_malloc(soap, sizeof(struct ns1__ArrayOfTExtraInfo)));
    if (NULL == *arrayOfTExtraInfo) {
        srmlogit(STORM_LOG_WARNING, callerName, "Memory allocation error for extraInfoArray\n");
        xmlrpc_DECREF(xml_arrayOfTExtraInfo);
        return(DECODE_ERR_SOAP_MEMORY_ERROR);
    }
    
    extraInfoArray = static_cast<ns1__TExtraInfo**>(soap_malloc(soap, arraySize * sizeof(struct ns1__TExtraInfo *)));
    if (NULL == extraInfoArray) {
        srmlogit(STORM_LOG_WARNING, callerName, "Memory allocation error for arrayOfTExtraInfo->extraInfoArray\n");
        *arrayOfTExtraInfo = NULL;
        xmlrpc_DECREF(xml_arrayOfTExtraInfo);
        return(DECODE_ERR_SOAP_MEMORY_ERROR);
    }
    
    for (i=0; i<arraySize; i++) {
        extraInfoArray[i] = static_cast<ns1__TExtraInfo*>(soap_malloc(soap, sizeof(struct ns1__TExtraInfo)));
        if (NULL == extraInfoArray[i]) {
            srmlogit(STORM_LOG_WARNING, callerName, "Memory allocation error for arrayOfTExtraInfo->extraInfoArray[%d]\n", i);
            *arrayOfTExtraInfo = NULL;
            xmlrpc_DECREF(xml_arrayOfTExtraInfo);
            return(DECODE_ERR_SOAP_MEMORY_ERROR);
        }
    }
    
    (*arrayOfTExtraInfo)->__sizeextraInfoArray = arraySize;
    (*arrayOfTExtraInfo)->extraInfoArray = extraInfoArray;
    
    /********************************************************************************************************/
    /*************************** Decode each element of arrayOfFileStatuses *********************************/
    for (i=0; i<arraySize; i++) {
        xmlrpc_array_read_item(env_addr, xml_arrayOfTExtraInfo, i, &xml_arrayItem);
        if (env_addr->fault_occurred) {
            srmlogit(STORM_LOG_ERROR, callerName, "Error reading item (arrayOfTExtraInfo->extraInfoArray[%d])\n", i);
            extraInfoArray[i] = NULL;
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
            xmlrpc_DECREF(xml_arrayItem);
            continue;   // Go to the next element
        }
        
        /** OPTIONAL ******* (1) Decode key (in char *) ***************************/
        error = decode_string(callerName, env_addr, soap, &(extraInfoArray[i]->key), "key", xml_arrayItem);
        if (error != 0) {
            if (error != DECODE_ERR_NOT_FOUND)
                srmlogit(STORM_LOG_ERROR, callerName, "Error reading (arrayOfTExtraInfo->extraInfoArray[%d]->key)\n", i);
            else
                srmlogit(STORM_LOG_WARNING, callerName, "Not found (arrayOfTExtraInfo->extraInfoArray[%d]->key)\n", i);
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        /** OPTIONAL ******* (2) Decode value (in char *) ***************************/
        error = decode_string(callerName, env_addr, soap, &(extraInfoArray[i]->value), "value", xml_arrayItem);
        if (error != 0) {
            if (error != DECODE_ERR_NOT_FOUND)
                srmlogit(STORM_LOG_ERROR, callerName, "Error reading (arrayOfTExtraInfo->extraInfoArray[%d]->value)\n", i);
            else
                srmlogit(STORM_LOG_WARNING, callerName, "Not found (arrayOfTExtraInfo->extraInfoArray[%d]->value)\n", i);
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        xmlrpc_DECREF(xml_arrayItem);
        
    } // for (i=0; i<arraySize; i++)
    
    xmlrpc_DECREF(xml_arrayOfTExtraInfo);
    
    return(0);
}

/**
 * The decode_ArrayOfTSURLLifetimeReturnStatus() decodes an ArrayOfTSURLLifetimeReturnStatus
 * from the xml structure received by the BE.
 * @param callerName The name of the caller callerNametion (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc.
 * @param soap The gSOAP structure
 * @param arrayOfFileStatuses The returned arrayOfFileStatuses (memory is allocated with soap_malloc()).
 * @param fieldName The name of the xml field to search for.
 * @param xmlStruct The xml structure to parse.
 */
int decode_ArrayOfTSURLLifetimeReturnStatus(const char *callerName,
                                            xmlrpc_env *env_addr,
                                            struct soap *soap,
                                            struct ns1__ArrayOfTSURLLifetimeReturnStatus **arrayOfFileStatuses,
                                            char *fieldName,
                                            xmlrpc_value *xmlStruct)
{
    struct ns1__TSURLLifetimeReturnStatus **statusArray;
    int i, arraySize, error;
    xmlrpc_value *xml_arrayOfFileStatuses, *xml_arrayItem;
    
    *arrayOfFileStatuses = NULL;
    
    xmlrpc_struct_find_value(env_addr, xmlStruct, fieldName, &xml_arrayOfFileStatuses);
    if ((env_addr->fault_occurred) || (NULL == xml_arrayOfFileStatuses)) {
        srmlogit(STORM_LOG_WARNING, callerName, "%s not specified by BE.\n", fieldName);
        return(DECODE_ERR_NOT_FOUND);
    }
    
    arraySize = -1;
    arraySize = xmlrpc_array_size(env_addr, xml_arrayOfFileStatuses);
    if ( (env_addr->fault_occurred) || (arraySize<1) ) {
        srmlogit(STORM_LOG_WARNING, callerName, "xml_arrayOfFileStatuses is empty\n");
        return(DECODE_ERR_NOT_FOUND);
    }
    
    /************************ Allocate memory for the response structure ******************************/
    *arrayOfFileStatuses = static_cast<ns1__ArrayOfTSURLLifetimeReturnStatus*>(soap_malloc(soap, sizeof(struct ns1__ArrayOfTSURLLifetimeReturnStatus)));
    if (NULL == *arrayOfFileStatuses) {
        srmlogit(STORM_LOG_WARNING, callerName, "Memory allocation error for arrayOfFileStatuses\n");
        xmlrpc_DECREF(xml_arrayOfFileStatuses);
        return(DECODE_ERR_SOAP_MEMORY_ERROR);
    }
    
    statusArray = static_cast<ns1__TSURLLifetimeReturnStatus**>(soap_malloc(soap, arraySize * sizeof(struct ns1__TSURLLifetimeReturnStatus *)));
    if (NULL == statusArray) {
        srmlogit(STORM_LOG_WARNING, callerName, "Memory allocation error for arrayOfFileStatuses->statusArray\n");
        *arrayOfFileStatuses = NULL;
        xmlrpc_DECREF(xml_arrayOfFileStatuses);
        return(DECODE_ERR_SOAP_MEMORY_ERROR);
    }
    
    for (i=0; i<arraySize; i++) {
        statusArray[i] = static_cast<ns1__TSURLLifetimeReturnStatus*>(soap_malloc(soap, sizeof(struct ns1__TSURLLifetimeReturnStatus)));
        if (NULL == statusArray[i]) {
            srmlogit(STORM_LOG_WARNING, callerName, "Memory allocation error for arrayOfFileStatuses->statusArray[%d]\n", i);
            *arrayOfFileStatuses = NULL;
            xmlrpc_DECREF(xml_arrayOfFileStatuses);
            return(DECODE_ERR_SOAP_MEMORY_ERROR);
        }
    }
    
    (*arrayOfFileStatuses)->__sizestatusArray = arraySize;
    (*arrayOfFileStatuses)->statusArray = statusArray;
    
    /********************************************************************************************************/
    /*************************** Decode each element of arrayOfFileStatuses *********************************/
    for (i=0; i<arraySize; i++) {
        xmlrpc_array_read_item(env_addr, xml_arrayOfFileStatuses, i, &xml_arrayItem);
        if (env_addr->fault_occurred) {
            srmlogit(STORM_LOG_ERROR, callerName, "Error reading item (arrayOfFileStatuses->statusArray[%d])\n", i);
            statusArray[i] = NULL;
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
            xmlrpc_DECREF(xml_arrayItem);
            continue;   // Go to the next element
        }
        /** MANDATORY ******* (1) Decode surl (in char *) **************************************************/
        error = decode_string(callerName, env_addr, soap, &(statusArray[i]->surl), "surl", xml_arrayItem);
        if (error != 0) {
            srmlogit(STORM_LOG_ERROR, callerName, "Error reading (arrayOfFileStatuses->statusArray[%d]->surl)\n", i);
            statusArray[i] = NULL;
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
            xmlrpc_DECREF(xml_arrayItem);
            continue;   // Go to the next element
        }
        /** MANDATORY ******* (2) Decode status (in struct ns1__TReturnStatus *) ***************************/
        error = decode_TReturnStatus(callerName, env_addr, soap, &(statusArray[i]->status), SRM_PARAM_status, xml_arrayItem);
        if (error != 0) {
            srmlogit(STORM_LOG_ERROR, callerName, "Error reading (arrayOfFileStatuses->statusArray[%d]->status)\n", i);
            statusArray[i] = NULL;
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
            xmlrpc_DECREF(xml_arrayItem);
            continue;   // Go to the next element
        }
        /** OPTIONAL ******* (3) Decode fileLifetime (in int *) ***************************/
        error = decode_lifetimeValue(callerName, env_addr, soap, &(statusArray[i]->fileLifetime), SRM_PARAM_fileLifetime, xml_arrayItem);
        if (error != 0) {
            if (error != DECODE_ERR_NOT_FOUND)
                srmlogit(STORM_LOG_ERROR, callerName, "Error reading (arrayOfFileStatuses->statusArray[%d]->fileLifetime)\n", i);
            else
                srmlogit(STORM_LOG_WARNING, callerName, "Not found (arrayOfFileStatuses->statusArray[%d]->fileLifetime)\n", i);
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        /** OPTIONAL ******* (4) Decode pinLifetime (in int *) ***************************/
        error = decode_lifetimeValue(callerName, env_addr, soap, &(statusArray[i]->pinLifetime), SRM_PARAM_pinLifetime, xml_arrayItem);
        if (error != 0) {
            if (error != DECODE_ERR_NOT_FOUND)
                srmlogit(STORM_LOG_ERROR, callerName, "Error reading (arrayOfFileStatuses->statusArray[%d]->pinLifetime)\n", i);
            else
                srmlogit(STORM_LOG_WARNING, callerName, "Not found (arrayOfFileStatuses->statusArray[%d]->pinLifetime)\n", i);
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        xmlrpc_DECREF(xml_arrayItem);
        
    } // for (i=0; i<arraySize; i++)
    
    xmlrpc_DECREF(xml_arrayOfFileStatuses);
    
    return(0);
}

/**
 * The decode_ArrayOfTMetaDataSpace() callerNametion decodes a ArrayOfTMetaDataSpace from the xml structure
 * received by the BE.
 * @param callerName The name of the caller callerNametion (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc callerNametions.
 * @param soap The gSOAP structure
 * @param arrayOfSpaceDetails The returned arrayOfSpaceDetails (memory is allocated with soap_malloc()).
 * @param fieldName The name of the xml field to search for.
 * @param xmlStruct The xml structure to parse.
 */
int decode_ArrayOfTMetaDataSpace(const char *callerName,
                                 xmlrpc_env *env_addr,
                                 struct soap *soap,
                                 struct ns1__ArrayOfTMetaDataSpace **arrayOfSpaceDetails,
                                 char *fieldName,
                                 xmlrpc_value *xmlStruct)
{
    struct ns1__TMetaDataSpace **spaceDataArray;
    int i, arraySize, error;
    xmlrpc_value *xml_arrayOfSpaceDetails, *xml_arrayItem;
    
    *arrayOfSpaceDetails = NULL;
    
    xmlrpc_struct_find_value(env_addr, xmlStruct, fieldName, &xml_arrayOfSpaceDetails);
    if ((env_addr->fault_occurred) || (NULL == xml_arrayOfSpaceDetails)) {
        srmlogit(STORM_LOG_WARNING, callerName, "%s not specified by BE.\n", fieldName);
        return(DECODE_ERR_NOT_FOUND);
    }
    
    arraySize = -1;
    arraySize = xmlrpc_array_size(env_addr, xml_arrayOfSpaceDetails);
    if ( (env_addr->fault_occurred) || (arraySize<1) ) {
        srmlogit(STORM_LOG_WARNING, callerName, "%s is empty\n", fieldName);
        return(DECODE_ERR_NOT_FOUND);
    }
    
    /************************ Allocate memory for the response structure ******************************/
    *arrayOfSpaceDetails = static_cast<ns1__ArrayOfTMetaDataSpace*>(soap_malloc(soap, sizeof(struct ns1__ArrayOfTMetaDataSpace)));
    if (NULL == *arrayOfSpaceDetails) {
        srmlogit(STORM_LOG_WARNING, callerName, "Memory allocation error for arrayOfSpaceDetails\n");
        xmlrpc_DECREF(xml_arrayOfSpaceDetails);
        return(DECODE_ERR_SOAP_MEMORY_ERROR);
    }
    
    spaceDataArray = static_cast<ns1__TMetaDataSpace**>(soap_malloc(soap, arraySize * sizeof(struct ns1__TMetaDataSpace *)));
    if (NULL == spaceDataArray) {
        srmlogit(STORM_LOG_WARNING, callerName, "Memory allocation error for arrayOfSpaceDetails->spaceDataArray\n");
        *arrayOfSpaceDetails = NULL;
        xmlrpc_DECREF(xml_arrayOfSpaceDetails);
        return(DECODE_ERR_SOAP_MEMORY_ERROR);
    }
    
    for (i=0; i<arraySize; i++) {
        spaceDataArray[i] = static_cast<ns1__TMetaDataSpace*>(soap_malloc(soap, sizeof(struct ns1__TMetaDataSpace)));
        if (NULL == spaceDataArray[i]) {
            srmlogit(STORM_LOG_WARNING, callerName, "Memory allocation error for arrayOfSpaceDetails->spaceDataArray[%d]\n", i);
            *arrayOfSpaceDetails = NULL;
            xmlrpc_DECREF(xml_arrayOfSpaceDetails);
            return(DECODE_ERR_SOAP_MEMORY_ERROR);
        }
    }
    
    (*arrayOfSpaceDetails)->__sizespaceDataArray = arraySize;
    (*arrayOfSpaceDetails)->spaceDataArray = spaceDataArray;
    
    /********************************************************************************************************/
    /*************************** Decode each element of arrayOfSpaceDetails *********************************/
    for (i=0; i<arraySize; i++) {
        xmlrpc_array_read_item(env_addr, xml_arrayOfSpaceDetails, i, &xml_arrayItem);
        if (env_addr->fault_occurred) {
            srmlogit(STORM_LOG_ERROR, callerName, "Error reading item (arrayOfSpaceDetails->spaceDataArray[%d])\n", i);
            spaceDataArray[i] = NULL;
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
            xmlrpc_DECREF(xml_arrayItem);
            continue;   // Go to the next element
        }
        
        /** MANDATORY ******* (1) Decode spaceToken (in char *) *********************************************/
        error = decode_string(callerName, env_addr, soap, &(spaceDataArray[i]->spaceToken), SRM_PARAM_spaceToken, xml_arrayItem);
        if (error != 0) {
            srmlogit(STORM_LOG_ERROR, callerName, "Error reading (arrayOfSpaceDetails->spaceDataArray[%d]): spaceToken\n", i);
            spaceDataArray[i] = NULL;
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
            xmlrpc_DECREF(xml_arrayItem);
            continue;   // Go to the next element
        }
        /** OPTIONAL ******** (2) Decode status (in struct ns1__TReturnStatus *) ******************************/
        error = decode_TReturnStatus(callerName, env_addr, soap, &(spaceDataArray[i]->status), SRM_PARAM_status, xml_arrayItem);
        if (error != 0) {
            if (error != DECODE_ERR_NOT_FOUND) {
            	srmlogit(STORM_LOG_ERROR, callerName, "Error decoding status\n");
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfSpaceDetails);
                return(error);
            }
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        /** OPTIONAL ******** (3) Decode retentionPolicyInfo (in struct ns1__TRetentionPolicyInfo *) ***********/
        error = decode_TRetentionPolicyInfo(callerName, env_addr, soap, &(spaceDataArray[i]->retentionPolicyInfo), SRM_PARAM_retentionPolicyInfo, xml_arrayItem);
        if (error != 0) {
            if (error != DECODE_ERR_NOT_FOUND) {
            	srmlogit(STORM_LOG_ERROR, callerName, "Error decoding retentionPolicyInfo\n");
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfSpaceDetails);
                return(error);
            }
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        /** OPTIONAL ******** (4) Decode owner (in char *) *****************************************************/
        error = decode_string(callerName, env_addr, soap, &(spaceDataArray[i]->owner), SRM_PARAM_owner, xml_arrayItem);
        if (error != 0) {
            if (error != DECODE_ERR_NOT_FOUND) {
            	srmlogit(STORM_LOG_ERROR, callerName, "Error decoding owner\n");
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfSpaceDetails);
                return(error);
            }
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        /** OPTIONAL ******** (5) Decode totalSize (in ULONG64 *) **********************************************/
        error = decode_ULONG64(callerName, env_addr, soap, &(spaceDataArray[i]->totalSize), SRM_PARAM_totalSize, xml_arrayItem);
        if (error != 0) {
            if (error != DECODE_ERR_NOT_FOUND) {
            	srmlogit(STORM_LOG_ERROR, callerName, "Error decoding totalSize\n");
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfSpaceDetails);
                return(error);
            }
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        /** OPTIONAL ******** (6) Decode guaranteedSize (in ULONG64 *) **********************************************/
        error = decode_ULONG64(callerName, env_addr, soap, &(spaceDataArray[i]->guaranteedSize), SRM_PARAM_guaranteedSize, xml_arrayItem);
        if (error != 0) {
            if (error != DECODE_ERR_NOT_FOUND) {
            	srmlogit(STORM_LOG_ERROR, callerName, "Error decoding guaranteedSize\n");
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfSpaceDetails);
                return(error);
            }
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        /** OPTIONAL ******** (7) Decode unusedSize (in ULONG64 *) **********************************************/
        error = decode_ULONG64(callerName, env_addr, soap, &(spaceDataArray[i]->unusedSize), SRM_PARAM_unusedSize, xml_arrayItem);
        if (error != 0) {
            if (error != DECODE_ERR_NOT_FOUND) {
            	srmlogit(STORM_LOG_ERROR, callerName, "Error decoding unusedSize\n");
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfSpaceDetails);
                return(error);
            }
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        /** OPTIONAL ******** (8) Decode lifetimeAssigned (in int *) **********************************************/
        error = decode_lifetimeValue(callerName, env_addr, soap, &(spaceDataArray[i]->lifetimeAssigned), SRM_PARAM_lifetimeAssigned, xml_arrayItem);
        if (error != 0) {
            if (error != DECODE_ERR_NOT_FOUND) {
            	srmlogit(STORM_LOG_ERROR, callerName, "Error decoding lifetimeAssigned\n");
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfSpaceDetails);
                return(error);
            }
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        /** OPTIONAL ******** (9) Decode lifetimeLeft (in int *) **********************************************/
        error = decode_lifetimeValue(callerName, env_addr, soap, &(spaceDataArray[i]->lifetimeLeft), SRM_PARAM_lifetimeLeft, xml_arrayItem);
        if (error != 0) {
            if (error != DECODE_ERR_NOT_FOUND) {
            	srmlogit(STORM_LOG_ERROR, callerName, "Error decoding lifetimeLeft\n");
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfSpaceDetails);
                return(error);
            }
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        xmlrpc_DECREF(xml_arrayItem);
    } // end for (i=0; i<arraySize; i++)
    
    xmlrpc_DECREF(xml_arrayOfSpaceDetails);
    
    return(0);
}

/**
 * The decode_TRetentionPolicyInfo() callerNametion decodes a TRetentionPolicyInfo from the xml structure received by the BE.
 * @param callerName The name of the caller callerNametion (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc callerNametions.
 * @param soap The gSOAP structure
 * @param retentionPolicyInfo The TRetentionPolicyInfo structure returned.
 * @param fieldName The name of the xml field to search for.
 * @param xmlStruct The xml structure to parse.
 */
int decode_TRetentionPolicyInfo(const char *callerName,
                                xmlrpc_env *env_addr,
                                struct soap *soap,
                                struct ns1__TRetentionPolicyInfo **retentionPolicyInfo,
                                char *fieldName,
                                xmlrpc_value *xmlStruct)
{
    int retentionPolicy, accessLatency;
    xmlrpc_value *retentionPolInfo;
    
    *retentionPolicyInfo = NULL;
    
    xmlrpc_struct_find_value(env_addr, xmlStruct, fieldName, &retentionPolInfo);
    if ((env_addr->fault_occurred) || (retentionPolInfo == NULL)) {
        srmlogit(STORM_LOG_DEBUG, callerName, "%s not specified by BE.\n", fieldName);
        return(DECODE_ERR_NOT_FOUND);
    }
    
    xmlrpc_decompose_value(env_addr, retentionPolInfo, "{s:i,s:i,*}", "retentionPolicy", &retentionPolicy,
                           "accessLatency", &accessLatency);
    xmlrpc_DECREF(retentionPolInfo);
    if (env_addr->fault_occurred) {
        srmlogit(STORM_LOG_WARNING, callerName, "Error decoding retentionPolicyInfo (unable to parse the BE response)\n");
        return(DECODE_ERR_DECODING_ERROR);
    }
    
    *retentionPolicyInfo = static_cast<ns1__TRetentionPolicyInfo*>(soap_malloc(soap, sizeof(struct ns1__TRetentionPolicyInfo)));
    if (NULL == *retentionPolicyInfo) {
        srmlogit(STORM_LOG_WARNING, callerName, "Decoding %s: memory allocation error\n", fieldName);
        return(DECODE_ERR_SOAP_MEMORY_ERROR);
    }
    (*retentionPolicyInfo)->accessLatency = static_cast<ns1__TAccessLatency*>(soap_malloc(soap, sizeof(enum ns1__TAccessLatency)));
    if (NULL == (*retentionPolicyInfo)->accessLatency) {
        *retentionPolicyInfo = NULL;
        srmlogit(STORM_LOG_WARNING, callerName, "Decoding %s: memory allocation error\n", fieldName);
        return(DECODE_ERR_SOAP_MEMORY_ERROR);
    }
    
    (*retentionPolicyInfo)->retentionPolicy = static_cast<ns1__TRetentionPolicy>(retentionPolicy);
    *((*retentionPolicyInfo)->accessLatency) = static_cast<ns1__TAccessLatency>(accessLatency);
    
    return(0);
}

/**
 * The decode_globalTReturnStatus() callerNametion decodes a TReturnStatus from the xml structure received by the BE.
 * @param callerName The name of the caller callerNametion (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc callerNametions.
 * @param soap The gSOAP structure
 * @param returnStatus The TReturnStatus structure returned.
 * @param xmlStruct The xml structure to parse.
 */
int decode_globalTReturnStatus(const char *callerName,
                         xmlrpc_env *env_addr,
                         struct soap *soap,
                         struct ns1__TReturnStatus *returnStatus,
                         xmlrpc_value *xmlStruct)
{
    char *statusCode, *explanation;
    xmlrpc_value *retStatus;
    
    xmlrpc_struct_find_value(env_addr, xmlStruct, "returnStatus", &retStatus);
    if ((env_addr->fault_occurred) || (retStatus == NULL)) {
        srmlogit(STORM_LOG_WARNING, callerName, "StatusCode not specified by BE.\n");
        return(DECODE_ERR_NOT_FOUND);
    }
    
    xmlrpc_decompose_value(env_addr, retStatus, "{s:s,s:s,*}", "statusCode", &statusCode, "explanation", &explanation);
    xmlrpc_DECREF(retStatus);
    if (env_addr->fault_occurred) {
        srmlogit(STORM_LOG_WARNING, callerName, "Error decoding the return status (unable to parse the BE response)\n");
        return(DECODE_ERR_DECODING_ERROR);
    } 
    
    returnStatus->statusCode = static_cast<ns1__TStatusCode>(convertStatusCode(statusCode));
    returnStatus->explanation = soap_strdup(soap, explanation);
    free(statusCode);
    free(explanation);
    
    return(0);
}

/**
 * The decode_TPermissionMode() callerNametion decodes a TPermissionMode (int) from the xml structure received by the BE.
 * @param callerName The name of the caller callerNametion (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc callerNametions.
 * @param soap The gSOAP structure
 * @param permissionMode The ns1__TPermissionMode structure returned.
 * @param xmlStruct The xml structure to parse.
 */
int decode_TPermissionMode(const char *callerName,
                           xmlrpc_env *env_addr,
                           struct soap *soap,
                           enum ns1__TPermissionMode **permissionMode,
                           char* fieldName,
                           xmlrpc_value *xmlStruct)
{
    return decode_int(callerName, env_addr, soap, (int**) permissionMode, fieldName, xmlStruct);
}

/**
 * The decode_TUserPermission() callerNametion decodes a TUserPermission from the xml structure received by the BE.
 * @param callerName The name of the caller callerNametion (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc callerNametions.
 * @param soap The gSOAP structure
 * @param userPermission The returned ns1__TUserPermission (memory is allocated with soap_malloc()).
 * @param fieldName The name of the xml field to search for.
 * @param xmlStruct The xml structure to parse.
 */
int decode_TUserPermission(const char *callerName,
                           xmlrpc_env *env_addr,
                           struct soap *soap,
                           struct ns1__TUserPermission **userPermission,
                           char *fieldName,
                           xmlrpc_value *xmlStruct)
{
    int error;
    enum ns1__TPermissionMode *mode;
    xmlrpc_value *xml_userPermission, *permissionMode;
    
    // Reset return structure    
    *userPermission = NULL;
    
    //Get TUserPermission
    xmlrpc_struct_find_value(env_addr, xmlStruct, fieldName, &xml_userPermission);
    if ((env_addr->fault_occurred) || (NULL == xml_userPermission)) {
        srmlogit(STORM_LOG_DEBUG, callerName, "userPermission not specified by BE.\n");
        return(DECODE_ERR_NOT_FOUND);
    }
    
    /************************ Allocate memory for the response structure ******************************/
    *userPermission = static_cast<ns1__TUserPermission*>(soap_malloc(soap, sizeof(struct ns1__TUserPermission)));
    if (*userPermission == NULL) {
        srmlogit(STORM_LOG_WARNING, callerName, "Memory allocation error for userPermission\n");
        xmlrpc_DECREF(xml_userPermission);
        return(DECODE_ERR_SOAP_MEMORY_ERROR);
    }
    
    /** MANDATORY ********* (1) Decode UserID (string)  *****************************/
    error = decode_string(callerName, env_addr, soap, &((*userPermission)->userID), "userID", xml_userPermission);
    if (error) { //Mandatory Paramter for userPermission
        srmlogit(STORM_LOG_WARNING, callerName, "Error decoding TUserPermission\n");
        xmlrpc_DECREF(xml_userPermission);
        return(DECODE_ERR_NOT_FOUND);
    }
    
    /** MANDATORY ********* (2) Decode TPermissionMode (int)  *****************************/
    error = decode_TPermissionMode(callerName, env_addr, soap, &mode, SRM_PARAM_mode, xml_userPermission);
    if (error) { //Mandatory Paramter for userPermission
        srmlogit(STORM_LOG_WARNING, callerName, "Error decoding userPermission\n");
        xmlrpc_DECREF(xml_userPermission);
        return(DECODE_ERR_NOT_FOUND);
    }
    (*userPermission)->mode = *mode;
    
    xmlrpc_DECREF(xml_userPermission);
    return(0);
}

/**
 * The decode_TGroupPermission() callerNametion decodes a  TGroupPermission from the xml structure received by the BE.
 * @param callerName The name of the caller callerNametion (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc callerNametions.
 * @param soap The gSOAP structure
 * @param userPermission The returned ns1__TGroupPermission (memory is allocated with soap_malloc()).
 * @param fieldName The name of the xml field to search for.
 * @param xmlStruct The xml structure to parse.
 */
int decode_TGroupPermission(const char *callerName,
                  xmlrpc_env *env_addr,
                  struct soap *soap,
                  struct ns1__TGroupPermission **groupPermission,
                  char *fieldName,
                  xmlrpc_value *xmlStruct)
{
    enum ns1__TPermissionMode *mode;
    int error;
    xmlrpc_value *xml_groupPermission, *permissionMode;
    
    //Reset return structure    
    *groupPermission = NULL;
    
    //Get TGroupPermission
    xmlrpc_struct_find_value(env_addr, xmlStruct, fieldName, &xml_groupPermission);
    if ((env_addr->fault_occurred) || (NULL == xml_groupPermission)) {
        srmlogit(STORM_LOG_DEBUG, callerName, "userPermission not specified by BE.\n");
        return(DECODE_ERR_NOT_FOUND);
    }
    
    /************************ Allocate memory for the response structure ******************************/
    *groupPermission = static_cast<ns1__TGroupPermission*>(soap_malloc(soap, sizeof(struct ns1__TGroupPermission)));
    if (*groupPermission == NULL) {
        srmlogit(STORM_LOG_WARNING, callerName, "Memory allocation error for groupPermission\n");
        xmlrpc_DECREF(xml_groupPermission);
        return(DECODE_ERR_SOAP_MEMORY_ERROR);
    }
    
    /** MANDATORY ********* (1) Decode UserID (string)  *****************************/
    error = decode_string(callerName, env_addr, soap, &((*groupPermission)->groupID), "groupID", xml_groupPermission);
    if (error) { //Mandatory Paramter for userPermission
        srmlogit(STORM_LOG_WARNING, callerName, "Error decoding TUserPermission\n");
        xmlrpc_DECREF(xml_groupPermission);
        return(DECODE_ERR_NOT_FOUND);
    }
    
    /** MANDATORY ********* (2) Decode TPermissionMode (int)  *****************************/
    error = decode_TPermissionMode(callerName, env_addr, soap, &mode, SRM_PARAM_mode, xml_groupPermission);
    if (error) { //Mandatory Paramter for userPermission
        srmlogit(STORM_LOG_WARNING, callerName, "Error decoding userPermission\n");
        xmlrpc_DECREF(xml_groupPermission);
        return(DECODE_ERR_NOT_FOUND);
    }
    (*groupPermission)->mode = *mode;
    
    xmlrpc_DECREF(xml_groupPermission);
    return(0);
}

/**
 * The decode_string() callerNametion decodes a string from the xml structure received by the BE.
 * @param callerName The name of the caller callerNametion (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc callerNametions.
 * @param soap The gSOAP structure
 * @param outputString The returned string (memory is allocated with soap_malloc()).
 * @param fieldName The name of the xml field to search for.
 * @param xmlStruct The xml structure to parse.
 */
int decode_string(const char *callerName,
                  xmlrpc_env *env_addr,
                  struct soap *soap,
                  char **outputString,
                  char *fieldName,
                  xmlrpc_value *xmlStruct)
{
    char *strValue;
    
    *outputString = NULL;
    
    xmlrpc_decompose_value(env_addr, xmlStruct, "{s:s,*}", fieldName, &strValue);
    if (env_addr->fault_occurred) {
        srmlogit(STORM_LOG_DEBUG, callerName, "%s not specified by BE\n", fieldName);
        return(DECODE_ERR_NOT_FOUND);
    }
    
    *(outputString) = soap_strdup(soap, strValue);
    free(strValue);
    
    if (NULL == *(outputString)) return(DECODE_ERR_SOAP_MEMORY_ERROR);
    
    return(0);
}

/**
 * The decode_ArrayOfString() callerNametion decodes a string array from the xml structure received by the BE.
 * @param callerName The name of the caller callerNametion (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc callerNametions.
 * @param soap The gSOAP structure
 * @param arrayOfString The returned string (memory is allocated with soap_malloc()).
 * @param fieldName The name of the xml field to search for.
 * @param xmlStruct The xml structure to parse.
 */
int decode_ArrayOfString(const char *callerName,
                  xmlrpc_env *env_addr,
                  struct soap *soap,
                  struct ns1__ArrayOfString **arrayOfString,
                  char *fieldName,
                  xmlrpc_value *xmlStruct)
{
    int arraySize, error, i;
    char **arrayP, *strValue;
    xmlrpc_value *xml_arrayOfString, *xml_arrayItem;
    
    //Reset return structure    
    *arrayOfString = NULL;
    
    xmlrpc_struct_find_value(env_addr, xmlStruct, fieldName, &xml_arrayOfString);
    if ((env_addr->fault_occurred)||(xml_arrayOfString == NULL)) {
        srmlogit(STORM_LOG_DEBUG, callerName, "Array of string not found.\n");
        return(DECODE_ERR_NOT_FOUND);
    }
    
    //Get Size of Array structure
    arraySize = -1;
    arraySize = xmlrpc_array_size(env_addr, xml_arrayOfString);
    
    if ( (env_addr->fault_occurred) || (arraySize<1) ) {
        srmlogit(STORM_LOG_ERROR, callerName, "Error: unable to retrieve the size of stringArray\n");
        xmlrpc_DECREF(xml_arrayOfString);        
        return(DECODE_ERR_NOT_FOUND);
    }

    /****** Allocate memory for the response structure *******/
    error = 0;
    *arrayOfString = static_cast<ns1__ArrayOfString*>(soap_malloc(soap, sizeof(struct ns1__ArrayOfString)));
    if (*arrayOfString == NULL) error = 1;
    else {
        (*arrayOfString)->stringArray = static_cast<char**>(soap_malloc(soap, arraySize * sizeof(char *)));
        if ((*arrayOfString)->stringArray == NULL) error = 1;
    }
    if (error) {
        srmlogit(STORM_LOG_ERROR, callerName, "Error: soap_malloc() error\n");
        /* Clean up xmlrpc error-handling environment. */
        xmlrpc_DECREF(xml_arrayOfString);
        return(DECODE_ERR_SOAP_MEMORY_ERROR);
    }
    
    //Set ArrayOfString size
    (*arrayOfString)->__sizestringArray  = arraySize;  
    //Get pointer to char**
    arrayP = (*arrayOfString)->stringArray;
    
    //Get char*   
    for (i=0; i<arraySize; i++) {
 
        xmlrpc_array_read_item(env_addr, xml_arrayOfString, i, &xml_arrayItem);
        if (env_addr->fault_occurred) {
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
            srmlogit(STORM_LOG_ERROR, callerName, "Error: reading item number %d (arrayItem)\n", i);
            continue;
        }    
        
        strValue = NULL;
        xmlrpc_read_string(env_addr, xml_arrayItem, (const char**) &strValue);
 
        if (env_addr->fault_occurred) {
            srmlogit(STORM_LOG_WARNING, callerName, "%s not specified by BE\n", fieldName);
            if (strValue != NULL) free(strValue);
            xmlrpc_DECREF(xml_arrayItem);
            xmlrpc_DECREF(xml_arrayOfString);
            return(DECODE_ERR_DECODING_ERROR);
        }
        
        //Allocate memory and copy the string obtained
        arrayP[i] = soap_strdup(soap, strValue);
        free(strValue);
        
        if (NULL == arrayP[i]) {
            srmlogit(STORM_LOG_WARNING, callerName, "Soap malloc fail!");
            xmlrpc_DECREF(xml_arrayItem);
            xmlrpc_DECREF(xml_arrayOfString);
            return(DECODE_ERR_SOAP_MEMORY_ERROR);
        }
        
        xmlrpc_DECREF(xml_arrayItem);
    }
    
    xmlrpc_DECREF(xml_arrayOfString);
    
    return(0);      

}


/**
 * The decode_int() callerNametion decodes an int from the xml structure received by the BE.
 * @param callerName The name of the caller callerNametion (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc callerNametions.
 * @param soap The gSOAP structure
 * @param outputInt The returned int (memory is allocated with soap_malloc()).
 * @param fieldName The name of the xml field to search for.
 * @param xmlStruct The xml structure to parse.
 */
int decode_int(const char *callerName,
               xmlrpc_env *env_addr,
               struct soap *soap,
               int **outputInt,
               char *fieldName,
               xmlrpc_value *xmlStruct)
{
    int i;
    
    *outputInt = NULL;
    
    xmlrpc_decompose_value(env_addr, xmlStruct, "{s:i,*}", fieldName, &i);
    if (env_addr->fault_occurred) {
        srmlogit(STORM_LOG_DEBUG, callerName, "%s not specified by BE\n", fieldName);
        return(DECODE_ERR_NOT_FOUND);
    }
    
    *outputInt = static_cast<int*>(soap_malloc(soap, sizeof(int)));
    if (NULL == *(outputInt)) return(DECODE_ERR_SOAP_MEMORY_ERROR);
    
    **outputInt = i;
    
    return(0);
}

/**
 * The decode_ULONG64() callerNametion decodes a long int 64 bit from the xml structure received by the BE.
 * @param callerName The name of the caller callerNametion (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc callerNametions.
 * @param soap The gSOAP structure
 * @param outputULONG64 The returned ULONG64 (memory is allocated with soap_malloc()).
 * @param fieldName The name of the xml field to search for.
 * @param xmlStruct The xml structure to parse.
 */
int decode_ULONG64(const char *callerName,
                   xmlrpc_env *env_addr,
                   struct soap *soap,
                   ULONG64 **outputULONG64,
                   char *fieldName,
                   xmlrpc_value *xmlStruct)
{
    char *strULONG64;
    int retStat;
    
    *outputULONG64 = NULL;
    
    retStat = decode_string(callerName, env_addr, soap, &strULONG64, fieldName, xmlStruct);
    if (retStat != 0) return(retStat);
    
    *outputULONG64 = static_cast<ULONG64*>(soap_malloc(soap, sizeof(ULONG64)));
    if (NULL == *(outputULONG64)) return(DECODE_ERR_SOAP_MEMORY_ERROR);
    
    **outputULONG64 = strtoull((char*) strULONG64, (char**)NULL,10);
    
    // strULONG64 is allocated with soap_strdup(), therefore it must not be freed
    
    return(0);
}

/**
 * The decode_TReturnStatus() callerNametion decodes a TReturnStatus from the xml structure received by the BE.
 * The difference with decode_globalTReturnStatus is in the filedName and that now we allocate memory
 * for the returned TReturnStatus.
 * @param callerName The name of the caller callerNametion (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc callerNametions.
 * @param soap The gSOAP structure
 * @param status The returned status (memory is allocated with soap_malloc()).
 * @param xmlStruct The xml structure to parse.
 */
int decode_TReturnStatus(const char *callerName,
                  xmlrpc_env *env_addr,
                  struct soap *soap,
                  struct ns1__TReturnStatus **status,
                  char *fieldName,
                  xmlrpc_value *xmlStruct)
{
    char *statusCode, *explanation;
    xmlrpc_value *retStatus;
    
    *status = NULL;
    
    xmlrpc_struct_find_value(env_addr, xmlStruct, fieldName, &retStatus);
    if (env_addr->fault_occurred) {
        srmlogit(STORM_LOG_ERROR, callerName, "Error decoding status.\n");
        return(DECODE_ERR_NOT_FOUND);
    }
    if (retStatus == NULL) {
        srmlogit(STORM_LOG_WARNING, callerName, "Warning: missing status.\n");
        return(DECODE_ERR_NOT_FOUND);
    }
    
    xmlrpc_decompose_value(env_addr, retStatus, "{s:s,s:s,*}", "statusCode", &statusCode, "explanation", &explanation);
    xmlrpc_DECREF(retStatus);
    if (env_addr->fault_occurred) {
        srmlogit(STORM_LOG_ERROR, callerName, "Error decoding the return status (unable to parse the BE response)\n");
        return(DECODE_ERR_DECODING_ERROR);
    } 
    
    /************ Allocate memory for the response structure struct ns1__TReturnStatus *status *************/
    *status = static_cast<ns1__TReturnStatus*>(soap_malloc(soap, sizeof(struct ns1__TReturnStatus)));
    if (NULL == *status) {
        srmlogit(STORM_LOG_ERROR, callerName, "Memory allocation error.\n");
        free(statusCode);
        free(explanation);
        return(DECODE_ERR_SOAP_MEMORY_ERROR);
    }
    
    (*status)->statusCode = static_cast<ns1__TStatusCode>(convertStatusCode(statusCode));
    (*status)->explanation = soap_strdup(soap, explanation);
    free(statusCode);
    free(explanation);
    
    return(0);
}

/**
 * The decode_ArrayOfTMetaDataPathDetail() callerNametion decodes the ArrayOfTMetaDataPathDetails from the xml structure received by the BE.
 * @param callerName The name of the caller callerNametion (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc callerNametions.
 * @param soap The gSOAP structure
 * @param arrayOfTMetaData The returned ns1__ArrayOfTMetaDataPathDetail (memory is allocated with soap_malloc()).
 * @param xmlStruct The xml structure to parse.
 */
int decode_ArrayOfTMetaDataPathDetail(const char *callerName,
                                      xmlrpc_env *env_addr,
                                      struct soap *soap,
                                      struct ns1__ArrayOfTMetaDataPathDetail **arrayOfTMetaData,
                                      char *fieldName,
                                      xmlrpc_value *xmlStruct)
{
    static const char *funcName = "decode_ArrayOfTMetaDataPathDetail";
    struct ns1__TMetaDataPathDetail *repfilep;
    struct ns1__TMetaDataPathDetail **pathDetailArray;
    int i, arraySize, error;
    xmlrpc_value *xml_arrayOfTMetaData;
    xmlrpc_value *xml_arrayItem;
    
    *arrayOfTMetaData = NULL;
    
    xmlrpc_decompose_value(env_addr, xmlStruct, "{s:A,*}", fieldName, &xml_arrayOfTMetaData);
    if ((env_addr->fault_occurred) || (xml_arrayOfTMetaData == NULL)) {
        srmlogit(STORM_LOG_DEBUG, callerName, "Array of details not found.\n");
        return(DECODE_ERR_NOT_FOUND);
    }

    arraySize = -1;    
    arraySize = xmlrpc_array_size(env_addr, xml_arrayOfTMetaData);
    if ( (env_addr->fault_occurred) || (arraySize<1) ) {
        srmlogit(STORM_LOG_ERROR, callerName, "Error: unable to retrieve the size of details\n");
        xmlrpc_DECREF(xml_arrayOfTMetaData);
        return(DECODE_ERR_NOT_FOUND);
    }

    srmlogit(STORM_LOG_DEBUG, callerName, "Array Size %i\n", arraySize);
  
    /****************************************************************************************************/
    /******************* Allocate memory for the response structure *************************************/
    *arrayOfTMetaData = static_cast<ns1__ArrayOfTMetaDataPathDetail*>(soap_malloc(soap, sizeof(struct ns1__ArrayOfTMetaDataPathDetail)));
    error = 0;
    if (*arrayOfTMetaData == NULL) error = 1;
    else {
        pathDetailArray = static_cast<ns1__TMetaDataPathDetail**>(soap_malloc(soap, arraySize * sizeof(struct ns1__TMetaDataPathDetail *)));
        if (pathDetailArray == NULL) error = 1;
    }
    if (error) {
        srmlogit(STORM_LOG_ERROR, callerName, "Error: soap_malloc() error\n");
        /* Clean up xmlrpc error-handling environment. */
        xmlrpc_DECREF(xml_arrayOfTMetaData);
        return(DECODE_ERR_SOAP_MEMORY_ERROR);
    }
    
    (*arrayOfTMetaData)->__sizepathDetailArray = arraySize;
    (*arrayOfTMetaData)->pathDetailArray = pathDetailArray;
    
    // Allocate memory for each pathDetailArray item
    for (i=0; i<arraySize; i++) {
        pathDetailArray[i] = static_cast<ns1__TMetaDataPathDetail*>(soap_malloc(soap, sizeof(struct ns1__TMetaDataPathDetail)));
        if (pathDetailArray[i] == NULL)
        {
            srmlogit(STORM_LOG_ERROR, callerName, "Error: soap_malloc() error\n");
            xmlrpc_DECREF(xml_arrayOfTMetaData);
            return(DECODE_ERR_SOAP_MEMORY_ERROR);
        }
        memset(pathDetailArray[i], 0, sizeof(struct ns1__TMetaDataPathDetail));
    } 
 
    /***********************************************************************************************/
    /*********** Parse the 18 entries of the TMetaDataPathDetail structure ************************/
    /***********************************************************************************************/
    for (i=0; i<arraySize; i++) {
 
        repfilep = pathDetailArray[i];
        
        xmlrpc_array_read_item(env_addr,xml_arrayOfTMetaData, i, &xml_arrayItem);
        if (env_addr->fault_occurred) {
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
            srmlogit(STORM_LOG_ERROR, callerName, "Error: reading item number %d (arrayItem)\n", i);
            continue;
        }
 
        /** MANDATORY ************* (1) Get surl (char *) ****************************/
        error = decode_string(callerName, env_addr, soap, &(repfilep->path), SRM_PARAM_path, xml_arrayItem);
        if (error) {
            xmlrpc_DECREF(xml_arrayItem);
            xmlrpc_DECREF(xml_arrayOfTMetaData);
            return error;
        }
        
        /** OPTIONAL ************** (2) Get status (TReturnStatus) ****************************/
        error = decode_TReturnStatus(callerName, env_addr, soap, &(repfilep->status), SRM_PARAM_status, xml_arrayItem);
        if (error) {
             if (error != DECODE_ERR_NOT_FOUND) {  
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfTMetaData);
                return error;
             }
            //Parameter not found, but it's optional so continue execution..
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        /** OPTIONAL ********** (3) Get size (ULONG64) ***********************/
        error =  decode_ULONG64(callerName, env_addr, soap, &(repfilep->size), SRM_PARAM_size, xml_arrayItem);
        if (error) {
            if (error!= DECODE_ERR_NOT_FOUND) { 
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfTMetaData);
                return error;
            }
            //Parameter not found, but it's optional so continue execution..
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        /** OPTIONAL ************** (4) Get createdAtTime (char *) ****************************/
        error =  decode_string(callerName, env_addr, soap, &(repfilep->createdAtTime), SRM_PARAM_createdAtTime, xml_arrayItem);
        if (error) {
            if (error!= DECODE_ERR_NOT_FOUND) { 
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfTMetaData);
                return error;
            }
            //Parameter not found, but it's optional so continue execution..
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        /** OPTIONAL ************** (5) Get lastModificationTime (char *) ****************************/
        error = decode_string(callerName, env_addr, soap, &(repfilep->lastModificationTime), SRM_PARAM_lastModificationTime, xml_arrayItem);
        if (error) {
            if (error!= DECODE_ERR_NOT_FOUND) { 
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfTMetaData);
                return error;
            }
            //Parameter not found, but it's optional so continue execution..
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        /** OPTIONAL ************** (6) Get fileStorageType (TFileStorageType) ****************************/
        error = decode_int(callerName, env_addr, soap, (int **) &(repfilep->fileStorageType),  SRM_PARAM_fileStorageType, xml_arrayItem);
        if (error) {
            if (error!= DECODE_ERR_NOT_FOUND) { 
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfTMetaData);
                return error;
            }
            //Parameter not found, but it's optional so continue execution..
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        /** OPTIONAL ************** (7) Get retentionPolicyInfo (TRetentionPolicyInfo) ****************************/
        error = decode_TRetentionPolicyInfo(callerName, env_addr, soap, &(repfilep->retentionPolicyInfo), SRM_PARAM_retentionPolicyInfo, xml_arrayItem);
        if (error) {
            if (error!= DECODE_ERR_NOT_FOUND) { 
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfTMetaData);
                return error;
            }
            //Parameter not found, but it's optional so continue execution..
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        /** OPTIONAL ************** (8) Get fileLocality (TFileLocality) ****************************/
        error = decode_int(callerName, env_addr, soap, (int **) &(repfilep->fileLocality), SRM_PARAM_fileLocality, xml_arrayItem);
        if (error) {
            if (error!= DECODE_ERR_NOT_FOUND) { 
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfTMetaData);
                return error;
            }
            //Parameter not found, but it's optional so continue execution..
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        /** OPTIONAL ************** (9) Get arrayOfTSpaceTokens (ArrayOfString) ****************************/
        error = decode_ArrayOfString(callerName, env_addr, soap, &(repfilep->arrayOfSpaceTokens), SRM_PARAM_arrayOfTSpaceTokens, xml_arrayItem);
        if (error) {
            if (error!= DECODE_ERR_NOT_FOUND) { 
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfTMetaData);
                return error;
            }
            //Parameter not found, but it's optional so continue execution..
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        /** OPTIONAL ************** (10) Get TFileType (int) ****************************/
        error = decode_int(callerName, env_addr, soap, (int **) &(repfilep->type), SRM_PARAM_type, xml_arrayItem);
        if (error) {
            if (error!= DECODE_ERR_NOT_FOUND) { 
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfTMetaData);
                return error;
            }
            //Parameter not found, but it's optional so continue execution..
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        /** OPTIONAL ************** (11) Get lifetimeAssigned (int) ****************************/
        error = decode_lifetimeValue(callerName, env_addr, soap, &(repfilep->lifetimeAssigned), SRM_PARAM_lifetimeAssigned, xml_arrayItem);
        if (error) {
            if (error!= DECODE_ERR_NOT_FOUND) { 
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfTMetaData);
                return error;
            }
            //Parameter not found, but it's optional so continue execution..
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        /** OPTIOANL ************** (12) Get lifetimeLeft (int) ****************************/
        error = decode_lifetimeValue(callerName, env_addr, soap, &(repfilep->lifetimeLeft), SRM_PARAM_lifetimeLeft, xml_arrayItem);
        if (error) {
            if (error!= DECODE_ERR_NOT_FOUND) { 
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfTMetaData);
                return error;
            }
            //Parameter not found, but it's optional so continue execution..
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
  
        /** OPTIONAL ************** (13) Get ownerPermission (TUserPermission) ****************************/
        error = decode_TUserPermission(callerName, env_addr ,soap, &(repfilep->ownerPermission), SRM_PARAM_ownerPermission, xml_arrayItem);
        if (error) {
            if (error!= DECODE_ERR_NOT_FOUND) { 
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfTMetaData);
                return error;
            }
            //Parameter not found, but it's optional so continue execution..
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        /** OPTIONAL ************** (14) Get groupPermission (TGroupPermission) ****************************/
        error = decode_TGroupPermission(callerName, env_addr ,soap, &(repfilep->groupPermission), SRM_PARAM_groupPermission, xml_arrayItem);
        if (error) {
            if (error!= DECODE_ERR_NOT_FOUND) { 
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfTMetaData);
                return error;
            }
            //Parameter not found, but it's optional so continue execution..
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
  
        /** OPTIONAL ************** (15) Get otherPermission (TPermissionMode) ****************************/
        error = decode_TPermissionMode(callerName, env_addr ,soap, &(repfilep->otherPermission), SRM_PARAM_otherPermission, xml_arrayItem);
        if (error) {
            if (error!= DECODE_ERR_NOT_FOUND) { 
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfTMetaData);
                return error;
            }
            //Parameter not found, but it's optional so continue execution..
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
            
        /** OPTIONAL ************** (16) Get checkSumType (string) ****************************/
        error = decode_string(callerName, env_addr, soap, &(repfilep->checkSumType), SRM_PARAM_checkSumType, xml_arrayItem);
        if (error) {
            if (error!= DECODE_ERR_NOT_FOUND) { 
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfTMetaData);
                return error;
            }
            //Parameter not found, but it's optional so continue execution..
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
      
        /** OPTIONAL ************** (17) Get checkSumValue (string) ****************************/
        error = decode_string(callerName, env_addr, soap, &(repfilep->checkSumValue), SRM_PARAM_checkSumValue, xml_arrayItem);
        if (error) {
            if (error!= DECODE_ERR_NOT_FOUND) { 
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfTMetaData);
                return error;
            }
            //Parameter not found, but it's optional so continue execution..
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
    
        /** OPTIONAL ************** (18) Get arrayOfSubPaths ****************************/
        error = decode_ArrayOfTMetaDataPathDetail(funcName, env_addr, soap, &(repfilep->arrayOfSubPaths), SRM_PARAM_arrayOfSubPaths, xml_arrayItem);
        if (error) {
            if (error!= DECODE_ERR_NOT_FOUND) {
                xmlrpc_DECREF(xml_arrayItem);
                xmlrpc_DECREF(xml_arrayOfTMetaData);
                return error;
            }
            //Parameter not found, but it's optional so continue execution..
            xmlrpc_env_clean(env_addr);
            xmlrpc_env_init(env_addr);
        }
        
        xmlrpc_DECREF(xml_arrayItem);
    } // for (i=0; i<arraySize; i++)
    
    xmlrpc_DECREF(xml_arrayOfTMetaData);
    
    return(0);
}

