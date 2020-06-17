#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libstudy], [1.0.0-alpha], [teruterubouzu1024@gmail.com])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_HEADERS([src/config.h])
AC_CANONICAL_HOST
AC_CONFIG_MACRO_DIRS([m4])
AM_GNU_GETTEXT_INTL_SUBDIR
AM_PROG_LIBTOOL
LT_INIT

# Checks for programs.
AC_ENABLE_SHARED

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
m4_ifdef([AM_GNU_GETTEXT], [AM_GNU_GETTEXT([external])],[AC_MSG_FAILURE([could not find lib gettext])])

AC_CONFIG_FILES([Makefile
    intl/Makefile
    po/Makefile.in
    src/Makefile
])
AC_OUTPUT
