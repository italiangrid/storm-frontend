#include "Monitoring.hpp"
#include "srmv2H.h"

extern "C" int ns1__srmPing_impl(struct soap* soap, struct ns1__srmPingRequest *req, struct ns1__srmPingResponse_ *rep);

int ns1__srmPing(struct soap* soap, struct ns1__srmPingRequest *req, struct ns1__srmPingResponse_ *rep)
{
    int ret_val = ns1__srmPing_impl(soap, req, rep);

    storm::Monitoring::getInstance()->notifyPingCompleted(-1, true);

    return ret_val;
}
