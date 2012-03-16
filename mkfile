</$objtype/mkfile

CFLAGS=-FTVw -DPLAN9
TARG=httpbench
OFILES=\
	util.$O\
	http.$O\
	plan9.$O\

HFILES=fns.h\
	dat.h\

BIN=/$objtype/bin
</sys/src/cmd/mkone