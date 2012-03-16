#ifdef PLAN9
#include <u.h>
#include <libc.h>
#include <ctype.h>
#else
#include <stdio.h>
#include <linux.h>
#endif

#include <dat.h>
#include <fns.h>

/*  Read a line from a network connection  */
/*
	Shamelessly ripped from Paul Griffith's "webserv", which in turn
	shamelessly rips it off from "UNIX Network Programming by
	W Richard Stevens.
*/

int 
readline(int sockd, void *vptr, int maxlen) 
{
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

int writeline(int sockd, const void *vptr, int n) {
    int      nleft;
    int     nwritten;
    const char *buffer;

    buffer = vptr;
    nleft  = n;

    while ( nleft > 0 ) {
	if ( (nwritten = write(sockd, buffer, nleft)) <= 0 ) {
		exits("Error in writeline()");
	}
	nleft  -= nwritten;
	buffer += nwritten;
    }

    return n;
}

/*  Removes trailing whitespace from a string, also shamelessly ripped from Griffith  */

int 
trim(char * buffer) 
{
    int n = strlen(buffer) - 1;

    while ( !isalnum(buffer[n]) && n >= 0 )
	buffer[n--] = '\0';

    return 0;
}

/*  Cleans up url-encoded string... guess where this function originates ☺  */
	
void 
cleanURL(char * buffer) 
{
    char asciinum[3] = {0};
    int i = 0, c;
    
    while ( buffer[i] ) {
	if ( buffer[i] == '+' )
	    buffer[i] = ' ';
	else if ( buffer[i] == '%' ) {
	    asciinum[0] = buffer[i+1];
	    asciinum[1] = buffer[i+2];
	    buffer[i] = strtol(asciinum, 0, 16);
	    c = i+1;
	    do {
		buffer[c] = buffer[c+2];
	    } while ( buffer[2+(c++)] );
	}
	++i;
    }
}