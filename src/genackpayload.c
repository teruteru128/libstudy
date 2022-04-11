
#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>

int genAckPayload(int streamNumber, int stealthLevel, unsigned char **payload,
                  size_t *payloadlen)
{
    int acktype = 0;
    int version = 0;
    unsigned char *ackdata = NULL;
    size_t ackdatalen = 0;

    if (stealthLevel == 2)
    {
        unsigned char priv[32];
        getrandom(priv, 32, GRND_NONBLOCK);
        unsigned char *pubkey;
        unsigned char *dummyMessage;
        acktype = 2;
        version = 1;
    }
    else if (stealthLevel == 1)
    {
        ackdatalen = 32;
        ackdata = malloc(ackdatalen);
        getrandom(ackdata, ackdatalen, GRND_NONBLOCK);
        acktype = 0;
        version = 4;
    }
    else if (stealthLevel == 0)
    {
        ackdatalen = 32;
        ackdata = malloc(ackdatalen);
        getrandom(ackdata, ackdatalen, GRND_NONBLOCK);
        acktype = 2;
        version = 1;
    }

    return 0;
}
