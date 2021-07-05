
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>

#include "args.h"
#include "bitmessage.h"
#include "bm.h"

int BM_init(void)
{
    return 1;
}

static int bm_init()
{
    return 0;
}

static int bm_cleanup()
{
    return 0;
}

static int global_init()
{
    return 0;
}

static int global_cleanup()
{
    return 0;
}

static int arg_parse(args_t *arg, int argc, char **argv)
{
    return 0;
}

static char *get_config_filepath()
{
    return NULL;
}

/**
 *  --xxx arg
 *  --xxx=arg
 *  default arg
 *  short arg
 * */
static int config_parse(char *filename)
{
    return 0;
}

static char *getString()
{
    return NULL;
}

static int getInt()
{
    return 0;
}
