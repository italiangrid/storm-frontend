#include "Monitoring.hpp"
#include "srmv2H.h"

#include <ctime>

extern "C" int ns1__srmPing_impl(struct soap* soap, struct ns1__srmPingRequest *req,
        struct ns1__srmPingResponse_ *rep);

int ns1__srmPing(struct soap* soap, struct ns1__srmPingRequest *req, struct ns1__srmPingResponse_ *rep) {

    clock_t start_time = clock();

    int ret_val = ns1__srmPing_impl(soap, req, rep);

    clock_t end_time = clock();

    float et = (end_time - start_time) / CLOCKS_PER_SEC;

    if (ret_val != SOAP_OK) {
        storm::Monitoring::getInstance()->notifyPingCompleted(-1, false);
    } else {
        storm::Monitoring::getInstance()->notifyPingCompleted(et, true);
    }

    return ret_val;
}
