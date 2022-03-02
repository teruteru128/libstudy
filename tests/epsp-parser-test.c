
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <CUnit/Automated.h>
#include <epsp-parser.h>
#include <errno.h>
#include <limits.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string-array.h>
#include <string.h>

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

void fail(void) { CU_FAIL(""); }

/*
 * 文字列の配列を返す。テストデータファイルの改行コードはCRLFにすること。
 */
int main(int argc, char *argv[])
{
    CU_initialize_registry();
    CU_pSuite testSuite = CU_add_suite("epsp parser test", NULL, NULL);

    CU_add_test(testSuite, "Fail", fail);

    CU_automated_run_tests();
    // CU_console_run_tests();
    CU_cleanup_registry();
    /*
        char path[PATH_MAX];
        snprintf(path, PATH_MAX, "%s/epsp-packet-sample-success.txt",
                 getenv("srcdir"));
        printf("%d\n", PATH_MAX);
        printf("%s\n", path);
        FILE *in = fopen(path, "r");
        if (!in)
        {
            perror("fopen");
            return EXIT_FAILURE;
        }
        char buf[BUFSIZ];
        printf("%d\n", BUFSIZ);
        size_t len = 0L;
        while (fgets(buf, BUFSIZ, in))
        {
            fprintf(stdout, "%s", buf);
        }
        int r = fclose(in);
        if (r)
        {
            perror("fopen");
            return EXIT_FAILURE;
        } */
    /*
    // 正規表現で分割 OR strtokで分割
    //
    char* regex = "([[:digit:]]+) ([[:digit:]]+) ?(.*)?";
    char* str = "116 12 25:6911:901:2";
    char** packets={
      "211",
      "232",
      "233",
      "234",
      "235",
      "236",
      "237",
      "238",
      "239",
      "243",
      "244",
      "246",
      "247",
      "248",
      "291",
      "292",
      "293",
      "295",
      "298",
      "299",
      "551",
      "552",
      "555",
      "556",
      "561",
      "611",
      "612",
      "614",
      "615",
      "631",
      "632",
      "634",
      "694",
      NULL
    };
    if(split_by_regex(str, regex) != EXIT_SUCCESS){
      return EXIT_FAILURE;
    }
    */
    return CU_get_error();
}
