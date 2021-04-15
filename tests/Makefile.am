
METASOURCES=AUTO
check_PROGRAMS=epsp-parser-test minecraft-test base58test ripeencode changebasetest string_list_test chainCookiesTest random-test
TESTS=test.sh xfail.sh $(check_PROGRAMS)
XFAIL_TESTS=xfail.sh
dist_check_DATA=epsp-packet-sample-fail.txt epsp-packet-sample-success.txt seeds.txt
EXTRA_DIST=test.sh xfail.sh

AM_CPPFLAGS = -I$(top_srcdir)/include

chainCookiesTest_LDFLAGS=-lm
chainCookiesTest_SOURCES=chainCookiesTest.c
chainCookiesTest_LDADD=$(top_builddir)/src/libstudy.la
epsp_parser_test_SOURCES=epsp-parser-test.c
epsp_parser_test_LDADD=$(top_builddir)/src/libstudy.la
minecraft_test_LDADD=$(top_builddir)/src/libstudy.la
minecraft_test_LDFLAGS=-static
base58test_LDADD=$(top_builddir)/src/libstudy.la
base58test_SOURCES=base58test.c
ripeencode_LDADD=$(top_builddir)/src/libstudy.la
ripeencode_SOURCES=ripeencode.c
changebasetest_LDADD=$(top_builddir)/src/libstudy.la
changebasetest_SOURCES=changebasetest.c
string_list_test_LDADD=$(top_builddir)/src/libstudy.la
string_list_test_SOURCES=string_list_test.c
random_test_LDADD=$(top_builddir)/src/libstudy.la
