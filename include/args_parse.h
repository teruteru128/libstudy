
#ifndef ARGS_PARSE_H
#define ARGS_PARSE_H

#include "args.h"

args_t *args_new(void);
int args_free(args_t *args);
void parse_args(args_t *args, int argc, const char **argv);

#endif
