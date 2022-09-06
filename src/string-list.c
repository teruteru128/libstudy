
#include "string-list.h"
#include "internal-string-list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static string_list *peek_last_node(string_list *tmp)
{
    while (tmp->next != NULL)
    {
        tmp = tmp->next;
    }
    return tmp;
}

/*
 * リストの末尾に要素を追加します
 * @param dest 末尾に追加されるリスト。NULLの場合、リストを新規作成します
 * @param str 追加される要素。常に新しい領域に複製されます
 *
 * @return 要素が追加されたリスト。
 */
string_list *string_list_add(string_list *dest, char *str)
{
    string_list *last = NULL;
    if (dest == NULL)
    {
        // 新規作成
        dest = last = malloc(sizeof(string_list));
    }
    else
    {
        // 末尾に新しいノードを追加する
        last = peek_last_node(dest);
        last->next = malloc(sizeof(string_list));
        last = last->next;
    }

    last->next = NULL;
    const size_t len = strlen(str) + 1;
    last->str = malloc(len);
    memcpy(last->str, str, len);
    return dest;
}

size_t string_list_size(string_list *list)
{
    if (list == NULL)
    {
        return 0;
    }
    string_list *tmp = list;
    size_t count = 1;
    while (tmp->next != NULL)
    {
        count++;
        tmp = tmp->next;
    }
    return count;
}

string_list *str_split(char *str, char *delim) { return NULL; }

void string_list_free(string_list *list)
{
    if (list == NULL)
    {
        return;
    }
    string_list *tmp = list;
    while (tmp->next != NULL)
    {
        free(tmp->str);
        tmp->str = NULL;
        string_list *tmp_b = tmp; // 一つ前のノードを取る
        tmp = tmp->next;
        free(tmp_b);
    }
    // free(tmp->str);
    tmp->str = NULL;
    free(tmp);
}

void string_list_foreach(string_list *list, void (*func)(const char *))
{
    string_list *p = list;
    while (p->next != NULL)
    {
        func(p->str);
        p = p->next;
    }
}

void string_list_foreach_r(string_list *list,
                           void (*func)(const char *, void *), void *arg)
{
    string_list *p = list;
    while (p->next != NULL)
    {
        func(p->str, arg);
        p = p->next;
    }
}
