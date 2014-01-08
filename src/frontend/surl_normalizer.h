/*
 * surl_normalizer.h
 *
 *  Created on: Oct 15, 2013
 *      Author: andrea
 */

#ifndef SURL_NORMALIZER_H_
#define SURL_NORMALIZER_H_

#ifdef __cplusplus
#include <string>
extern "C" {
#endif

/**
 * Transforms the surl in surl from the query format to the simple
 * format.
 *
 * Returns NULL in case the surl validation fails.
 *
 * N.B. The returned c string needs to be freed by the caller.
 */
char* storm_normalize_surl(const char* surl);

#ifdef __cplusplus
}

namespace storm{

    /**
     * Transforms the input surl from the query format to the simple
     * format.
     *
     * Throws storm::invalid_surl if the input surl is not a valid
     * surl.
     *
     **/
    std::string normalize_surl(std::string surl);
}
#endif
#endif /* SURL_NORMALIZER_H_ */
