AM_CPPFLAGS = -I$(top_srcdir)/lib -I../lib
AM_CFLAGS = $(WARN_CFLAGS)

bin_PROGRAMS = oksh

oksh_SOURCES = main.c \
               main.h \
               baseio.c \
               baseio.h \
               rlio.c \
               rlio.h \
               reader.c \
               reader.h \
               parser.c \
               parser.h \
               execute.c \
               execute.h \
               runner.c \
               runner.h \
               userinfo.c \
               userinfo.h
oksh_CFLAGS = $(AM_CFLAGS) $(READLINE_CFLAGS)
oksh_LDADD = $(READLINE_LIBS)
