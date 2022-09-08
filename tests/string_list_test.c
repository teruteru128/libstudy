
#include <stdio.h>
#include <string-list.h>

void func(const char *element) { printf("%s\n", element); }
void funcv(const char *element, void *arg)
{
    printf("%s, %s\n", element, (char *)arg);
}

int main(int argc, char *argv[])
{
    string_list *list = string_list_add(NULL, "aaa");
    list = string_list_add(list, "bbb");
    list = string_list_add(list, "ccc");
    string_list_foreach(list, func);
    string_list_foreach_r(list, funcv, "yeah!");
    printf("%ld\n", string_list_size(list));
    string_list_free(list);
    return 0;
}
