#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "dat.h"
#include "fns.h"

int sock;

void
sigcatch(int blah)
{
        printf("caught interrupt, dying\n");
        close(sock);
}

void
main()
{
	int fd;
	int on = 1;
	struct sockaddr_in servaddr;

   	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		printf("something wicked happened\n");
		exit(-1);
	}
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	signal(SIGINT, (void*)sigcatch);

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(8080);

	bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr));

	listen(sock, 100);

	for (;;) {
		fd = accept(sock, NULL, NULL);
		if (fd < 0) {
			printf("something wicked happened\n");
			exit(-1);
		}

		switch(fork()) {
		case -1:
			printf("fork failed\n");
			close(fd);
			break;
		case 0:
			handler(fd);
			exit(0);
		default:
			break;
		}
	}
	close(sock);
}

