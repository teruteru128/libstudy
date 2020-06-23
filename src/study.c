
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "gettext.h"
#define _(str) gettext(str)
#include <locale.h>
#include <stdio.h>

int study_helloworld(void)
{
    printf(_("Hello World!\n"));
    return 0;
}
