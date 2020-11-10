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
 * \file xml_encode.c
 *
 * This file contains the functions implementing, for each input type of srmv2.2, the functions
 * that encode a specified type into an xml structure.
 */

#include "xmlrpc_encode.hpp"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <cgsi_plugin.h>
#include "get_socket_info.h"
#include "surl_normalizer.h"
#include <xmlrpc-c/util.h>
#include "FrontendConfiguration.hpp"

static bool getXMLRPCCheckAscii() 
{
    return FrontendConfiguration::getInstance()->getXMLRPCCheckAscii(); 
}

static int isASCII(const char *data)
{
    const unsigned char *str = (const unsigned char*)data;
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] & 0x80)
        {
            return 0;
        }
    }
    return 1;
}


/***************************************************************************************************************/
/***************************************************************************************************************/
/***************************************************************************************************************/

/**
 * The encode_lifetimeValue() function encodes a lifetime value into a xml structure
 * In the wsdl of SRM v2.2 lifetimes are int,  we encode as a ULONG64
 * @param callerName The name of the caller function (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc functions.
 * @param soap The gSOAP structure
 * @param lifetimeVal The lifetime value.
 * @param fieldName The name to assign to the xmlrpc structure
 * @param xmlStruct The xmlrpc destination variable.
 */
int encode_lifetimeValue(const char *callerName,
        xmlrpc_env *env_addr,
        int *lifetimeVal,
        char const* fieldName,
        xmlrpc_value *xmlStruct)
{
    ULONG64 longValue;

    if (lifetimeVal == NULL) {
        srmlogit(STORM_LOG_ERROR, callerName, "Warning: missing %s parameter\n", fieldName);
        return(ENCODE_ERR_MISSING_PARAM);
    }

    longValue = *lifetimeVal;
    XMLRPC_ASSERT_ENV_OK(env_addr);
    return encode_ULONG64(callerName, env_addr, &longValue, fieldName, xmlStruct);
}
/**
 * The encode_arrayOfString() function encodes an array of strings into a xml structure
 * @param callerName The name of the caller function (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc functions.
 * @param soap The gSOAP structure
 * @param arrayOfString The array of long values.
 * @param fieldName The name to assign to the xmlrpc structure
 * @param xmlStruct The xmlrpc destination variable.
 */
int encode_arrayOfString(const char *callerName,
        xmlrpc_env *env_addr,
        struct ns1__ArrayOfString *arrayOfString,
        char const* fieldName,
        xmlrpc_value *xmlStruct)
{
    char **stringArray;
    int i, nbItems;
    xmlrpc_value *xml_string, *xml_arrayOfString;

    if (arrayOfString == NULL) {
        srmlogit(STORM_LOG_ERROR, callerName, "Warning: missing arrayOfString parameter\n");
        return(ENCODE_ERR_MISSING_PARAM);
    }
    nbItems = arrayOfString->__sizestringArray;
    srmlogit(STORM_LOG_DEBUG, callerName, "%s: size=%d\n", fieldName, nbItems);
    if (nbItems < 1) {
        srmlogit(STORM_LOG_ERROR, callerName, "Warning: missing arrayOfString parameter\n");
        return(ENCODE_ERR_MISSING_PARAM);
    }
    stringArray = arrayOfString->stringArray;
    if (stringArray == NULL) {
        srmlogit(STORM_LOG_ERROR, callerName, "Error: stringArray is NULL (but it should be NOT)\n");
        return(ENCODE_ERR_ENCODING_ERROR);
    }

    XMLRPC_ASSERT_ENV_OK(env_addr);
    xml_arrayOfString = xmlrpc_array_new(env_addr);
    XMLRPC_ASSERT_ENV_OK(env_addr);

    for (i=0; i<nbItems; i++) {
        if (stringArray[i] == NULL) {
            srmlogit(STORM_LOG_DEBUG, callerName, "string[%d] is NULL\n", i);
        } else {
            srmlogit(STORM_LOG_DEBUG, callerName, "string[%d]=\"%s\"\n", i, stringArray[i]);
            if(getXMLRPCCheckAscii() && !isASCII(stringArray[i]))
            {
                srmlogit(STORM_LOG_ERROR, callerName, "Unable to encode value: %s , it contains non ASCII characters\n", stringArray[i]);
                xmlrpc_DECREF(xml_arrayOfString);
                return(ENCODE_ERR_ENCODING_ERROR);
            }
            xml_string = xmlrpc_string_new(env_addr, stringArray[i]);
            XMLRPC_ASSERT_ENV_OK(env_addr);
            xmlrpc_array_append_item(env_addr, xml_arrayOfString, xml_string);
            xmlrpc_DECREF(xml_string);
        }
    }

    xmlrpc_struct_set_value(env_addr, xmlStruct, fieldName, xml_arrayOfString);
    xmlrpc_DECREF(xml_arrayOfString);

    return(0);
}

/**
 * The encode_ArrayOfUnsignedLong() function encodes an array of long values into a xml structure
 * @param callerName The name of the caller function (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc functions.
 * @param soap The gSOAP structure
 * @param arrayOfUnsignedLong The array of long values.
 * @param fieldName The name to assign to the xmlrpc structure
 * @param xmlStruct The xmlrpc destination variable.
 */
int encode_arrayOfUnsignedLong(const char *callerName,
        xmlrpc_env *env_addr,
        struct ns1__ArrayOfUnsignedLong *arrayOfUnsignedLong,
        char const* fieldName,
        xmlrpc_value *xmlStruct)
{
    char long64Str[NUM_OF_LONG_CHR];
    int i, nbItems;
    ULONG64 *unsignedLongArray;
    xmlrpc_value *xml_arrayOfLong, *xml_string;

    if (arrayOfUnsignedLong == NULL) {
        srmlogit(STORM_LOG_ERROR, callerName, "Warning: missing arrayOfUnsignedLong parameter\n");
        return(ENCODE_ERR_MISSING_PARAM);
    }
    nbItems = arrayOfUnsignedLong->__sizeunsignedLongArray;
    if (nbItems < 1) {
        srmlogit(STORM_LOG_ERROR, callerName, "Warning: missing arrayOfUnsignedLong parameter\n");
        return(ENCODE_ERR_MISSING_PARAM);
    }

    unsignedLongArray = arrayOfUnsignedLong->unsignedLongArray;
    xml_arrayOfLong = xmlrpc_array_new(env_addr);
    XMLRPC_ASSERT_ENV_OK(env_addr);
    for (i=0; i<nbItems; i++) {
        snprintf(long64Str, NUM_OF_LONG_CHR, "%lu", unsignedLongArray[i]);
        xml_string = xmlrpc_string_new(env_addr, long64Str);
        XMLRPC_ASSERT_ENV_OK(env_addr);
        xmlrpc_array_append_item(env_addr, xml_arrayOfLong, xml_string);
        xmlrpc_DECREF(xml_string);
    }

    xmlrpc_struct_set_value(env_addr, xmlStruct, fieldName, xml_arrayOfLong);
    xmlrpc_DECREF(xml_arrayOfLong);

    return(0);
}

/**
 * The encode_intPointer() function encodes an int value specified by a pointer into a xml structure
 * @param callerName The name of the caller function (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc functions.
 * @param intVal The pointer to an int value.
 * @param fieldName The name to assign to the xmlrpc structure
 * @param xmlStruct The xmlrpc destination variable.
 */
int encode_int(const char *callerName, xmlrpc_env *env_addr, int *intVal, char const* fieldName, xmlrpc_value *xmlStruct)
{
    xmlrpc_value *xml_intVal;

    if (intVal == NULL) {
        srmlogit(STORM_LOG_DEBUG, callerName, "Warning: missing int parameter\n");
        return(ENCODE_ERR_MISSING_PARAM);
    }

    xml_intVal = xmlrpc_int_new(env_addr, *intVal);
    XMLRPC_ASSERT_ENV_OK(env_addr);
    xmlrpc_struct_set_value(env_addr, xmlStruct, fieldName, xml_intVal);
    xmlrpc_DECREF(xml_intVal);

    return(0);
}

/**
 * The encode_boolPointer() function encodes a boolean value specified by a pointer into a xml structure
 * @param callerName The name of the caller function (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc functions.
 * @param boolVal The pointer to an int value.
 * @param fieldName The name to assign to the xmlrpc structure
 * @param xmlStruct The xmlrpc destination variable.
 */
int encode_bool(const char *callerName, xmlrpc_env *env_addr, unsigned int *boolVal, char const* fieldName, xmlrpc_value *xmlStruct)
{
    xmlrpc_value *xml_boolVal;

    if (boolVal == NULL) {
        srmlogit(STORM_LOG_DEBUG, callerName, "Warning: missing bool parameter\n");
        return(ENCODE_ERR_MISSING_PARAM);
    }

    xml_boolVal = xmlrpc_bool_new(env_addr, *boolVal);
    XMLRPC_ASSERT_ENV_OK(env_addr);
    xmlrpc_struct_set_value(env_addr, xmlStruct, fieldName, xml_boolVal);
    xmlrpc_DECREF(xml_boolVal);

    return(0);
}


/**
 * The encode_ULONG64pointer() function encodes a long 64 bit value  specified by a pointer into a xml structure
 * @param callerName The name of the caller function (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc functions.
 * @param soap The gSOAP structure
 * @param long64Val The pointer to a long 64 bit value.
 * @param fieldName The name to assign to the xmlrpc structure
 * @param xmlStruct The xmlrpc destination variable.
 */
int encode_ULONG64(const char *callerName, xmlrpc_env *env_addr, ULONG64 *long64Val, char const* fieldName, xmlrpc_value *xmlStruct)
{
    char long64Str[NUM_OF_LONG_CHR];
    xmlrpc_value *xml_string;

    if (long64Val == NULL) {
        srmlogit(STORM_LOG_DEBUG, callerName,"Warning: missing ULONG64 parameter\n");
        return(ENCODE_ERR_MISSING_PARAM);
    }
    /* Convert LONG64 value into string */
    snprintf(long64Str, NUM_OF_LONG_CHR, "%lu", *long64Val);
    xml_string = xmlrpc_string_new(env_addr, long64Str);
    XMLRPC_ASSERT_ENV_OK(env_addr);
    xmlrpc_struct_set_value(env_addr, xmlStruct, fieldName, xml_string);
    xmlrpc_DECREF(xml_string);

    return(0);
}

/**
 * The encode_retentionPolicyInfo() function encodes the retentionPolicyInfo field (SRM v2.2) into a xml structure
 * @param callerName The name of the caller function (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc functions.
 * @param soap The gSOAP structure
 * @param retentionPolicyInfo The retentionPolicyInfo structure, it is a struct ns1__TRetentionPolicyInfo * in SRM v2.2.
 * @param xmlStruct The xmlrpc destination variable.
 */
int encode_retentionPolicyInfo(const char *callerName,
        xmlrpc_env *env_addr,
        struct ns1__TRetentionPolicyInfo *retentionPolicyInfo,
        xmlrpc_value *xmlStruct)
{
    xmlrpc_value *xml_retentionPolicyInfo, *xml_intVal;

    if (retentionPolicyInfo == NULL) {
        srmlogit(STORM_LOG_ERROR, callerName,"Warning: missing retentionPolicyInfo parameter\n");
        return(ENCODE_ERR_MISSING_PARAM);
    }

    xml_retentionPolicyInfo = xmlrpc_struct_new(env_addr);
    XMLRPC_ASSERT_ENV_OK(env_addr);
    // Encode retentionPolicy field
    xml_intVal = xmlrpc_int_new(env_addr, retentionPolicyInfo->retentionPolicy);
    XMLRPC_ASSERT_ENV_OK(env_addr);
    xmlrpc_struct_set_value(env_addr, xml_retentionPolicyInfo, "retentionPolicy", xml_intVal);
    xmlrpc_DECREF(xml_intVal);
    // Encode accessLatency field
    if (retentionPolicyInfo->accessLatency != NULL) {
        xml_intVal = xmlrpc_int_new(env_addr, *(retentionPolicyInfo->accessLatency));
        XMLRPC_ASSERT_ENV_OK(env_addr);
        xmlrpc_struct_set_value(env_addr, xml_retentionPolicyInfo, "accessLatency", xml_intVal);
        xmlrpc_DECREF(xml_intVal);
    }

    xmlrpc_struct_set_value(env_addr, xmlStruct, "retentionPolicyInfo", xml_retentionPolicyInfo);
    xmlrpc_DECREF(xml_retentionPolicyInfo);

    return(0);
}

/**
 * The encode_userSpaceTokenDescription() function encodes the spaceToken field (SRM v2.2) into a xml structure
 * @param callerName The name of the caller function (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc functions.
 * @param soap The gSOAP structure
 * @param spaceToken The space token value, it is a char * in SRM v2.2.
 * @param xmlStruct The xmlrpc destination variable.
 */
int encode_userSpaceTokenDescription(const char *callerName, xmlrpc_env *env_addr, char *spaceToken, xmlrpc_value *xmlStruct)
{
    xmlrpc_value *xml_spaceToken;

    if (spaceToken == NULL) return(ENCODE_ERR_MISSING_PARAM);
    if(getXMLRPCCheckAscii() && !isASCII(spaceToken))
    {
        srmlogit(STORM_LOG_ERROR, callerName, "Unable to encode value: %s , it contains non ASCII characters\n", spaceToken);
        return(ENCODE_ERR_ENCODING_ERROR);
    }
    xml_spaceToken = xmlrpc_string_new(env_addr, spaceToken);
    XMLRPC_ASSERT_ENV_OK(env_addr);
    xmlrpc_struct_set_value(env_addr, xmlStruct, "userSpaceTokenDescription", xml_spaceToken);
    xmlrpc_DECREF(xml_spaceToken);

    return(0);
}

/**
 * The encode_VOMSAttributes() function encodes the authorizationID field (SRM v2.2) into a xml structure
 * @param callerName The name of the caller function (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc functions.
 * @param soap The gSOAP structure
 * @param autohID The authorizationID value, it is a char * in SRM v2.2.
 * @param xmlStruct The xmlrpc destination variable.
 */
int encode_VOMSAttributes(const char *callerName, xmlrpc_env *env_addr, struct soap *soap, char * /* autohID */, xmlrpc_value *xmlStruct)
{
    char clientdn[256], **fqans;
    int i, nbfqans, error;
    xmlrpc_value *userDN, *fqansArray, *fqansItem;
    char ip[256];

    /* Initialized to empty string */
    ip[0]=0;
    clientdn[0] = 0;

    /* Get DN and FQAN from the CGSI plugin and the CGSI_VOMS plugin */
    get_client_dn(soap, clientdn, sizeof(clientdn));
    nbfqans = 0;
    /* fqans will point to a memory area in the soap structure: it must not be freed */
    fqans = get_client_roles(soap, &nbfqans);

    /* Paranoic error check for the result returned by the get_client_roles() function */
    if (nbfqans > 0) {
        if (fqans == NULL) {
            srmlogit(STORM_LOG_ERROR, callerName,"ERROR: FQAN not found (but they should exist): fqans=NULL\n");
            return(ENCODE_ERR_ENCODING_ERROR);
        }
    }

    /* Encode the userDN field */
    userDN = xmlrpc_string_new(env_addr, clientdn);
    XMLRPC_ASSERT_ENV_OK(env_addr);
    xmlrpc_struct_set_value(env_addr, xmlStruct, "userDN", userDN);

    srmlogit(STORM_LOG_DEBUG, callerName, "UserDN=%s\n", clientdn);
    srmlogit(STORM_LOG_DEBUG, callerName, "Client IP=%s\n", getip(soap, ip));

    xmlrpc_DECREF(userDN);

    srmlogit(STORM_LOG_DEBUG, callerName, "Number of FQANs: %d\n", nbfqans);

    if (nbfqans > 0) {
        fqansArray = xmlrpc_array_new(env_addr);
        XMLRPC_ASSERT_ENV_OK(env_addr);
        /* Encode FQANS (fqans is an array of strings) into fqansArray */
        error = 1;  // variable for paranoic error check
        for (i=0; i<nbfqans; i++) {
            if (fqans[i] != NULL) {
                srmlogit(STORM_LOG_DEBUG, callerName, "FQAN[%d]: %s\n", i, fqans[i]);
                error = 0;
                fqansItem = xmlrpc_string_new(env_addr, fqans[i]);
                XMLRPC_ASSERT_ENV_OK(env_addr);
                xmlrpc_array_append_item(env_addr, fqansArray, fqansItem);
                xmlrpc_DECREF(fqansItem);
            }
            else srmlogit(STORM_LOG_DEBUG, callerName, "FQAN[%d] is NULL\n", i);
        }
        if (!error) xmlrpc_struct_set_value(env_addr, xmlStruct, "userFQANS", fqansArray);
        xmlrpc_DECREF(fqansArray);
    }
    return(0);
}

/**
 * The encode_ArrayOfAnyURI() function encodes the arrayOfSURLs field (SRM v2.2) into a xml structure
 * @param callerName The name of the caller function (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc functions.
 * @param arrayOfSURLs The authorizationID value, it is a struct ns1__ArrayOfAnyURI * in SRM v2.2.
 * @param xmlStruct The xmlrpc destination variable.
 */
int encode_ArrayOfAnyURI(const char *callerName,
        xmlrpc_env *env_addr,
        struct ns1__ArrayOfAnyURI *arrayOfSURLs,
        char const* fieldName,
        xmlrpc_value *xmlStruct)
{
    char **urlArray;
    int i, nbsurls;
    xmlrpc_value *xml_urlArray, *xml_SURL;
    char* normalized_surl = NULL;

    if (arrayOfSURLs == NULL) return(ENCODE_ERR_MISSING_PARAM);

    nbsurls = arrayOfSURLs->__sizeurlArray;
    if (nbsurls < 1) return(ENCODE_ERR_GENERAL_ERROR);

    urlArray = arrayOfSURLs->urlArray;
    if (urlArray == NULL) return(ENCODE_ERR_MISSING_PARAM);

    xml_urlArray = xmlrpc_array_new(env_addr);
    XMLRPC_ASSERT_ENV_OK(env_addr);

    for (i=0; i<nbsurls; i++) {
        if (urlArray[i] == NULL) {
            srmlogit(STORM_LOG_DEBUG, callerName, "SURL[%d] is NULL \n", i);
            continue;
        }
        srmlogit(STORM_LOG_DEBUG, callerName, "SURL[%d]: %s\n", i, urlArray[i]);
        if(getXMLRPCCheckAscii() && !isASCII(urlArray[i]))
        {
            srmlogit(STORM_LOG_ERROR, callerName, "Unable to encode value: %s , it contains non ASCII characters\n", urlArray[i]);
            xmlrpc_DECREF(xml_urlArray);
            return(ENCODE_ERR_ENCODING_ERROR);
        }

        normalized_surl = storm_normalize_surl(urlArray[i]);
        if (normalized_surl == NULL)
        {
            srmlogit(STORM_LOG_ERROR, callerName, "Unable to encode value: %s. SURL validation failed\n", urlArray[i]);
            xmlrpc_DECREF(xml_urlArray);
            return(ENCODE_ERR_ENCODING_ERROR);

        }else{

            xml_SURL = xmlrpc_string_new(env_addr, normalized_surl);
            XMLRPC_ASSERT_ENV_OK(env_addr);
            free(normalized_surl);
        }

        xmlrpc_array_append_item(env_addr, xml_urlArray, xml_SURL);
        xmlrpc_DECREF(xml_SURL);
    }

    xmlrpc_struct_set_value(env_addr, xmlStruct, fieldName, xml_urlArray);
    xmlrpc_DECREF(xml_urlArray);

    return(0);
}

/**
 * The encode_arrayOfTExtraInfo() function encodes a truct ns1__ArrayOfTExtraInfo * into a xml structure
 * @param callerName The name of the caller function (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc functions.
 * @param soap The gSOAP structure
 * @param extraInfo The extra info structure.
 * @param fieldName The name to assign to the xmlrpc structure
 * @param xmlStruct The xmlrpc destination variable.
 */
int encode_ArrayOfTExtraInfo(const char *callerName,
        xmlrpc_env *env_addr,
        struct ns1__ArrayOfTExtraInfo *extraInfo,
        char const* fieldName,
        xmlrpc_value *xmlStruct)
{
    struct ns1__TExtraInfo **extraInfoArray;
    int i, arraySize;
    xmlrpc_value *infoArray, *infoElement;

    if (NULL == extraInfo) {
        srmlogit(STORM_LOG_ERROR, callerName, "Warning: missing ArrayOfTExtraInfo parameter\n");
        return(ENCODE_ERR_MISSING_PARAM);
    }
    arraySize = extraInfo->__sizeextraInfoArray;
    if (arraySize < 1) {
        srmlogit(STORM_LOG_ERROR, callerName, "Warning: missing ArrayOfTExtraInfo parameter\n");
        return(ENCODE_ERR_MISSING_PARAM);
    }

    extraInfoArray = extraInfo->extraInfoArray;
    infoArray = xmlrpc_array_new(env_addr);
    XMLRPC_ASSERT_ENV_OK(env_addr);

    for (i=0; i<arraySize; i++) {

        if (extraInfoArray[i] == NULL) {
            srmlogit(STORM_LOG_DEBUG, callerName, "StorageSystemInfo[%d] is NULL\n", i);
            continue;
        }

        infoElement = xmlrpc_struct_new(env_addr);
        XMLRPC_ASSERT_ENV_OK(env_addr);

        if (extraInfoArray[i]->key == NULL) {
            srmlogit(STORM_LOG_DEBUG, callerName, "StorageSystemInfo[%d] is NULL\n", i);
            continue;
        }
        if(getXMLRPCCheckAscii() && !isASCII(extraInfoArray[i]->key))
        {
            srmlogit(STORM_LOG_ERROR, callerName, "Unable to encode value: %s , it contains non ASCII characters\n", extraInfoArray[i]->key);
            xmlrpc_DECREF(infoElement);
            xmlrpc_DECREF(infoArray);
            return(ENCODE_ERR_ENCODING_ERROR);
        }
        xmlrpc_value* key = xmlrpc_string_new(env_addr, extraInfoArray[i]->key);
        XMLRPC_ASSERT_ENV_OK(env_addr);
        xmlrpc_struct_set_value(env_addr, infoElement, "key", key);
        xmlrpc_DECREF(key);

        if (extraInfoArray[i]->value != NULL)
        {
            if(getXMLRPCCheckAscii() && !isASCII(extraInfoArray[i]->value))
            {
                srmlogit(STORM_LOG_ERROR, callerName, "Unable to encode value: %s , it contains non ASCII characters\n", extraInfoArray[i]->value);
                xmlrpc_DECREF(infoElement);
                xmlrpc_DECREF(infoArray);
                return(ENCODE_ERR_ENCODING_ERROR);
            }
            xmlrpc_value* value = xmlrpc_string_new(env_addr, extraInfoArray[i]->value);
            XMLRPC_ASSERT_ENV_OK(env_addr);
            xmlrpc_struct_set_value(env_addr, infoElement, "value", value);
            xmlrpc_DECREF(value);
        }
        xmlrpc_array_append_item(env_addr, infoArray, infoElement);
        xmlrpc_DECREF(infoElement);
    }

    xmlrpc_struct_set_value(env_addr, xmlStruct, fieldName, infoArray);

    xmlrpc_DECREF(infoArray);

    return(0);
}


/**
 * The encode_charPointer() function encodes the string field (SRM v2.2) into a xml structure
 * @param callerName The name of the caller function (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc functions.
 * @param value The string value, it is a char * in SRM v2.2.
 * @param fieldName The name to assign to xmlrpc field.
 * @param xmlStruct The xmlrpc destination variable.
 */
int encode_string(const char *callerName, xmlrpc_env *env_addr, char *value, char const* fieldName, xmlrpc_value *xmlStruct)
{
    xmlrpc_value *xml_val;
    const char* normalized_surl = NULL;

    if (NULL == value) {
        srmlogit(STORM_LOG_DEBUG, callerName, "Optional parameter %s=NULL\n", fieldName);
        return(ENCODE_ERR_MISSING_PARAM);
    }

    if(getXMLRPCCheckAscii() && !isASCII(value))
    {
        srmlogit(STORM_LOG_ERROR, callerName, "Unable to encode value: %s , it contains non ASCII characters\n", value);
        return(ENCODE_ERR_ENCODING_ERROR);
    }

    if (strcmp(SRM_PARAM_SURL,fieldName) == 0
            || strcmp(SRM_PARAM_fromSURL, fieldName) == 0
            || strcmp(SRM_PARAM_toSURL, fieldName) == 0){
        normalized_surl = storm_normalize_surl(value);
        if (normalized_surl == NULL){

            srmlogit(STORM_LOG_ERROR, callerName, "Unable to encode value: %s. SURL validation failed\n", value);
            return(ENCODE_ERR_ENCODING_ERROR);

        } else {
            xml_val = xmlrpc_string_new(env_addr,normalized_surl);
            XMLRPC_ASSERT_ENV_OK(env_addr);
            free(const_cast<char*>(normalized_surl));
        }
    } else {
        xml_val = xmlrpc_string_new(env_addr, value);
        XMLRPC_ASSERT_ENV_OK(env_addr);
    }

    xmlrpc_struct_set_value(env_addr, xmlStruct, fieldName, xml_val);
    xmlrpc_DECREF(xml_val);
    if (env_addr->fault_occurred) {
        srmlogit(STORM_LOG_ERROR, callerName, "Error inserting string field: %s [%d]\n", env_addr->fault_string, env_addr->fault_code);
        return(ENCODE_ERR_ENCODING_ERROR);
    }

    srmlogit(STORM_LOG_DEBUG, callerName, "%s=%s\n", fieldName, value);
    return(0);
}



/**
 * The encode_transferParameters() function encodes a struct ns1__TTransferParameters * into a xml structure
 * @param callerName The name of the caller function (for log messages).
 * @param env_addr The address of the environment variable used for xmlrpc functions.
 * @param soap The gSOAP structure
 * @param transferParameters The transferParameters structure.
 * @param xmlStruct The xmlrpc destination variable.
 */
int encode_TTransferParameters(const char *callerName,
        xmlrpc_env *env_addr,
        struct ns1__TTransferParameters *transferParameters,
        char const* fieldName,
        xmlrpc_value *xmlStruct)
{
    xmlrpc_value *xml_transferParametersStruct;

    if (NULL == transferParameters) {
        srmlogit(STORM_LOG_DEBUG, callerName, "Warning: missing transferParameters parameter\n");
        return(ENCODE_ERR_MISSING_PARAM);
    }

    xml_transferParametersStruct = xmlrpc_struct_new(env_addr);
    XMLRPC_ASSERT_ENV_OK(env_addr);

    if (transferParameters->accessPattern != NULL)
        encode_int(callerName, env_addr, (int *) transferParameters->accessPattern, "accessPattern", xml_transferParametersStruct);

    if (transferParameters->connectionType != NULL)
        encode_int(callerName, env_addr, (int *) transferParameters->connectionType, "connectionType", xml_transferParametersStruct);

    encode_arrayOfString(callerName, env_addr, transferParameters->arrayOfClientNetworks, "arrayOfClientNetworks", xml_transferParametersStruct);
    encode_arrayOfString(callerName, env_addr, transferParameters->arrayOfTransferProtocols, "arrayOfTransferProtocols", xml_transferParametersStruct);

    xmlrpc_struct_set_value(env_addr, xmlStruct, fieldName, xml_transferParametersStruct);
    return(0);
}
