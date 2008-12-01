/*
 * $Id$
 */


#ifndef _CGRP_H
#define _CGRP_H

#include <osdep.h>
#include <grp.h>
#include <sys/types.h>

EXTERN_C struct group DLL_DECL *Cgetgrnam _PROTO((CONST char *));
EXTERN_C struct group DLL_DECL *Cgetgrgid _PROTO((gid_t));

#endif /* _CGRP_H */

