
#include <stdio.h>

typedef struct string_array string_array;
typedef struct string_array{
  /* memory area */
  char** str;
  /* num of strings */
  size_t size;
  /* memory area length */
  size_t length;
} string_array;
