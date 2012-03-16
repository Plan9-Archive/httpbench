#include <u.h>
#include <libc.h>
#include <ctype.h>
#include <fns.h>
#include <dat.h>

/*  Read a line from a network connection  */
/*
	Shamelessly ripped from Paul Griffith's "webserv", which in turn
	shamelessly rips it off from "UNIX Network Programming by
	W Richard Stevens.
*/

int Readline(int sockd, void *vptr, int maxlen) {
    int n, rc;
    char    c, *buffer;

    buffer = vptr;

    for ( n = 1; n < maxlen; n++ ) {
	if ( (rc = read(sockd, &c, 1)) == 1 ) {
	    *buffer++ = c;
	    if ( c == '\n' )
		break;
	} else if ( rc == 0 ) {
	    if ( n == 1 )
		return 0;
	    else
		break;
	} else {
	    exits("Error in Readline()");
	}
    }

    *buffer = 0;
    return n;
}

/*  Removes trailing whitespace from a string, also shamelessly ripped from Griffith  */

int Trim(char * buffer) {
    int n = strlen(buffer) - 1;

    while ( !isalnum(buffer[n]) && n >= 0 )
	buffer[n--] = '\0';

    return 0;
}
