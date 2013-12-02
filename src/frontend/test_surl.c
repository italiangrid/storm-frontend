#include "surl_normalizer.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>

int main(int argc, const char *argv[])
{
    const char* surls[] = {
        "srm://host.ciccio:8444/test//palla",
        "srm://host.ciccio/manager",
        "srm://host.com:8009//srm/managerv2?SFN=/test//ciccio",
        "srm://atlasse.lnf.infn.it/dpm/lnf.infn.it/home/atlas/atlasdatadisk/rucio/mc12_8TeV/52/d4/NTUP_TRUTH.01369588._000098.root.1"
    };

    const char* expected[] = {
         "srm://host.ciccio:8444/test/palla",
         "srm://host.ciccio/manager",
         "srm://host.com:8009/test/ciccio",
         "srm://atlasse.lnf.infn.it/dpm/lnf.infn.it/home/atlas/atlasdatadisk/rucio/mc12_8TeV/52/d4/NTUP_TRUTH.01369588._000098.root.1"
    };

    const char* invalid_surls[] = {
        "invalid surl",
        "http://www.google.com",
        "https://www.cicciopalla.com",
        "",
        0
    };

    int const surls_size = sizeof(surls) / sizeof(surls[0]);
    int const invalid_surls_size = sizeof(invalid_surls) / sizeof(invalid_surls[0]);
    int index=0;

    for(; index < surls_size; index++){

        char * s = storm_normalize_surl(surls[index]);
        if (s == NULL){
            printf("Got null from validation, unexpected!\n");
        }else if (strcmp(s,expected[index])){
            printf("Test %d failed. Expected: %s Got: %s\n", index , expected[index], s);
        }else{
            printf("Test %d: success.\n", index);
        }
        free(s);

    }

    for(index = 0; index < invalid_surls_size; index++)
    {
        char *s = storm_normalize_surl(invalid_surls[index]);
        if (s != NULL){
            printf("Invalid surl test %d failed. Expected: NULL Got: %s\n", index, s);
            free(s);
        }else
            printf("Invalid surl test %d: success.\n", index);
    }

    return 0;
}

