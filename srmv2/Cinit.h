/*
 * $Id$
 */

#ifndef _CINIT_H
#define _CINIT_H

#include "osdep.h"

	/* structure to be used with Cinitdaemon()/Cinitservice() */

struct main_args {
	int	argc;
	char	**argv;
};

EXTERN_C int Cinitdaemon _PROTO((char *, void (*) _PROTO((int))));

#endif
