#define TIMEOUT 10000	// wait 10 seconds. This should be defined to "10" for Linux (see alarm(2))

enum
{
	BufSize = 32*1024,
	MaxLineLen = 1024,
};

enum Method
{
	GET,
	UNSUPPORTED
};

enum Type
{
	SIMPLE,
	FULL
};

typedef struct Request {
	enum Method method;
	enum Type type;
	char *referer;
	char *useragent;
	char *resource;
	int status;
} Request;

static char* root = "/tmp";
