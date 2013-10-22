/*
 * surl_normalizer.h
 *
 *  Created on: Oct 15, 2013
 *      Author: andrea
 */

#ifndef SURL_NORMALIZER_H_
#define SURL_NORMALIZER_H_

#include <string.h>

/**
 * Transforms the surl in surl from the query format to the simple
 * format.
 *
 * Returns NULL if no conversion has been done or if the input
 * string is NULL.
 *
 * N.B. The returned c string needs to be freed by the caller.
 */
char* storm_get_normalized_surl(const char* surl);

#endif /* SURL_NORMALIZER_H_ */
