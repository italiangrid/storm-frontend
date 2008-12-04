/*
 * $Id$
 */

#include <sys/types.h>
#include "storm_limits.h"
#include "storm_functions.h"
#include "Cpwd.h"
#include "cgsi_plugin.h"
#include "srm_server.h"
#include "stdsoap2.h"

int get_supported_protocols(char ***sup_proto) {
    int nb_supported_protocols = 0;
    int c;
    int i;
    struct srm_dbfd dbfd;
    int nbprots = 0;
    int protlen = 10;
    char ** sup_protocols;
    DBLISTPTR dblistptr;

    /* Connect to the database if not done yet */

    if (storm_opendb(db_srvr, db_user, db_pwd, &dbfd) < 0) {
        return -1;
    }

    if (NULL == sup_proto) {
        srmlogit("get_supported_protocols", "sup_proto argument is a NULL pointer!\n");
        return -1;
    }

    nb_supported_protocols = storm_list_protocol(&dbfd, NULL, 0, 0, NULL);

    if (nb_supported_protocols == 0) {
        srmlogit("get_supported_protocols", "No protocols supported");
        return -1;
    } else if (nb_supported_protocols < 0) {
        srmlogit("get_supported_protocols", "Error in storm_list_protocol: %d",
                nb_supported_protocols);
        return -1;
    }
    nbprots = nb_supported_protocols;

    sup_protocols = calloc(nbprots, sizeof(char *));
    if (NULL == sup_protocols) {
        srmlogit("get_supported_protocols", "Unable to calloc() an array of lenght %d", nbprots);
        return -1;
    }

    for (i = 0; i < nbprots; i++) {
        sup_protocols[i] = calloc(protlen, sizeof(char));
        if (NULL == sup_protocols[i]) {
            srmlogit("get_supported_protocols", "Unable to calloc() an array of lenght %d", protlen);
            return -1;
        }
    }

    i = storm_list_protocol(&dbfd, sup_protocols, nbprots, protlen, NULL);
    if (i < 0) {
        srmlogit("get_supported_protocols", "Error in storm_list_protocol: %d", i);
        return -1;
    }

    storm_closedb(&dbfd);
    *sup_proto = sup_protocols;
    return (nb_supported_protocols);
}

