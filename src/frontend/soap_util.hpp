#ifndef SOAP_UTIL_HPP
#define SOAP_UTIL_HPP
// Copyright (C) 2006 by Antonio Messina <antonio.messina@ictp.it> for the ICTP project EGRID.
/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */


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
        soap_type_t *ptr;
        if(NULL == (ptr = static_cast<soap_type_t*>(soap_malloc(soap, sizeof(soap_type_t)))))
            throw soap_bad_alloc("soap_calloc(soap)");
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
        soap_type_t **ptr;
        if(NULL == (ptr = static_cast<soap_type_t**>(soap_malloc(soap, size * sizeof(soap_type_t*)))))
            throw soap_bad_alloc("soap_calloc(soap,size)");
        memset(ptr, 0, size * sizeof(soap_type_t*));
        return ptr;
    }
}

#endif
