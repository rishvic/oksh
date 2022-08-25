AM_CPPFLAGS = -I$(top_srcdir)/lib -I../lib
AM_CFLAGS = $(WARN_CFLAGS)

bin_PROGRAMS = rish

rish_SOURCES = main.c main.h
rish_CFLAGS = $(AM_CFLAGS) $(READLINE_CFLAGS)
rish_LDADD = $(READLINE_LIBS)
