
#include "Monitoring.hpp"
#include "srmv2H.h"

#include "boost/date_time/posix_time/posix_time.hpp"

extern "C" int ns1__srmPing_impl(struct soap* soap, struct ns1__srmPingRequest *req,
        struct ns1__srmPingResponse_ *rep);

using namespace boost::posix_time;

int ns1__srmPing(struct soap* soap, struct ns1__srmPingRequest *req, struct ns1__srmPingResponse_ *rep) {

    ptime start_time = microsec_clock::local_time();

    int ret_val = ns1__srmPing_impl(soap, req, rep);

    ptime end_time = microsec_clock::local_time();

    time_duration et = (end_time - start_time);

    if (ret_val != SOAP_OK) {
        storm::Monitoring::getInstance()->notifyPingCompleted(-1, false);
    } else {
        storm::Monitoring::getInstance()->notifyPingCompleted(et.total_milliseconds(), true);
    }

    return ret_val;
}

