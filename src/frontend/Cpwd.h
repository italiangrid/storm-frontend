/*
 * $Id$
 */


#ifndef _CPWD_H
#define _CPWD_H

#include <osdep.h>
#include <pwd.h>

EXTERN_C struct passwd DLL_DECL *Cgetpwnam _PROTO((CONST char *));
EXTERN_C struct passwd DLL_DECL *Cgetpwuid _PROTO((uid_t));

#endif /* _CPWD_H */

