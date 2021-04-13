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

#ifndef XML_ENCODE_H_
#define XML_ENCODE_H_

#endif /*XML_ENCODE_H_*/

#include "srmlogit.h"
#include "srmv2H.h"

#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>

/* General encoding constants */
#define NUM_OF_LONG_CHR 64

/* Parameter names */
#define SRM_PARAM_storageSystemInfo "storageSystemInfo"
#define SRM_PARAM_desiredSizeOfTotalSpace "desiredSizeOfTotalSpace"
#define SRM_PARAM_desiredSizeOfGuaranteedSpace "desiredSizeOfGuaranteedSpace"
#define SRM_PARAM_desiredLifetimeOfReservedSpace "desiredLifetimeOfReservedSpace"
#define SRM_PARAM_arrayOfExpectedFileSizes "arrayOfExpectedFileSizes"
#define SRM_PARAM_requestToken "requestToken"
#define SRM_PARAM_arrayOfSpaceTokens "arrayOfSpaceTokens"
#define SRM_PARAM_spaceToken "spaceToken"
#define SRM_PARAM_transferParameters "transferParameters"
#define SRM_PARAM_storageSystemInfo "storageSystemInfo"
#define SRM_PARAM_authorizationID "authorizationID"
#define SRM_PARAM_SURL "surl"
#define SRM_PARAM_fromSURL "fromSURL"
#define SRM_PARAM_toSURL "toSURL"
#define SRM_PARAM_fullDetailedList "fullDetailedList"
#define SRM_PARAM_allLevelRecursive "allLevelRecursive"
#define SRM_PARAM_numOfLevels "numOfLevels"
#define SRM_PARAM_offset "offset"
#define SRM_PARAM_count "count"
#define SRM_PARAM_arrayOfSURLs "arrayOfSURLs"
#define SRM_PARAM_userSpaceTokenDescription "userSpaceTokenDescription"
#define SRM_PARAM_fileLifeTime "fileLifetime"
#define SRM_PARAM_pinLifeTime "pinLifetime"


/* Error codes for the encoding functions */
#define ENCODE_ERR_GENERAL_ERROR -1
#define ENCODE_ERR_ENCODING_ERROR -2
#define ENCODE_ERR_MISSING_PARAM -3


/* Prototype definitions */
int encode_lifetimeValue(const char *callerName, xmlrpc_env *env_addr, int *lifetimeVal, char const* fieldName, xmlrpc_value *xmlStruct);
int encode_arrayOfString(const char *callerName, xmlrpc_env *env_addr, struct ns1__ArrayOfString *arrayOfString, char const* fieldName, xmlrpc_value *xmlStruct);
int encode_arrayOfUnsignedLong(const char *callerName, xmlrpc_env *env_addr, struct ns1__ArrayOfUnsignedLong *arrayOfUnsignedLong, char const* fieldName, xmlrpc_value *xmlStruct);
int encode_int(const char *callerName, xmlrpc_env *env_addr, int *intVal, char const* fieldName, xmlrpc_value *xmlStruct);
int encode_ULONG64(const char *callerName, xmlrpc_env *env_addr, ULONG64 *long64Val, char const* fieldName, xmlrpc_value *xmlStruct);
int encode_retentionPolicyInfo(const char *callerName, xmlrpc_env *env_addr, struct ns1__TRetentionPolicyInfo *retentionPolicyInfo, xmlrpc_value *xmlStruct);
int encode_userSpaceTokenDescription(const char *callerName, xmlrpc_env *env_addr, char *spaceToken, xmlrpc_value *xmlStruct);
int encode_VOMSAttributes(const char *callerName, xmlrpc_env *env_addr, struct soap *soap, char *autohID, xmlrpc_value *xmlStruct);
int encode_ArrayOfAnyURI(const char *callerName, xmlrpc_env *env_addr, struct ns1__ArrayOfAnyURI *arrayOfSURLs, char const* fieldName, xmlrpc_value *xmlStruct);
int encode_ArrayOfTExtraInfo(const char *callerName, xmlrpc_env *env_addr, struct ns1__ArrayOfTExtraInfo *extraInfo, char const* fieldName, xmlrpc_value *xmlStruct);
//int encode_TExtraInfo(const char *callerName, xmlrpc_env *env_addr, xmlrpc_value* infoArray, char* key, char* value);
int encode_string(const char *callerName, xmlrpc_env *env_addr, char *anyURI, char const* fieldName, xmlrpc_value *xmlStruct);
int encode_bool(const char *callerName, xmlrpc_env *env_addr, unsigned int *boolVal, char const* fieldName, xmlrpc_value *xmlStruct);
int encode_TTransferParameters(const char *callerName, xmlrpc_env *env_addr, struct ns1__TTransferParameters *transferParameters, char const* fieldName, xmlrpc_value *xmlStruct);

int call_FrontendConfiguration_getXMLRPCCheckAscii(); 
