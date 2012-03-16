#include <u.h>
#include <libc.h>
#include <dat.h>
#include <fns.h>

// This started life as a straight rip-off from dial(2)
void
main()
{
	int dfd, acfd, lcfd;
	char adir[40], ldir[40];

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
			exits(0); // should never get here
		default:
			close(lcfd);
			break;
		}
	}
}
