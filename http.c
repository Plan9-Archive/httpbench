#ifdef PLAN9
#include <u.h>
#include <libc.h>
#include <ctype.h>
#else
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "linux.h"
#endif

#include "dat.h"
#include "fns.h"

int
parse(char *buf, Request *req)
{
	static int first_header = 1; // interesting
	char      *endptr;
	int        len;

	if (first_header) {
		if (!strncmp(buf, "GET ", 4)) {
			req->method = GET;
			buf += 4;
		} else {
			req->method = UNSUPPORTED;
			req->status = 501;
		}

		while (*buf && isspace(*buf))
			buf++;

		endptr = strchr(buf, ' ');
		if (endptr == 0)
			len = strlen(buf);
		else
			len = endptr - buf;
		if (!len) {
			req->status = 400;
			return -1;
		}

		req->resource = calloc(len+1, sizeof(char));
		strncpy(req->resource, buf, len);

		if (strstr(buf, "HTTP/"))
			req->type = FULL;
		else
			req->type = SIMPLE;

		first_header = 0;
		return 0;
	}
	return 0;
}

void
reqinit(Request *req)
{
	req->status = 200;
}

void
initresourcepath()
{
	resourcepath = calloc(MaxLineLen, sizeof(char));
}

int
openresource(Request *req)
{
	cleanURL(req->resource);
	strcat(resourcepath, root);
	strcat(resourcepath, req->resource);
	return open(resourcepath, OREAD);
}

void
outputheaders(int fd, Request *req)
{
	char buf[100];
	sprint(buf, "HTTP/1.0 %d OK\r\r", req->status);
	writeline(fd, buf, strlen(buf));

	writeline(fd, "Server: httpbench\r\n", strlen("Server: httpbench\r\n"));
	writeline(fd, "Content-Type: text/html\r\n", 25);
	writeline(fd, "\r\n", 2);
}

int
sendresource(int conn, int resource)
{
    char c[1024];
    int  i;

    while ( (i = read(resource, c, 1024)) ) {
	if ( i < 0 )
	    exits("Error reading from file.");
	if ( write(conn, c, i) < 1 )
	    exits("Error sending file.");
    }

    return 0;
}

int
senderror(int conn, Request* reqinfo)
{
    
    char buffer[100];

    sprint(buffer, "<HTML>\n<HEAD>\n<TITLE>Server Error %d</TITLE>\n"
	            "</HEAD>\n\n", reqinfo->status);
    writeline(conn, buffer, strlen(buffer));

    sprint(buffer, "<BODY>\n<H1>Server Error %d</H1>\n", reqinfo->status);
    writeline(conn, buffer, strlen(buffer));

    sprint(buffer, "<P>The request could not be completed.</P>\n"
	            "</BODY>\n</HTML>\n");
    writeline(conn, buffer, strlen(buffer));

    return 0;

}

void
handler(int fd)
{
	char buffer[MaxLineLen] = {0};
	Request req;
	int rfd; // resource fd

	reqinit(&req);

	alarm(TIMEOUT);
	do {
		readline(fd, buffer, MaxLineLen - 1);
		trim(buffer);
		if (buffer[0] == '\0')
			break;
		if (parse(buffer, &req))
			break;
	} while (req.type != SIMPLE);
	alarm(0);
	rfd = openresource(&req); // make sure the resource exists
	if (rfd < 0)
		req.status = 404;
	if (req.type == FULL)
		outputheaders(fd, &req);

	if (req.status == 200) {
		if (sendresource(fd, rfd))
			exits("something bad happened while sending the resource\n");
	} else {
		senderror(fd, &req);
	}
	close(fd);
	exits(0);
}
