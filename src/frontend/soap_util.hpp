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

#ifndef SOAP_UTIL_HPP
#define SOAP_UTIL_HPP

#include <stdexcept>
#include <stdsoap2.h>

class soap_bad_alloc : public std::bad_alloc {
public: 
    soap_bad_alloc(){};
    soap_bad_alloc(std::string reason){errmsg=reason.c_str();}
    const char *what() const throw() {return errmsg;}
private:
    const char *errmsg;
};

namespace storm {
    /* Template function which allocate a pointer of type soap_type_t
       and do a memset(,0,). Returns the pointer.  Throw ENOMEM in
       case of error.
    */
    template<typename soap_type_t>
    soap_type_t * soap_calloc(struct soap *soap)
    {
        if(NULL == soap)
            throw std::invalid_argument("soap_calloc: soap is a null pointer");
        soap_type_t *ptr = static_cast<soap_type_t*>(soap_malloc(soap,sizeof(soap_type_t)));
        if(ptr == NULL)
        {
        	throw soap_bad_alloc("soap_calloc(soap)");
        }
        memset(ptr, 0, sizeof(soap_type_t));
        return ptr;
    }

    /* Template function which allocate a pointer to an ARRAY of
     * soap_type_t lenghty size */
    template<typename soap_type_t>
    soap_type_t ** soap_calloc(struct soap *soap, const int size)
    {
        if(NULL == soap)
            throw std::invalid_argument("soap_calloc: soap is a null pointer");
        if(size<=0)
            throw std::invalid_argument("soap_calloc(,size): size is negative or null");
        soap_type_t **ptr = static_cast<soap_type_t**>(soap_malloc(soap, size * sizeof(soap_type_t*)));
        if(ptr == NULL)
        {
        	throw soap_bad_alloc("soap_calloc(soap,size)");
        }
        memset(ptr, 0, size * sizeof(soap_type_t*));
        return ptr;
    }
}

#endif
