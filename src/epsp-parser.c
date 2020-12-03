
/**
 * @file epsp-parser.c
 * @author Teruteru (teruterubouU1024@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-20
 * 
 * @copyright GPL
 * 
 */
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <regex.h>
#include "epsp_parser.h"
#include "string_array.h"
#include "string_list.h"

// validateでだけ使う？
#define LINE_PATTERN "[[:digit:]]{3} [[:digit:]]+( .+)?"

// XXX: 可能ならばグローバル領域に置きたくない
static regex_t line_pattern;
static int pattern_initialized = 0;

void parser_init(void)
{
  if (pattern_initialized == 0)
  {
    int errorcode = regcomp(&line_pattern, LINE_PATTERN, REG_EXTENDED | REG_NEWLINE);
    if (errorcode == 0)
      pattern_initialized = 1;
  }
}

void parser_free(void)
{
  if (pattern_initialized != 0)
  {
    regfree(&line_pattern);
    pattern_initialized = 0;
  }
}

/**
 * failed : 1
 * success : 0
 */
static int parse_internal(epsp_packet_t *packet, const char *line)
{
  // 一応nullチェック
  if (packet == NULL || line == NULL)
  {
    return 1;
  }
  // 予め改行文字(CRLF)は削除する
  // 半角スペースで分割する
  char *code_str = NULL;
  char *hop_str = NULL;
  char *data_str = NULL;
  char *trash_str = NULL;
  {
    // 作業用コピー copy_lineはfreeしてはいけない
    char *work_line = strdupa(line);
    char *catch = NULL;
    code_str = strtok_r(work_line, " ", &catch);
    hop_str = strtok_r(NULL, " ", &catch);
    data_str = strtok_r(NULL, " ", &catch);
    trash_str = strtok_r(NULL, " ", &catch);
  }
  // 分割した結果0個以下または4個以上の場合失敗
  if (code_str == NULL)
  {
    // 0個
    goto clean;
  }
  // パラメータが４つ以上渡されたときにエラーにするか無視するかは考えものだな？
  if (trash_str != NULL)
  {
    // 4個以上
    goto clean;
  }

  // 戻り値
  int ret = 0;

  // 初期値
  packet->code = -1;
  //packet->code_str = NULL;
  packet->hop_count = -1;
  //packet->hop_count_str = NULL;
  packet->data = NULL;

  {
    char *catch = NULL;
    packet->code = strtol(code_str, &catch, 10);
    if (*catch != '\0')
    {
      // 不正な文字が混入した
      ret = 1;
      goto clean;
    }
    if (hop_str != NULL)
    {
      packet->hop_count = strtol(hop_str, &catch, 10);
      if (*catch != '\0')
      {
        // 不正な文字が混入した
        ret = 1;
        goto clean;
      }
    }
  }
  if (data_str != NULL)
  {
    string_array *data = string_array_split(data_str, ":");
    if (data == NULL)
    {
      ret = 1;
      goto clean;
    }
    packet->data = data;
  }
  else
  {
  }
clean:
  return ret;
}

void epsp_packet_free(epsp_packet_t *packet)
{
  if (packet == NULL)
    return;

  packet->code = 0;
  packet->hop_count = 0;
  if (packet->data != NULL)
  {
    size_t len = strlen(packet->data);
    memset(packet->data, 0, len);
    free(packet->data);
    packet->data = NULL;
  }
  if (packet->next)
  {
    epsp_packet_free(packet->next);
    packet->next = NULL;
  }
  free(packet);
}

/**
 * @brief 文字列がスペース区切りで1つ以上3追加であることを検証する.
 * @link https://p2pquake.github.io/epsp-specifications/epsp-specifications.html#base-2
 * 
 * @param line 
 * @return int 
 */
int validate_epsp_packet_line(char *line)
{
  // 毎回コンパイルするのはもったいなくない？->どのタイミングでコンパイルする？
  // validateでだけ正規表現を使って分割はstrdupを使う？
  // sscanf()を使ってもいいかもしれないけどデータ部分どうするんだよ
  // sscanfは空白文字を読めないので不可
  regex_t packet_pattern;
  // "^[[:digit:]]{3} [[:digit:]]+( .+)?$"
  int ret = regcomp(&packet_pattern, "^[[:digit:]]{3}( [[:digit:]]+( .*)?)?$",
                    REG_EXTENDED | REG_NEWLINE | REG_ICASE);
  ret = regexec(&packet_pattern, line, 0, NULL, 0);
  regfree(&packet_pattern);
  char *tmp = strdupa(line);
  if (tmp == NULL)
  {
    perror("strdupa");
    exit(EXIT_FAILURE);
  }
  size_t count = 0;
  char *str = NULL;
  char *token = NULL;
  char *catch = NULL;
  for (str = tmp;; count++, tmp = NULL)
  {
    token = strtok_r(tmp, " ", &catch);
    if (token == NULL)
      break;
  }
  return 1 > count || count > 3;
}

/**
 * @brief 
 * 
 * @param line 
 * @return epsp_packet_t* new epsp packet object
 */
epsp_packet_t *epsp_packet_new(char *line)
{
  // null check
  if (line == NULL)
    return NULL;
  // validate

  // split
  // parse
  // build
  epsp_packet_t *packet = calloc(1, sizeof(epsp_packet_t));
  packet->code = 0;
  packet->hop_count = 0;
  packet->data = NULL;
  packet->next = NULL;
  // return
  return packet;
}

/**
 * epsp_packet_new
 * epsp_packet_free
 * int epsp_packet_parse(epsp_packet_t *, const char *);
 * https://github.com/teruteru128/epsp-peer-cs/blob/master/Client/Common/Net/Packet.cs
 */
int epsp_packet_parse(epsp_packet_t *packet, char *line)
{
  if (pattern_initialized == 0 || line == NULL)
  {
    return NULL;
  }

  /**
   * allocate
   * tryparse(call internal parser)
   * return packet OR Exception
   * epsp_packet_tを使い回すことを前提にするのならmemsetで0埋めなんてとてもじゃないができないんだが
   * TODO: epsp_packet_t使いまわし用スタック
   * いくつかのインスタンスをスタックしておく
   * 使用時：スタックが0なら新規作成して使う
   * 格納時：スタックが16以上(?)なら解放する
   * pthread_mutex_tさんの使い所では！？
   * そもそも単スレッド前提だったら最初に1個作って使い回せばいいんだし
   * あ゛ー……そもそもデータの中に受け取ったら経由数インクリメントして
   * ブロードキャストしないといけないデータがあるんだった、つっら
   * 
   * 改行コードの処理
   * ungetc様万歳！
   * LF:そのまま続行
   * CR:もう1バイト読んでLFなら続行、違ったらungetcして続行
   * CRLF:だいたいお前のせいだよクソ野郎
   */
  memset(packet, 0, sizeof(epsp_packet_t));
  if (parse_internal(packet, line) != 0)
  {
    //failed
    epsp_packet_free(packet);
    return NULL;
  }
}

/*
 * https://p2pquake.github.io/epsp-specifications/epsp-specifications.html
 */
void print_reg_error(int errorcode, regex_t *buf);

/*
  文字列の配列を返す
  https://hg.openjdk.java.net/jdk/jdk12/file/06222165c35f/src/java.base/share/classes/java/lang/String.java#l2274
*/
/*
int split(string_array** dest, regex_t pattern, char* input, size_t limit){
}
*/

string_list *split(char *src, char *regex, int limit)
{
  char ch = 0;
  size_t regex_len = strlen(regex);
  if (((regex_len == 1 && strchr(".$|()[{^?*+\\", ch = regex[0]) == NULL) ||
       (regex_len == 2 && regex[0] == '\\' && (((ch = regex[1]) - '0') | ('9' - ch)) < 0 && ((ch - 'a') | ('z' - ch)) < 0 && ((ch - 'A') | ('Z' - ch)) < 0)))
  {
    int off = 0;
    int next = 0;
    int limited = limit > 0;
    string_list *list = malloc(sizeof(string_list));
    while ((next = regex[off]) != '\0')
    {
      if (!limited || string_list_size(list) < limit - 1)
      {
        char *s = malloc(next - off + 1);
        strncpy(s, regex, next - off);
        s[next - off] = 0;
        string_list_add(list, s);
      }
      else
      {
      }
    }
  }
  return NULL;
}

int split_regex(string_array **dest, char *pattern, char *str, size_t limit)
{
}

int split_strtok(string_array **dest, char *delim, char *src, size_t limit)
{
}

/*
  プロトコルフォーマットに合致しているか検査しないといけないので面倒くさい可能性が高い
*/
int split_by_strtok(const char *str)
{
  char *target = strdup(str);

  free(target);
  return EXIT_FAILURE;
}

void free_string_array(string_array *str)
{
}
int split_by_regex(char *str, char *regex)
{
  return EXIT_SUCCESS;
}
