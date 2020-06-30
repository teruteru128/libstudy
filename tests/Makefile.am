
METASOURCES=AUTO
check_PROGRAMS=epsp-parser-test
TESTS=test.sh epsp-parser-test
EXTRA_DIST=test.sh epsp-packet-sample-fail.txt epsp-packet-sample-success.txt

AM_CPPFLAGS = -I$(top_srcdir)/include

epsp_parser_test_SOURCES=epsp-parser-test.c
epsp_parser_test_LDADD=../src/libstudy.la
