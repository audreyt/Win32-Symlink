#ifdef __cplusplus
extern "C" {
#endif
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#ifdef __cplusplus
}
#endif

#include "pgsymlink.c"

MODULE = Win32::Symlink		PACKAGE = Win32::Symlink		

int
symlink(oldpath, newpath)
    const char * oldpath
    const char * newpath
    CODE:
	if (pgsymlink(oldpath, newpath) == -1) {
	    RETVAL = 0;
	}
	else {
	    RETVAL = 1;
	}
    OUTPUT:
        RETVAL
