
METASOURCES=AUTO
check_PROGRAMS=epsp-parser-test minecraft-test
TESTS=test.sh xfail.sh epsp-parser-test minecraft-test
XFAIL_TESTS=xfail.sh
EXTRA_DIST=test.sh xfail.sh epsp-packet-sample-fail.txt epsp-packet-sample-success.txt

AM_CPPFLAGS = -I$(top_srcdir)/include

epsp_parser_test_SOURCES=epsp-parser-test.c
epsp_parser_test_LDADD=../src/libstudy.la
minecraft_test_LDADD=../src/libstudy.la
