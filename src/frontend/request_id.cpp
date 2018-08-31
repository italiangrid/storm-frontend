#include "request_id.hpp"
#include <boost/thread.hpp>
#include <boost/thread/tss.hpp>
#include <uuid/uuid.h>

static boost::thread_specific_ptr<std::string> request_id;

const char*
storm::get_request_id() {

    if (request_id.get() == NULL) {
	return NULL;
    }

    return request_id->c_str();
}

void 
storm::set_request_id() {
    char buf[37];

    uuid_t id;
    uuid_generate(id);
    uuid_unparse_lower(id,buf);

    std::string* req_id_p = new std::string(buf);
    request_id.reset(req_id_p);
}

void 
storm::clear_request_id() {
    std::string* s_p = request_id.release();
    delete s_p;
}
