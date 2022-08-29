#                                               -*- Autoconf -*-
AC_PREREQ([2.71])
AC_INIT([oksh], [0.1.2], [https://github.com/Colocasian/oksh/issues])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC
gl_EARLY

# Checks for libraries.
PKG_CHECK_MODULES([READLINE], [readline])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

gl_WARN_ADD([-Wall], [WARN_CFLAGS])
gl_WARN_ADD([-Wextra], [WARN_CFLAGS])
gl_WARN_ADD([-Wpedantic], [WARN_CFLAGS])

gl_INIT

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 lib/Makefile])
AC_OUTPUT
