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
    struct ns1__srmGetTransferProtocolsRequest req;
    struct ns1__srmGetTransferProtocolsResponse_ rep;
    struct ns1__srmGetTransferProtocolsResponse *repp;
    struct soap soap;
    char *srm_endpoint, *transferProtocol;
    int i, flags;
    
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
    
    if (soap_call_ns1__srmGetTransferProtocols(&soap, srm_endpoint, "GetTransferProtocols", &req, &rep)) {
        soap_print_fault(&soap, stderr);
        soap_print_fault_location(&soap, stderr);
        soap_end(&soap);
        exit(1);
    }
    
    repp = rep.srmGetTransferProtocolsResponse;
    
    if (repp->returnStatus == NULL) {
        printf("Error: no information returned\n\n");
        soap_end(&soap);
        exit(1);
    }
    
    printf("Request status: code=[%d]", repp->returnStatus->statusCode);
    if (repp->returnStatus->explanation != NULL)
        printf(" explanation=[%s]\n", repp->returnStatus->explanation);
    else
        printf("\n");
        
    if (repp->protocolInfo != NULL) {
        printf("List of supported protocols:\n");
        for (i=0; i<repp->protocolInfo->__sizeprotocolArray; i++) {
            transferProtocol = repp->protocolInfo->protocolArray[i]->transferProtocol;
            if (transferProtocol != NULL)
                printf("[%d]: %s\n", i, transferProtocol);
            else 
                printf("[%d]: NULL\n", i);
        }
    }
    printf("\n");
    
    soap_end(&soap);
    return(0);
}