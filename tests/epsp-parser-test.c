
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>
#include <CUnit/Console.h>
#include <epsp-parser.h>
#include <errno.h>
#include <limits.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string-array.h>
#include <string.h>
#include "simpletestcases.h"

/*
 *
  https://p2pquake.github.io/epsp-specifications/epsp-specifications.html
  初期化
  mainloop
    接続受け付け
  heartbeat loop
 */
void print_reg_error(int errorcode, regex_t *buf)
{
    size_t len = regerror(errorcode, buf, NULL, 0);
    char *msg = (char *)calloc(len, sizeof(char));
    regerror(errorcode, buf, msg, len);
    printf("%s\n", msg);
    free(msg);
}

/*
 * 文字列の配列を返す。テストデータファイルの改行コードはCRLFにすること。
 */
int main(int argc, char *argv[])
{
    CU_initialize_registry();
    CU_pSuite testSuite = CU_add_suite("epsp parser test", NULL, NULL);

    // CU_add_test(testSuite, "Fail", fail);
    CU_add_test(testSuite, "Pass", pass);

    // CU_automated_run_tests();
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    int ret = CU_get_number_of_failures();
    // CU_console_run_tests();
    CU_cleanup_registry();
    return ret;
}
