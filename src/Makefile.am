AM_CPPFLAGS = -I$(top_srcdir)/lib -I../lib
AM_CFLAGS = $(WARN_CFLAGS)

bin_PROGRAMS = oksh

oksh_SOURCES = main.c \
               baseio.c \
               baseio.h \
               rlio.c \
               rlio.h \
               runner.c \
               runner.h
oksh_CFLAGS = $(AM_CFLAGS) $(READLINE_CFLAGS)
oksh_LDADD = $(READLINE_LIBS)
