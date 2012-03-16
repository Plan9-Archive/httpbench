#include <u.h>
#include <libc.h>
#include <ctype.h>
#include <fns.h>
#include <dat.h>

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
	print("done\n");
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
