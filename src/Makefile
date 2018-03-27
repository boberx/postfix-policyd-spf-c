CC=gcc
CFLAGS=-Wall
LIBS= -lspf2 -lpthread -lnsl -lresolv
BINDIR=./bin
BINFILE=postfix-policyd-spf-c

main: dir ps_white ps_read_request ps_other ps_white_test ps_white_write ps_white_read ps_getopt
	$(CC) $(CFLAGS) $(LIBS) \
	ps_main.c \
	${BINDIR}/ps_white.o \
	${BINDIR}/ps_read_request.o \
	${BINDIR}/ps_other.o \
	${BINDIR}/ps_white_test.o \
	${BINDIR}/ps_white_write.o \
	${BINDIR}/ps_white_read.o \
	${BINDIR}/ps_getopt.o \
	-o ${BINDIR}/${BINFILE}

ps_white: dir
	$(CC) $(CFLAGS) $(INCLUDE) -c -o ${BINDIR}/ps_white.o ps_white.c

ps_getopt: dir
	$(CC) $(CFLAGS) $(INCLUDE) -c -o ${BINDIR}/ps_getopt.o ps_getopt.c

ps_white_test: dir
	$(CC) $(CFLAGS) $(INCLUDE) -c -o ${BINDIR}/ps_white_test.o ps_white_test.c

ps_read_request: dir
	$(CC) $(CFLAGS) $(INCLUDE) -c -o ${BINDIR}/ps_read_request.o ps_read_request.c

ps_other: dir
	$(CC) $(CFLAGS) $(INCLUDE) -c -o ${BINDIR}/ps_other.o ps_other.c

ps_white_write: dir
	$(CC) $(CFLAGS) $(INCLUDE) -c -o ${BINDIR}/ps_white_write.o ps_white_write.c

ps_white_read: dir
	$(CC) $(CFLAGS) $(INCLUDE) -c -o ${BINDIR}/ps_white_read.o ps_white_read.c

dir:
	mkdir -p ${BINDIR}

clean:
	rm -rf ${BINDIR}

install: main
	cp ${BINDIR}/${BINFILE} /usr/sbin/
