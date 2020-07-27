
METASOURCES=AUTO
check_PROGRAMS=epsp-parser-test minecraft-test base58test ripeencode
TESTS=test.sh xfail.sh epsp-parser-test minecraft-test base58test ripeencode
XFAIL_TESTS=xfail.sh
EXTRA_DIST=test.sh xfail.sh epsp-packet-sample-fail.txt epsp-packet-sample-success.txt seeds.txt

AM_CPPFLAGS = -I$(top_srcdir)/include

epsp_parser_test_SOURCES=epsp-parser-test.c
epsp_parser_test_LDADD=../src/libstudy.la
minecraft_test_LDADD=../src/libstudy.la
minecraft_test_LDFLAGS=-static
base58test_LDADD=$(top_builddir)/src/libstudy.la
base58test_SOURCES=base58test.c
ripeencode_LDADD=$(top_builddir)/src/libstudy.la
ripeencode_SOURCES=ripeencode.c
