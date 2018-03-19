#ifndef REQUEST_ID_HTTP_
#define REQUEST_ID_HTTP_

#include <string>

namespace storm {

    const char* get_request_id();
    void set_request_id();
    void clear_request_id();
}

#endif
