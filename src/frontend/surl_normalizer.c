/*
 * surl_normalizer.c
 *
 *  Created on: Oct 15, 2013
 *      Author: andrea
 */

#include "surl_normalizer.h"
#include <string.h>
#include <stdlib.h>

char* storm_get_normalized_surl(const char* surl) {

    static const char* QUERY_PATTERN = "/srm/managerv2?SFN=";
    const size_t QUERY_PATTERN_SIZE = strlen(QUERY_PATTERN);

    if (surl == NULL)
        return NULL;

    const char* query_substr = strstr(surl, QUERY_PATTERN);

    if (query_substr == NULL)
        return NULL;

    size_t prefix_size = query_substr - surl;
    size_t dest_size = strlen(surl) - QUERY_PATTERN_SIZE + 1;

    char *dest = (char*) malloc(dest_size);

    memset(dest,0,dest_size);
    memcpy(dest, surl, prefix_size);

    memcpy(dest + prefix_size,
            query_substr + QUERY_PATTERN_SIZE,
            strlen(query_substr+ QUERY_PATTERN_SIZE));

    return dest;
}
