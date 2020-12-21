#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libstudy],[1.0.2],[teruterubouzu1024@gmail.com])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/study.c])
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_HOST
AC_CONFIG_MACRO_DIRS([m4])
AC_USE_SYSTEM_EXTENSIONS
AM_PROG_AR
LT_INIT

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([crypto], [main], [], [AC_MSG_FAILURE([could not find lib crypto])])
AC_CHECK_LIB([xmlrpc], [main], [], [AC_MSG_FAILURE([could not find lib xmlrpc])])
AC_CHECK_LIB([xmlrpc_client], [main], [], [AC_MSG_FAILURE([could not find lib xmlrpc_client])])
AC_CHECK_LIB([xmlrpc_server], [main], [], [AC_MSG_FAILURE([could not find lib xmlrpc_server])])
AC_CHECK_LIB([cunit], [main], [], [AC_MSG_FAILURE([could not find lib cunit])])

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([argz.h fcntl.h float.h inttypes.h langinfo.h libintl.h limits.h locale.h malloc.h stddef.h stdint.h stdio_ext.h stdlib.h string.h sys/param.h unistd.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.8])
AC_FUNC_MALLOC
AC_FUNC_MBRTOWC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([__argz_count __argz_next __argz_stringify getcwd localeconv mblen mbrlen memchr mempcpy memset munmap nl_langinfo setlocale stpcpy strcasecmp strchr strcspn strdup strdupa strrchr strstr strtoul])

AC_CONFIG_FILES([Makefile
    include/Makefile
    po/Makefile.in
    tests/Makefile
    src/Makefile
])
AC_OUTPUT
