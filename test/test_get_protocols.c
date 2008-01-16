#include <stdio.h>
#include "storm_util.h"
#include "storm_limits.h"
#include "srmv2H.h"
SOAP_NMAC struct Namespace namespaces[] =
{
  {"SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/", "http://www.w3.org/*/soap-envelope"},
  {"SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/", "http://www.w3.org/*/soap-encoding"},
  {"xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance"},
  {"xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema"},
  {"ns1", "urn://StorageResourceManagerV2.1.1"},
  {NULL, NULL}
};


char db_pwd[33];
char db_srvr[33];
char db_user[33];
int jid;
char logfile[ST_MAXPATHLEN+1];

main() {

//char **supported_protocols;
//int nb_supported_protocols;
//int i;
//
//strcpy(db_pwd,"storm");
//strcpy(db_srvr,"testbed006");
//strcpy(db_user,"storm");
//strcpy(logfile,"/var/log/storm/storm.log");
//jid=12345;
//nb_supported_protocols = get_supported_protocols(&supported_protocols);
//
//printf("The number of supported protocols is %i\n", nb_supported_protocols);
//printf("They are :\n");
//for (i=0; i<nb_supported_protocols; i++) {
//    printf("%s\n", supported_protocols[i]);
//}
}

