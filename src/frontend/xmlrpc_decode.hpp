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


#ifndef XMLRPC_DECODE_H_
#define XMLRPC_DECODE_H_

#endif /*XMLRPC_DECODE_H_*/

#include "srmlogit.h"
#include "srmv2H.h"
#include "storm_util.hpp"

#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>

/* Error codes for the dencoding functions */
#define DECODE_ERR_SOAP_MEMORY_ERROR -1
#define DECODE_ERR_NOT_FOUND -2
#define DECODE_ERR_DECODING_ERROR -3

/* Parameters names */
/* see also file: xmlrpc_encode.hpp */
#define SRM_PARAM_arrayOfFileStatuses "arrayOfFileStatuses"
#define SRM_PARAM_requestToken "requestToken"
#define SRM_PARAM_retentionPolicyInfo "retentionPolicyInfo"
#define SRM_PARAM_sizeOfTotalReservedSpace "sizeOfTotalReservedSpace"
#define SRM_PARAM_sizeOfGuaranteedReservedSpace "sizeOfGuaranteedReservedSpace"
#define SRM_PARAM_lifetimeOfReservedSpace "lifetimeOfReservedSpace"
#define SRM_PARAM_spaceToken "spaceToken"
#define SRM_PARAM_owner "owner"
#define SRM_PARAM_totalSize "totalSize"
#define SRM_PARAM_guaranteedSize "guaranteedSize"
#define SRM_PARAM_unusedSize "unusedSize"
#define SRM_PARAM_lifetimeAssigned "lifetimeAssigned"
#define SRM_PARAM_lifetimeLeft "lifetimeLeft"
#define SRM_PARAM_arrayOfSpaceDetails "arrayOfSpaceDetails"
#define SRM_PARAM_status "status"
#define SRM_PARAM_details "details"
#define SRM_PARAM_arrayOfSubPaths "arrayOfSubPaths"
#define SRM_PARAM_type "type"
#define SRM_PARAM_fileLocality "fileLocality"
#define SRM_PARAM_fileStorageType "fileStorageType"
//#define SRM_PARAM_surl "surl"
#define SRM_PARAM_path "path"
#define SRM_PARAM_size "size"
#define SRM_PARAM_createdAtTime "createdAtTime"
#define SRM_PARAM_lastModificationTime "lastModificationTime"
#define SRM_PARAM_arrayOfTSpaceTokens "arrayOfTSpaceTokens"
#define SRM_PARAM_ownerPermission "ownerPermission"
#define SRM_PARAM_groupPermission "groupPermission"
#define SRM_PARAM_otherPermission "otherPermission"
#define SRM_PARAM_checkSumType "checkSumType"
#define SRM_PARAM_checkSumValue "checkSumValue"
#define SRM_PARAM_estimatedProcessingTime "estimatedProcessingTime"
#define SRM_PARAM_fileLifetime "fileLifetime"
#define SRM_PARAM_pinLifetime "pinLifetime"
#define SRM_PARAM_mode "mode"

/* Prototype definitions */
int decode_lifetimeValue(const char *callerName, xmlrpc_env *env_addr, struct soap *soap, int **lifetimeVal, char const* fieldName, xmlrpc_value *xmlStruct);
int decode_ArrayOfTSURLReturnStatus(const char *callerName, xmlrpc_env *env_addr, struct soap *soap, struct ns1__ArrayOfTSURLReturnStatus **arrayOfFileStatuses, char const* fieldName, xmlrpc_value *xmlStruct);
int decode_ArrayOfTSURLLifetimeReturnStatus(const char *callerName, xmlrpc_env *env_addr, struct soap *soap, struct ns1__ArrayOfTSURLLifetimeReturnStatus **arrayOfFileStatuses, char const* fieldName, xmlrpc_value *xmlStruct);
int decode_ArrayOfTExtraInfo(const char *callerName, xmlrpc_env *env_addr, struct soap *soap, struct ns1__ArrayOfTExtraInfo **arrayOfTExtraInfo, char const* fieldName, xmlrpc_value *xmlStruct);
int decode_TRetentionPolicyInfo(const char *callerName, xmlrpc_env *env_addr, struct soap *soap, struct ns1__TRetentionPolicyInfo **retentionPolicyInfo, char const* fieldName, xmlrpc_value *xmlStruct);
int decode_string(const char *callerName, xmlrpc_env *env_addr, struct soap *soap, char **outputString, char const* fieldName, xmlrpc_value *xmlStruct);
int decode_int(const char *callerName, xmlrpc_env *env_addr, struct soap *soap, int **outputInt, char const* fieldName, xmlrpc_value *xmlStruct);
int decode_ULONG64(const char *callerName, xmlrpc_env *env_addr, struct soap *soap, ULONG64 **outputULONG64, char const* fieldName, xmlrpc_value *xmlStruct);
int decode_ArrayOfString(const char *callerName, xmlrpc_env *env_addr, struct soap *soap, struct ns1__ArrayOfString **arrayOfString, char const* fieldName, xmlrpc_value *xmlStruct);
int decode_TPermissionMode(const char *callerName, xmlrpc_env *env_addr, struct soap *soap, enum ns1__TPermissionMode **permissionMode, char const* fieldName, xmlrpc_value *xmlStruct);
int decode_ArrayOfTMetaDataSpace(const char *callerName, xmlrpc_env *env_addr, struct soap *soap, struct ns1__ArrayOfTMetaDataSpace **arrayOfSpaceDetails, char const* fieldName, xmlrpc_value *xmlStruct);
int decode_TUserPermission(const char *callerName, xmlrpc_env *env_addr, struct soap *soap, struct ns1__TUserPermission **userPermission, char const* fieldName, xmlrpc_value *xmlStruct);
int decode_TGroupPermission(const char *callerName, xmlrpc_env *env_addr, struct soap *soap, struct ns1__TGroupPermission **groupPermission, char const* fieldName, xmlrpc_value *xmlStruct);
int decode_TReturnStatus(const char *callerName, xmlrpc_env *env_addr, struct soap *soap, struct ns1__TReturnStatus **status, char const* fieldName, xmlrpc_value *xmlStruct);
int decode_ArrayOfTMetaDataPathDetail(const char *callerName, xmlrpc_env *env_addr, struct soap *soap, struct ns1__ArrayOfTMetaDataPathDetail **arrayOfTMetaData, char const* fieldName, xmlrpc_value *xmlStruct);

int decode_globalTReturnStatus(const char *callerName, xmlrpc_env *env_addr, struct soap *soap, struct ns1__TReturnStatus *returnStatus, xmlrpc_value *xmlStruct);
