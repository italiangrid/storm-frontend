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
 
 #ifdef GSI_PLUGINS
#include "cgsi_plugin.h"
#endif
#include "srmv2H.h"
#include "storm_util.h"
#include "srmSoapBinding.nsmap"
 
int main(int argc,char ** argv)
{
    struct ns1__srmPingRequest req;
    struct ns1__srmPingResponse_ rep;
    struct soap soap;
    char *srm_endpoint;
    int flags;
    
    if (argc < 2) {
        fprintf (stderr, "Usage: %s <endpoint>\n\n", argv[0]);
        exit (1);
    }
    
    /* Get endpoint from command line argument */
    srm_endpoint = argv[1];
    
    soap_init (&soap);
#ifdef GSI_PLUGINS
    flags = CGSI_OPT_DISABLE_NAME_CHECK | CGSI_OPT_DELEG_FLAG;
    soap_register_plugin_arg(&soap, client_cgsi_plugin, &flags);
#endif

    req.authorizationID = NULL;
    
    if (soap_call_ns1__srmPing(&soap, srm_endpoint, "Ping", &req, &rep)) {
        soap_print_fault(&soap, stderr);
        soap_print_fault_location(&soap, stderr);
        soap_end(&soap);
        exit(1);
    }
    
    if (rep.srmPingResponse->versionInfo == NULL)
        printf("No information returned\n\n");
    else 
        printf("SRM version: %s\n\n", rep.srmPingResponse->versionInfo);
    
    soap_end(&soap);
    return(0);
}