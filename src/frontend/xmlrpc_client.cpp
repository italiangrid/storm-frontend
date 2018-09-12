#include "xmlrpc_client.hpp"
#include <xmlrpc-c/client.h>
#include <boost/thread.hpp>
#include <boost/thread/tss.hpp>
#include <cassert>
#include "FrontendConfiguration.hpp"
#include "srmlogit.h"

typedef boost::shared_ptr<xmlrpc_client> XmlRpcClientPtr;

static boost::thread_specific_ptr<xmlrpc_client> client(&xmlrpc_client_destroy);

static xmlrpc_client *create_client()
{
    xmlrpc_client *result = NULL;

    FrontendConfiguration *configuration = FrontendConfiguration::getInstance();

    std::string storm_ua_token("STORM/" + configuration->getXMLRPCToken());

    xmlrpc_env env;
    xmlrpc_env_init(&env);
    struct xmlrpc_clientparms client_params;
    struct xmlrpc_curl_xportparms curl_params;

    memset(&curl_params, 0, sizeof(curl_params));

    // Ugly hack to encode token in User-Agent HTTP header
    // as xmlrpc doesnt allow us to access CURL object and
    // set a decent header ourselves.
    curl_params.user_agent = storm_ua_token.c_str();
    curl_params.dont_advertise = 1;

    client_params.transport = "curl";
    client_params.transportparmsP = &curl_params;
    client_params.transportparm_size = XMLRPC_CXPSIZE(dont_advertise);
    xmlrpc_client_create(&env, XMLRPC_CLIENT_NO_FLAGS, //
                         NULL, NULL,                   // name and version, unused
                         &client_params, XMLRPC_CPSIZE(transportparm_size),
                         &result);

    if (env.fault_occurred) {
        srmlogit(STORM_LOG_DEBUG, __func__, env.fault_string);
    }

    xmlrpc_env_clean(&env);

    return result;
}

extern "C" xmlrpc_client *get_xmlrpc_client()
{
    xmlrpc_client *result = client.get();
    if (result == NULL)
    {
        // this is the first time we call this function
        // initialize the pointer

        result = create_client();
        assert(result != NULL);
        client.reset(result);
    }

    return result;
}
