#include <u.h>
#include <libc.h>
#include <ctype.h>

#define TIMEOUT 10000	// wait 10 seconds. This should be defined to "10" for Linux (see alarm(2))

enum
{
	BufSize = 32*1024,
	MaxLineLen = 1024,
};

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


void
handler(int fd)
{
	char buffer[MaxLineLen] = {0};

	print("new connection\n");

	alarm(TIMEOUT);
	do {
		Readline(fd, buffer, MaxLineLen - 1);
		Trim(buffer);
		print("read: %s\n", buffer);
	} while (1);
	alarm(0);
}

// This started life as a straight rip-off from dial(2)
void
main()
{
	int dfd, acfd, lcfd;
	char adir[40], ldir[40];
	int n;
	char buf[256];

	acfd = announce("tcp!*!8080", adir);
	if (acfd < 0)
		exits("announce failed");

	for (;;) {
		lcfd = listen(adir, ldir);
		if (lcfd < 0)
			exits("listen failed");

		switch (fork()) {
		case -1:
			perror("fork failed");
			close(lcfd);
			break;
		case 0:
			dfd = accept(lcfd, ldir);
			if (dfd < 0)
				exits("accept failed");

			handler(dfd);
			/* echo until EOF */
			//while((n = read(dfd, buf, sizeof(buf))) > 0)
			//	write(dfd, buf, n);
			exits(0);
		default:
			close(lcfd);
			break;
		}
	}
}
